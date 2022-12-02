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
#include "Particles/ParticleSystemComponent.h"
#include "HelperFunctions/ViolenceRegistration.h"
#include "Components/AudioComponent.h"
AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	RootComponent = GunMesh;
	GunMesh->CastShadow = false;

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(GunMesh);

	SightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SightMesh"));
	SightMesh->SetupAttachment(GunMesh,FName("ADS"));
	GunMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GunMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	GunMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,ECollisionResponse::ECR_Ignore);
	GunMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera,ECollisionResponse::ECR_Ignore);
	bIsWeaponShootable = true;

}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	PlayerCamera = PlayerCharacter->GetCamera();
	SingleShotAlpha = 0.f;//used to add randomness when firing auto
	bCanShoot = true;
	bIsReloading = false;
	GunMesh->SetSimulatePhysics(true);
	SetActorTickEnabled(bCanTick);

	if (GunChargeCurve)
	{
		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic TimelineEndCallback;
		TimelineCallback.BindUFunction(this, FName("ChargeGun"));
		TimelineEndCallback.BindUFunction(this, FName("ReleaseCharge"));
		GunChargeTimeline.AddInterpFloat(GunChargeCurve, TimelineCallback);
		GunChargeTimeline.SetTimelineFinishedFunc(TimelineEndCallback);
	}
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GunChargeTimeline.TickTimeline(DeltaTime);
}

TSubclassOf<AWeaponBase> AWeaponBase::GetPickupWeapon()
{
	return WeaponBP;
}

void AWeaponBase::DrawWeapon(float Drawspeed)
{
	bCanShoot = false;
	if (WeaponDrawMontage)
	{
		float DrawTime = PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(WeaponDrawMontage,Drawspeed) * Drawspeed;
		SetActorHiddenInGame(true);
		FTimerHandle DrawTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(
			DrawTimerHandle, [&]()
			{ bCanShoot = true; },
			DrawTime, false);
		
		FTimerHandle VisibilityTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(VisibilityTimerHandle,[&](){SetActorHiddenInGame(false);},0.1f,false);
	}
	else
	{
		bCanShoot = true;
	}
	UpdateWeaponVisuals();
	
}

void AWeaponBase::UnDrawWeapon(float DrawSpeed)
{
	bCanShoot = false;
	if(WeaponUnDrawMontage)
	{
		PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(WeaponUnDrawMontage,DrawSpeed) ;
	}
}


void AWeaponBase::SetCanShoot(bool bLCanShoot)
{
	bCanShoot = bLCanShoot;
}

void AWeaponBase::Shoot()
{
	if (bIsWeaponShootable && bCanShoot && !bIsReloading)
	{
		if (CurrentMagAmmo <= 0)
		{
			StopShooting();
			PlayerCharacter->Reload();
			return;
		}

		if (!bIsCharged)
		{
			ShootingInAction();
		}

		if (!ShootingTimerHandle.IsValid())
		{
			if (bIsAuto)
			{
				GetWorld()->GetTimerManager().SetTimer(ShootingTimerHandle, this, &AWeaponBase::ShootingInAction, FireRate, bIsAuto);
			}
			else if (bIsCharged)
			{
				bIsReversingTheCharge = false;
				if (ChargingSound)
				{
					ChargingSoundComp = UGameplayStatics::SpawnSoundAttached(ChargingSound, GunMesh);
					ChargingSoundComp->SetVolumeMultiplier(0.f);
				}
				GunChargeTimeline.PlayFromStart();
			}
		}

		bCanShoot = false;
	}
	else
	{
	//	UE_LOG(LogTemp, Warning, TEXT("No Shoot"));
	}
}

void AWeaponBase::StopShooting()
{
	if (bIsReloading || GetWorld()->GetTimerManager().IsTimerActive(ShootingCooldownTimerHandle))
	{
		return;
	}
	
	/*if (ChargingSoundComp )
	{
		ChargingSoundComp->Stop();
	}*/

	if (bIsGunShot)
	{
		GetWorld()->GetTimerManager().SetTimer(ShootingCooldownTimerHandle,[&]()
		{
			if (ShootingTimerHandle.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(ShootingTimerHandle);
			}; bCanShoot = true;
		},
			CoolDownTime,false);
		if (bIsCharged)
		{
			bIsReversingTheCharge = true;
			GunChargeTimeline.ReverseFromEnd();
		}
	}
	else
	{
		bCanShoot = true;
		if (bIsCharged)
		{
			GunChargeTimeline.Stop();
		}
	}

	bIsGunShot = false;
	/*if (bIsCharged)
	{
		if (ShootingTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(ShootingTimerHandle);
		}
		bCanShoot = true;
	}*/
	PlayerCharacter->bIsShooting = false;

	SingleShotAlpha = 0.0f;
}

void AWeaponBase::ChargeGun()
{
	float Alpha = GunChargeCurve->GetFloatValue(GunChargeTimeline.GetPlaybackPosition());
	if (ChargingSoundComp)
	{
		ChargingSoundComp->SetVolumeMultiplier(Alpha);
	}
	UMaterialInstanceDynamic* GunMaterialInstance = GunMesh->CreateDynamicMaterialInstance(0);
	if (GunMaterialInstance)
	{
		FMaterialParameterInfo MatInfo;
		MatInfo.Name = FName("EmissiveBrightness");
		float CurrentEmissiveValue;

		UE_LOG(LogTemp, Warning, TEXT("Alpha : %f"), CurrentEmissiveValue);
		GunMaterialInstance->GetScalarParameterValue(MatInfo, CurrentEmissiveValue);
		float NewEmissive = FMath::Lerp(8.f, 300.f,Alpha);
		GunMaterialInstance->SetScalarParameterValue(FName("EmissiveBrightness"), NewEmissive);
	}
}

void AWeaponBase::ReleaseCharge()
{
	if (bIsReversingTheCharge)
	{
		return;
	}
	ShootingInAction();
}

void AWeaponBase::ShootingInAction()
{
	//UE_LOG(LogTemp, Warning, TEXT("Shooooooooooooooot"));
	if (!PlayerCharacter->bIsDoingMeleeAttack)
	{
		if (CurrentMagAmmo > 0)
		{
			CurrentMagAmmo--;
			PlayerCharacter->bIsShooting = true;
			bIsGunShot = true;
			if (GunShootAnim)
				GunMesh->PlayAnimation(GunShootAnim, false);

			if (PlayerShootMontage)
			{
				PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(PlayerShootMontage);
			}

			if (TracerRound)
			{
				GetWorld()->SpawnActor<AActor>(TracerRound, Muzzle->GetComponentLocation(), PlayerCamera->GetForwardVector().Rotation())->SetLifeSpan(1.0f);
			}


			if (CameraShake)
			{
				float CameraShakeValue = PlayerCharacter->IsADSButtonDown() ? CameraShakeScaleADS : CameraShakeScale;
				PlayerCharacter->PlayCameraShake(CameraShake, CameraShakeValue	);
			}

			UpdateWeaponVarsInPlayer();
			UpdateWeaponVisuals();
			UAISense_Hearing::ReportNoiseEvent(this, GetActorLocation(), 1.f, PlayerCharacter, 2000.f);
			for(int i = 0 ; i < PalletCount; i++)
			{
				FVector RandomVector = FVector(UKismetMathLibrary::RandomFloatInRange(-TraceOffset,TraceOffset),UKismetMathLibrary::RandomFloatInRange(-TraceOffset,TraceOffset),UKismetMathLibrary::RandomFloatInRange(-TraceOffset,TraceOffset));
				FVector EndTrace = PlayerCamera->GetComponentLocation() + PlayerCamera->GetForwardVector() * Range + RandomVector + Recoil();
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
				
				if(TrailsFX)
				{
					UParticleSystemComponent* TrailsFXComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),TrailsFX,Muzzle->GetComponentLocation());
					if(TrailsFXComp)
					{
						FVector EndPoint = bIsHit ? GunShotHitResult.ImpactPoint : EndTrace;
						TrailsFXComp->SetVectorParameter(FName("ShockBeamEnd"),EndPoint);
					}
				}
				if(bIsHit)
				{
					HitEffect(GunShotHitResult);
					AActor *HitActor = GunShotHitResult.GetActor();
					if (HitActor)
					{
						if (HitActor->CanBeDamaged())
						{
							// UViolenceRegistration::RegisterViolence(this,GetActorLocation(),PlayerCharacter,EViolenceType::EVT_HittedSomethingDamaging);
							GiveDamage(GunShotHitResult);
						}
						/*else
						{
							UViolenceRegistration::RegisterViolence(this,GetActorLocation(),PlayerCharacter,EViolenceType::EVT_HittedSomethingNonDamaging);
						}*/
					}
				}

				/*else
				{
					UViolenceRegistration::RegisterViolence(this,GetActorLocation(),PlayerCharacter,EViolenceType::EVT_AirFire);
				}*/
			}
			SingleShotAlpha = 1.f;

		}
		else
		{
			StopShooting();
			PlayerCharacter->Reload();
		}
	}
}

void AWeaponBase::HitEffect(FHitResult GunHit)
{
	EPhysicalSurface SurfaceType = 	UGameplayStatics::GetSurfaceType(GunHit);
	switch (SurfaceType)
	{
		//wood
	case EPhysicalSurface::SurfaceType1:
		ImpactEffect(GunHit,WoodHitParticle,WoodHit,WoodHitDecal);
		break;

		//Metal
	case EPhysicalSurface::SurfaceType2 :
		ImpactEffect(GunHit,MetalHitParticle,MetalHit,MetalHitDecal);
		break;

		//Stone
	case EPhysicalSurface::SurfaceType3:
		ImpactEffect(GunHit,StoneHitParticle,StoneHit,StoneHitDecal);
		break;

		//HeavyMetal
	case EPhysicalSurface::SurfaceType4 :
		ImpactEffect(GunHit,MetalHitParticle,MetalHit,MetalHitDecal);

		break;
	
	default:
		break;
	}
}

void AWeaponBase::ImpactEffect(FHitResult GunHit,UParticleSystem* HitParticle,USoundBase* HitSound, UMaterialInterface* HitDecal)
{
	if(HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this,HitParticle,GunHit.Location,GunHit.Normal.Rotation());
	}
	if(HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,HitSound,GunHit.Location);
	}
	if(HitDecal)
	{
	UGameplayStatics::SpawnDecalAttached(HitDecal,
										 FVector(10,10,10),
										 GunHit.Component.Get(),
										 GunHit.BoneName,
										 GunHit.ImpactPoint,
										 GunHit.ImpactNormal.Rotation(),
										 EAttachLocation::KeepWorldPosition,
										 7.f);
	}
}

FVector AWeaponBase::Recoil()
{
	FVector PlayerVelocity = PlayerCharacter->GetVelocity();
	float MovementRecoilAlpha = 1.f;
	EMovementType MovementType = PlayerCharacter->GetCurrentMovementType();

	if (MovementType != EMovementType::EMT_WallRun && PlayerCharacter->IsInAir())
	{
		MovementRecoilAlpha = 1.5f;
	}
	else if (PlayerVelocity.Size() > 2.f)
	{
		switch (MovementType)
		{
		case EMovementType::EMT_Walking:
			MovementRecoilAlpha = 1.1f;
			break;

		case EMovementType::EMT_Sprinting:
			MovementRecoilAlpha = 1.2f;
			break;

		case EMovementType::EMT_Crouching:
			MovementRecoilAlpha = 1.1f;
			break;

		case EMovementType::EMT_WallRun:
			MovementRecoilAlpha = 1.4f;
			break;
		}
	}

	FVector RecoilVector;
	float FinalAccuracy = PlayerCharacter->IsADSButtonDown() ? Accuracy_ADS : Accuracy;
	RecoilVector.X = SingleShotAlpha * MovementRecoilAlpha * UKismetMathLibrary::RandomFloatInRange(-0.2, 0.2) * FinalAccuracy;
	RecoilVector.Y = SingleShotAlpha * MovementRecoilAlpha * UKismetMathLibrary::RandomFloatInRange(-0.2, 0.2) * FinalAccuracy;
	RecoilVector.Z = SingleShotAlpha * MovementRecoilAlpha * UKismetMathLibrary::RandomFloatInRange(-0.2, 0.2) * FinalAccuracy;
	// UE_LOG(LogTemp,Warning,TEXT("%f"),RecoilValue);
	return RecoilVector;
	// PlayerCharacter->AddControllerYawInput(RecoilValue);
	// PlayerCharacter->AddControllerPitchInput(RecoilValue);
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
				if (ShootingTimerHandle.IsValid())
				{
					GetWorld()->GetTimerManager().ClearTimer(ShootingTimerHandle);
				}
				PlayerCharacter->bIsReloading = true;
				GunMesh->PlayAnimation(GunReloadAnim, false);
				ReloadTime = PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(PlayerReloadMontage) - 0.1f;
				UE_LOG(LogTemp, Warning, TEXT("%f"), UKismetSystemLibrary::GetGameTimeInSeconds(this));
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

	//UE_LOG(LogTemp, Warning, TEXT("%f"), UKismetSystemLibrary::GetGameTimeInSeconds(this));

	bIsReloading = false;
	PlayerCharacter->bIsReloading = false;
	bCanShoot = true;
	if (PlayerCharacter->bIsShootButtonDown)
	{
		Shoot();
	}
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
	UMaterialInstanceDynamic* GunMaterialInstance = GunMesh->CreateDynamicMaterialInstance(0);
	if(GunMaterialInstance)
	{
		GunMaterialInstance->SetVectorParameterValue(FName("EmissiveColor"), NewColor);
	}

	// UMaterialInstanceDynamic* SightMaterialInstance = SightMesh->CreateDynamicMaterialInstance(0);
	// if(SightMaterialInstance)
	// {
	// 	SightMaterialInstance->SetVectorParameterValue(FName("EmissiveColor"), NewColor);
	// }

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
									   GunDamage);

	PlayerCharacter->HitMarker();
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

	if(!PlayerCharacter)
	{
		return;
	}
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
	if (PlayerCharacter->PrimaryWeapon.WeaponClass == nullptr)
	{
		PlayerCharacter->WeaponEquippedSlot = 0;
		PlayerCharacter->PrimaryWeapon = WeaponData;
	}

	else if (PlayerCharacter->SecondaryWeapon.WeaponClass == nullptr)
	{
		PlayerCharacter->WeaponEquippedSlot = 1;
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

void AWeaponBase::BackToHolster()
{
	GunMesh->PlayAnimation(nullptr,false);
	SetActorHiddenInGame(true);
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	StopShooting();
	
}