// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/WeaponBase.h"
#include "Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "Weapons/KnifeWeapon.h"
#include "Math/Color.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	RootComponent = GunMesh;
	GunMesh->CastShadow = false;

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(GunMesh);

	GunMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GunMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	GunMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,ECollisionResponse::ECR_Ignore);
	GunMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera,ECollisionResponse::ECR_Ignore);

}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	PlayerCamera = PlayerCharacter->GetCamera();
	SingleShotAlpha = 1.f;//used to add randomness when firing auto
	bCanShoot = true;
	bIsReloading = false;
	bIsWeaponShootable = true;
	GunMesh->SetSimulatePhysics(true);
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TSubclassOf<AWeaponBase> AWeaponBase::GetPickupWeapon()
{
	return WeaponBP;
}

void AWeaponBase::DrawWeapon()
{
	bCanShoot = false;
	if (WeaponDrawMontage)
	{
		float DrawTime = PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(WeaponDrawMontage);
		FTimerHandle DrawTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(
			DrawTimerHandle, [&]()
			{ bCanShoot = true; },
			DrawTime, false);
	}
	else
	{
		bCanShoot = true;
	}
	UpdateWeaponVisuals();
}
void AWeaponBase::Shoot()
{
	if (bIsWeaponShootable)
	{
		if(!ShootingTimerHandle.IsValid())
		GetWorld()->GetTimerManager().SetTimer(ShootingTimerHandle, this, &AWeaponBase::ShootingInAction, FireRate, bIsAuto,0.f);
	}
}

void AWeaponBase::StopShooting()
{
	if(GetWorld()->GetTimerManager().IsTimerActive(ShootingCooldownTimerHandle))
	{
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(ShootingCooldownTimerHandle,[&](){

	if (ShootingTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ShootingTimerHandle);
	};
	},CoolDownTime,false);

	SingleShotAlpha = 0.01f;
}

void AWeaponBase::ShootingInAction()
{
	if (bCanShoot && !PlayerCharacter->bIsDoingMeleeAttack)
	{
		if (CurrentMagAmmo > 0)
		{
			CurrentMagAmmo--;
			
			if (GunShootAnim)
				GunMesh->PlayAnimation(GunShootAnim, false);

			if (PlayerCharacter->IsADSButtonDown() && PlayerAdsShootingMontage)
			{
				PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(PlayerAdsShootingMontage);
			}

			else
			{
				if (PlayerCrouchShootMontage && ((PlayerCharacter->GetCurrentMovementType() == EMovementType::EMT_Crouching) || PlayerCharacter->GetCurrentMovementType() == EMovementType::EMT_Sliding))
				{
					PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(PlayerCrouchShootMontage);
				}
				else if (PlayerShootMontage)
				{
					PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(PlayerShootMontage);
				}
			}

			if (TracerRound)
			{
				GetWorld()->SpawnActor<AActor>(TracerRound, Muzzle->GetComponentLocation(), PlayerCamera->GetForwardVector().Rotation())->SetLifeSpan(1.0f);
			}

			Recoil();

			if (CameraShake)
			{
				PlayerCharacter->PlayCameraShake(CameraShake, CameraShakeScale);
			}

			SingleShotAlpha = 1.f;
			UpdateWeaponVarsInPlayer();
			UpdateWeaponVisuals();
			UAISense_Hearing::ReportNoiseEvent(this, GetActorLocation(), 1.f, PlayerCharacter, 2000.f);
			for(int i = 0 ; i < PalletCount; i++)
			{
				FVector RandomVector = FVector(UKismetMathLibrary::RandomFloatInRange(-TraceOffset,TraceOffset),UKismetMathLibrary::RandomFloatInRange(-TraceOffset,TraceOffset),UKismetMathLibrary::RandomFloatInRange(-TraceOffset,TraceOffset));
				FVector EndTrace = PlayerCamera->GetComponentLocation() + PlayerCamera->GetForwardVector() * Range + RandomVector;
				TArray<AActor *> ActorsToIgnore;
				ActorsToIgnore.Add(UGameplayStatics::GetPlayerCharacter(this, 0));
				FHitResult GunShotHitResult;

				bool bIsHit = UKismetSystemLibrary::LineTraceSingle(this,
																	PlayerCamera->GetComponentLocation(),
																	EndTrace,
																	UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
																	false,
																	ActorsToIgnore,
																	EDrawDebugTrace::ForDuration,
																	GunShotHitResult,
																	true);
				
				if(bIsHit)
				{
					HitSound(GunShotHitResult);
					AActor *HitActor = GunShotHitResult.GetActor();
					if (HitActor)
					{
						if (HitActor->CanBeDamaged())
						{
							GiveDamage(GunShotHitResult);
						}
					}
				}
			}
		}
		else
		{
			StopShooting();
			PlayerCharacter->Reload();
		}
	}
}

void AWeaponBase::HitSound(FHitResult GunHit)
{
	EPhysicalSurface SurfaceType = 	UGameplayStatics::GetSurfaceType(GunHit);
	switch (SurfaceType)
	{
		//wood
	case EPhysicalSurface::SurfaceType1:
		UE_LOG(LogTemp,Warning,TEXT("Surface 1"));
		UGameplayStatics::SpawnEmitterAtLocation(this,WoodHitParticle,GunHit.Location,GunHit.Normal.Rotation());
		UGameplayStatics::PlaySoundAtLocation(this,WoodHit,GunHit.Location);
		break;

		//Metal
	case EPhysicalSurface::SurfaceType2 :
		UE_LOG(LogTemp,Warning,TEXT("Surface 2"));
		UGameplayStatics::SpawnEmitterAtLocation(this,MetalHitParticle,GunHit.Location,GunHit.Normal.Rotation());
		UGameplayStatics::PlaySoundAtLocation(this,MetalHit,GunHit.Location);
		break;

		//Stone
	case EPhysicalSurface::SurfaceType3:
		UE_LOG(LogTemp,Warning,TEXT("Surface 3"));
		UGameplayStatics::SpawnEmitterAtLocation(this,StoneHitParticle,GunHit.Location,GunHit.Normal.Rotation());
		UGameplayStatics::PlaySoundAtLocation(this,StoneHit,GunHit.Location);
		break;

		//HeavyMetal
	case EPhysicalSurface::SurfaceType4 :
		UE_LOG(LogTemp,Warning,TEXT("Surface 4"));
		UGameplayStatics::SpawnEmitterAtLocation(this,MetalHitParticle,GunHit.Location,GunHit.Normal.Rotation());
		UGameplayStatics::PlaySoundAtLocation(this,MetalHit,GunHit.Location);
		break;
	
	default:
		break;
	}
}

void AWeaponBase::Recoil()
{
	FVector PlayerVelocity = PlayerCharacter->GetVelocity();
	float MovementRecoilAlpha = 1.f;
	EMovementType MovementType = PlayerCharacter->GetCurrentMovementType();

	if (MovementType != EMovementType::EMT_WallRun && PlayerCharacter->IsInAir())
	{
		MovementRecoilAlpha = 2.f;
	}
	else if (PlayerVelocity.Size() > 2.f)
	{
		switch (MovementType)
		{
		case EMovementType::EMT_Walking:
			MovementRecoilAlpha = 1.2f;
			break;

		case EMovementType::EMT_Sprinting:
			MovementRecoilAlpha = 1.6f;
			break;

		case EMovementType::EMT_Crouching:
			MovementRecoilAlpha = 1.1f;
			break;

		case EMovementType::EMT_WallRun:
			MovementRecoilAlpha = 1.8f;
			break;
		}
	}

	float RecoilValue = SingleShotAlpha * MovementRecoilAlpha * UKismetMathLibrary::RandomFloatInRange(-0.2, 0.2) * Accuracy;
	PlayerCharacter->AddControllerYawInput(RecoilValue);
	PlayerCharacter->AddControllerPitchInput(RecoilValue);
}
void AWeaponBase::Reload()
{
	if (bIsWeaponShootable)
	{
		if (!(CurrentMagAmmo == MagSize || TotalAmmo <= 0) && !bIsReloading)
		{
			bCanShoot = false;
			if (GunReloadAnim && PlayerReloadMontage)
			{
				bIsReloading = true;
				PlayerCharacter->bIsReloading = true;
				GunMesh->PlayAnimation(GunReloadAnim, false);
				ReloadTime = PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(PlayerReloadMontage);
				FTimerHandle ReloadTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AWeaponBase::ReloadEffect, ReloadTime, false);
			}
		}
	}
}

void AWeaponBase::ReloadEffect()
{
	if (MagSize < (CurrentMagAmmo + TotalAmmo))
	{
		TotalAmmo -= (MagSize - CurrentMagAmmo);
		CurrentMagAmmo = MagSize;
	}
	else
	{
		CurrentMagAmmo += TotalAmmo;
		TotalAmmo = 0;
	}

	bIsReloading = false;
	PlayerCharacter->bIsReloading = false;
	bCanShoot = true;

	if (PlayerCharacter->IsADSButtonDown())
	{
		PlayerCharacter->ADSON();
	}

	UpdateWeaponVisuals();
	UpdateWeaponVarsInPlayer();
}

void AWeaponBase::UpdateWeaponVisuals()
{
	float ColorAlpha = (float)CurrentMagAmmo / (float)MagSize;
	FLinearColor NewColor = FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor::Blue, ColorAlpha);
	NewColor = UKismetMathLibrary::Multiply_LinearColorFloat(NewColor, 10.f);
	GunMesh->CreateDynamicMaterialInstance(0)->SetVectorParameterValue(FName("EmissiveColor"), NewColor);
}

void AWeaponBase::UpdateWeaponVarsInPlayer()//for updating the struct 	 inside the player character to used them in the player hud
{
	if (bIsPrimaryWeapon)
	{
		PlayerCharacter->PrimaryWeapon.TotalAmmo = TotalAmmo;
		PlayerCharacter->PrimaryWeapon.CurrentMagAmmo = CurrentMagAmmo;
	}

	else
	{
		PlayerCharacter->SecondaryWeapon.TotalAmmo = TotalAmmo;
		PlayerCharacter->SecondaryWeapon.CurrentMagAmmo = CurrentMagAmmo;
	}
}

float AWeaponBase::DoMeleeAttack()
{
	float MeleeEndTime = 0;
	if (MeleeAttackMontage)
	{
		MeleeEndTime = PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(MeleeAttackMontage, 1.6f, EMontagePlayReturnType::Duration);
	}
	return MeleeEndTime;
}

void AWeaponBase::GiveDamage(FHitResult GunHit)
{
	float FinalDamage = DamagePerBone(GunHit.BoneName);
	UGameplayStatics::ApplyPointDamage(GunHit.GetActor(),
									   FinalDamage,
									   GunHit.TraceStart,
									   GunHit,
									   PlayerCharacter->GetInstigatorController(),
									   this,
									   UDamageType::StaticClass());

	// UE_LOG(LogTemp,Warning,TEXT("Damage Given %f"),FinalDamage);
	UAISense_Damage::ReportDamageEvent(this, GunHit.GetActor(), PlayerCharacter, FinalDamage, GunHit.TraceStart, GunHit.Location);
}

float AWeaponBase::DamagePerBone(FName BoneName)
{
	// UE_LOG(LogTemp,Warning,TEXT("%s"),*BoneName.ToString());
	if (BoneName == FName("neck_01"))
	{
		return HeadshotDamage;
	}

	else if (BoneName == FName("spine_01") || BoneName == FName("spine_02") || BoneName == FName("spine_03"))
	{
		return CriticalDamage;
	}

	return BaseDamage;
}

//=============================================================Pickup Weapon=============================================//
void AWeaponBase::PickupWeapon()
{
	//just a cross check to make sure player cannot pickup already holding weapon
	if (UKismetMathLibrary::EqualEqual_ClassClass(PlayerCharacter->PrimaryWeapon.WeaponClass.Get(), WeaponBP.Get()) &&
		UKismetMathLibrary::EqualEqual_ClassClass(PlayerCharacter->SecondaryWeapon.WeaponClass.Get(), WeaponBP.Get()))
	{
		return;
	}
	// FWeaponData Data;	
	bIsPlayerHoldingTheWeapon = true;
	GunMesh->SetSimulatePhysics(false);
	GunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//using this to save the data inside the player, so that we can use it in widgets and during save game
	FWeaponData WeaponData;
	WeaponData.WeaponName = WeaponName;
	WeaponData.WeaponClass = WeaponBP;
	WeaponData.TotalAmmo = TotalAmmo;
	WeaponData.MaxAmmo = MaxAmmo;
	WeaponData.CurrentMagAmmo = CurrentMagAmmo;
	
	//first we will check of the empty slot and assign the weapon to it
	if (PlayerCharacter->PrimaryWeapon.WeaponClass->IsChildOf(AKnifeWeapon::StaticClass()))
	{
		PlayerCharacter->WeaponEquippedSlot = 0;
		PlayerCharacter->DropWeapon(true);
		PlayerCharacter->PrimaryWeapon = WeaponData;
	}

	else if (PlayerCharacter->SecondaryWeapon.WeaponClass->IsChildOf(AKnifeWeapon::StaticClass()))
	{
		PlayerCharacter->WeaponEquippedSlot = 1;
		PlayerCharacter->DropWeapon(false);
		PlayerCharacter->SecondaryWeapon = WeaponData;
	}

	else
	{
		if (PlayerCharacter->WeaponEquippedSlot == 0)
		{
			PlayerCharacter->DropWeapon(true);
			PlayerCharacter->PrimaryWeapon = WeaponData;
		}

		else if (PlayerCharacter->WeaponEquippedSlot == 1)
		{
			PlayerCharacter->DropWeapon(false);
			PlayerCharacter->SecondaryWeapon = WeaponData;
		}
	}

	PlayerCharacter->SwitchWeapon(true);
}

void AWeaponBase::SetPickupWeaponName()
{
	PlayerCharacter->PickupWeaponName = WeaponName;
}

bool AWeaponBase::IsPickupGun()
{
	return true;
}

void AWeaponBase::DropGun()
{
	GunMesh->SetSimulatePhysics(true);
	GunMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}