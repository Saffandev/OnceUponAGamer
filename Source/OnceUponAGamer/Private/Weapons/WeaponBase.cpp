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

// Sets default values
AWeaponBase::AWeaponBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	RootComponent = GunMesh;
	GunMesh->CastShadow = false;

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(GunMesh);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	PlayerCamera = PlayerCharacter->GetCamera();
	SingleShotAlpha = 1.f;
	// CurrentMagAmmo = MagSize;
	// TotalAmmo = MaxAmmo;
	bCanShoot = true;
	bIsReloading = false;
	bIsWeaponShootable = true;
	GunMesh->SetSimulatePhysics(true);
}

// Called every frame
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
		// UE_LOG(LogTemp,Warning,TEXT("Shooting called"));

		GetWorld()->GetTimerManager().SetTimer(ShootingTimerHandle, this, &AWeaponBase::ShootingInAction, FireRate, true, 0);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Not Shootable"));
}

void AWeaponBase::StopShooting()
{
	if (ShootingTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ShootingTimerHandle);
	}
	SingleShotAlpha = 0.01f;
}

void AWeaponBase::ShootingInAction()
{
	if (bCanShoot && !PlayerCharacter->bIsDoingMeleeAttack)
	{
		if (CurrentMagAmmo > 0)
		{
			CurrentMagAmmo--;
			FVector EndTrace = PlayerCamera->GetComponentLocation() + PlayerCamera->GetForwardVector() * Range;
			TArray<AActor *> ActorsToIgnore;
			ActorsToIgnore.Add(UGameplayStatics::GetPlayerCharacter(this, 0));
			FHitResult GunShotHitResult;
			bool bIsHit = UKismetSystemLibrary::LineTraceSingle(this,
												  PlayerCamera->GetComponentLocation(),
												  EndTrace,
												  UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
												  false,
												  ActorsToIgnore,
												  EDrawDebugTrace::None,
												  GunShotHitResult,
												  true);
			
			if (ShootAnim)
				GunMesh->PlayAnimation(ShootAnim, false);

			if (PlayerCharacter->IsADSButtonDown())
			{
				if (PlayerAdsShootingMontage)
					PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(PlayerAdsShootingMontage);
			}
			else
			{
				if (PlayerShootMontage && !((PlayerCharacter->GetCurrentMovementType() == EMovementType::EMT_Crouching) || PlayerCharacter->GetCurrentMovementType() == EMovementType::EMT_Sliding))
				{
					PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(PlayerShootMontage);
				}
				else if (PlayerCrouchShootMontage && ((PlayerCharacter->GetCurrentMovementType() == EMovementType::EMT_Crouching) || PlayerCharacter->GetCurrentMovementType() == EMovementType::EMT_Sliding))
				{
					PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(PlayerCrouchShootMontage);
				}
			}
			if (TracerRound)
				GetWorld()->SpawnActor<AActor>(TracerRound, Muzzle->GetComponentLocation(), PlayerCamera->GetForwardVector().Rotation())->SetLifeSpan(1.0f);

			Recoil();
			if (CameraShake)
			{
				PlayerCharacter->PlayCameraShake(CameraShake, CameraShakeScale);
			}
			SingleShotAlpha = 1.f;
			UpdateWeaponVarsInPlayer();
			UpdateWeaponVisuals();
			UAISense_Hearing::ReportNoiseEvent(this, GetActorLocation(), 1.f, PlayerCharacter, 2000.f);
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
		else
		{
			// UE_LOG(LogTemp,Warning,TEXT("No ammo"));
			StopShooting();
			PlayerCharacter->Reload();
		}
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannnot shoot"));
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
				UGameplayStatics::SpawnEmitterAtLocation(this,WoodHitParticle,GunHit.Location);
				UGameplayStatics::PlaySoundAtLocation(this,WoodHit,GunHit.Location);
				break;

				//Metal
			case EPhysicalSurface::SurfaceType2:
				UE_LOG(LogTemp,Warning,TEXT("Surface 2"));
				UGameplayStatics::SpawnEmitterAtLocation(this,MetalHitParticle,GunHit.Location);
				UGameplayStatics::PlaySoundAtLocation(this,MetalHit,GunHit.Location);
				break;

				//Stone
			case EPhysicalSurface::SurfaceType3:
				UE_LOG(LogTemp,Warning,TEXT("Surface 3"));
				UGameplayStatics::SpawnEmitterAtLocation(this,StoneHitParticle,GunHit.Location);
				UGameplayStatics::PlaySoundAtLocation(this,StoneHit,GunHit.Location);
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
		PlayerCharacter->ADSON();
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

void AWeaponBase::UpdateWeaponVarsInPlayer()
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
}

void AWeaponBase::PickupWeaponSetup(EWeaponName LWeaponName, TSubclassOf<AWeaponBase> LWeaponClass, int LTotalAmmo, int LMaxAmmo, int LMagSize, int LCurrentMagAmmo, float LFireRate)
{

	if (UKismetMathLibrary::EqualEqual_ClassClass(PlayerCharacter->PrimaryWeapon.WeaponClass.Get(), LWeaponClass.Get()) &&
		UKismetMathLibrary::EqualEqual_ClassClass(PlayerCharacter->SecondaryWeapon.WeaponClass.Get(), LWeaponClass.Get()))
	{
		return;
	}
	bIsPlayerHoldingTheWeapon = true;
	GunMesh->SetSimulatePhysics(false);
	GunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (PlayerCharacter->PrimaryWeapon.WeaponClass->IsChildOf(AKnifeWeapon::StaticClass()))
	{
		UE_LOG(LogTemp,Warning,TEXT("Primary Weapon Pickup in empty slot"));
		PlayerCharacter->WeaponEquippedSlot = 0;
		PlayerCharacter->DropWeapon(true);
		PlayerCharacter->PrimaryWeapon.WeaponName = LWeaponName;
		PlayerCharacter->PrimaryWeapon.WeaponClass = LWeaponClass;
		PlayerCharacter->PrimaryWeapon.TotalAmmo = LTotalAmmo;
		PlayerCharacter->PrimaryWeapon.MaxAmmo = LMaxAmmo;
		PlayerCharacter->PrimaryWeapon.MagSize = LMagSize;
		PlayerCharacter->PrimaryWeapon.CurrentMagAmmo = LCurrentMagAmmo;
		PlayerCharacter->PrimaryWeapon.FireRate = LFireRate;
	}

	else if (PlayerCharacter->SecondaryWeapon.WeaponClass->IsChildOf(AKnifeWeapon::StaticClass()))
	{
		UE_LOG(LogTemp,Warning,TEXT("Secondary Weapon Pickup in empty slot"));
		PlayerCharacter->WeaponEquippedSlot = 1;
		PlayerCharacter->DropWeapon(false);
		PlayerCharacter->SecondaryWeapon.WeaponClass = LWeaponClass;
		PlayerCharacter->SecondaryWeapon.TotalAmmo = LTotalAmmo;
		PlayerCharacter->SecondaryWeapon.MaxAmmo = LMaxAmmo;
		PlayerCharacter->SecondaryWeapon.MagSize = LMagSize;
		PlayerCharacter->SecondaryWeapon.CurrentMagAmmo = LCurrentMagAmmo;
		PlayerCharacter->SecondaryWeapon.FireRate = LFireRate;
	}

	else
	{
		if (PlayerCharacter->WeaponEquippedSlot == 0)
		{
			UE_LOG(LogTemp,Warning,TEXT("Primary Weapon Pickup"));
			PlayerCharacter->DropWeapon(true);
			PlayerCharacter->PrimaryWeapon.WeaponClass = LWeaponClass;
			PlayerCharacter->PrimaryWeapon.TotalAmmo = LTotalAmmo;
			PlayerCharacter->PrimaryWeapon.MaxAmmo = LMaxAmmo;
			PlayerCharacter->PrimaryWeapon.MagSize = LMagSize;
			PlayerCharacter->PrimaryWeapon.CurrentMagAmmo = LCurrentMagAmmo;
			PlayerCharacter->PrimaryWeapon.FireRate = LFireRate;
		}

		else if (PlayerCharacter->WeaponEquippedSlot == 1)
		{
			UE_LOG(LogTemp,Warning,TEXT("Secondary Weapon Pickup"));
			PlayerCharacter->DropWeapon(false);
			PlayerCharacter->SecondaryWeapon.WeaponClass = LWeaponClass;
			PlayerCharacter->SecondaryWeapon.TotalAmmo = LTotalAmmo;
			PlayerCharacter->SecondaryWeapon.MaxAmmo = LMaxAmmo;
			PlayerCharacter->SecondaryWeapon.MagSize = LMagSize;
			PlayerCharacter->SecondaryWeapon.CurrentMagAmmo = LCurrentMagAmmo;
			PlayerCharacter->SecondaryWeapon.FireRate = LFireRate;
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