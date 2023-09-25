// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/BaseAI.h"
#include "AI/PatrolPoint.h"
#include "AI/NPC/Basic/AIGun.h"
#include "AI/Cover.h"
#include "AI/EncounterSpace.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameframeWork/CharacterMovementComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Weapons/WeaponBase.h"
#include "BrainComponent.h"
#include "HelperFunctions/ViolenceRegistration.h"

// Sets default values
ABaseAI::ABaseAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TouchSenseCapsule = CreateDefaultSubobject<UCapsuleComponent>("TouchSenseCapsule");
	TouchSenseCapsule->SetupAttachment(GetCapsuleComponent());
}

// Called when the game starts or when spawned
void ABaseAI::BeginPlay()
{
	Super::BeginPlay();
	TouchSenseCapsule->OnComponentBeginOverlap.AddDynamic(this, &ABaseAI::OnOverlapTouchSense);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABaseAI::OnOverlap);
	AIController = UAIBlueprintHelperLibrary::GetAIController(this);
	OnTakePointDamage.AddDynamic(this, &ABaseAI::TakePointDamage);
	this->OnTakeRadialDamage.AddDynamic(this, &ABaseAI::TakeRadialDamage);

	if (PatrolPointObj != nullptr)
	{
		FTransform PatrolobjTransform = PatrolPointObj->GetActorTransform();
		for (FVector TempPatrolPointLoc : PatrolPointObj->PatrolPointLocation)
		{
			PatrolPoint.Add(UKismetMathLibrary::TransformLocation(PatrolobjTransform, TempPatrolPointLoc));
		}
	}

	if (GunBp)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		Gun = GetWorld()->SpawnActor<AAIGun>(GunBp, SpawnParameters);
		Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GunSocketName);
		Gun->OwnerMesh = GetMesh();
	}
	CurrentHealth = Health;
	
}
void ABaseAI::OnOverlapTouchSense(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)
	{
		return;
	}
	if (OtherActor == UGameplayStatics::GetPlayerPawn(this, 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Touched"));
		UE_LOG(LogTemp, Warning, TEXT("%s"), *AIController->GetName());
	//	AIController->SetFocalPoint(OtherActor->GetActorLocation());
		AIController->SetFocus(OtherActor);
	}
}

void ABaseAI::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (MyEncounterSpace == nullptr)
	{
		MyEncounterSpace = Cast<AEncounterSpace>(OtherActor);
		if (MyEncounterSpace)
		{
			// UE_LOG(LogTemp,Warning,TEXT("Encounterspace overlappaed"));
			MyEncounterSpace->AddAI(this);

		}
	}
}

void ABaseAI::TakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* HitComp, FName BoneName, FVector ShotDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	CurrentHealth -= Damage;
	if (MyEncounterSpace && GetEnemyType() != EEnemyType::EET_Healer)
	{
		if (GetRemainingHealth() < 80.f)
		{
			MyEncounterSpace->AddDamagedAI(this);
		}
		else
		{
			MyEncounterSpace->RemoveDamagedAI(this);
		}
	}
	HealthVisuals();
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitLocation);
	}
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, HitLocation);
	}
	if (GetRemainingHealth() >= 200)
	{
		if (ExplosionActor)
		{
			AActor* Ex = GetWorld()->SpawnActor<AActor>(ExplosionActor, GetActorLocation(), GetActorRotation());
			Ex->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}

		FTimerHandle ExplisionTimerHandle;
		SetCanBeDamaged(false);
		GetWorld()->GetTimerManager().SetTimer(ExplisionTimerHandle, this, &ABaseAI::Explode, 0.8f, false);
	}
	if (CurrentHealth <= 0 && bIsDead == false)
	{
		//Healshot effects
		LastHitBoneName = BoneName;
		if (LastHitBoneName == "neck_01")
		{
			UMaterialInterface* MeshMat = GetMesh()->GetMaterial(0);
			GetMesh()->SetSkeletalMesh(HeadlessMesh, true);
			GetMesh()->SetMaterial(0, MeshMat);

		// spawning of the health/shield on headshot 
			if (UKismetMathLibrary::RandomBool())
			{
				if (HealthPickup)
				{
					AActor* SpawnedHealth = GetWorld()->SpawnActor<AActor>(HealthPickup, GetActorLocation(), GetActorRotation());
				}
			}
			else
			{
				if (ShieldPickup)
				{
					AActor* SpawnedHealth = GetWorld()->SpawnActor<AActor>(ShieldPickup, GetActorLocation(), GetActorRotation());
				}
			}
			if (Head)
			{
				AActor* SpawnedHead = GetWorld()->SpawnActor<AActor>(Head, GetMesh()->GetSocketLocation(FName("neck_01")), GetMesh()->GetSocketRotation(FName("neck_01")));
			}
		}

		GetMesh()->SetSimulatePhysics(true);

		DeathRituals(false);
	}
}

void ABaseAI::TakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, FHitResult Hit, AController* InstigatedBy, AActor* DamageCauser)
{
	CurrentHealth -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("Taking Damage"));
	if (GetRemainingHealth() < 80.f && MyEncounterSpace)
	{
		MyEncounterSpace->AddDamagedAI(this);
	}
	HealthVisuals();
	if (CurrentHealth <= 0 && bIsDead == false)
	{
		LastHitBoneName = Hit.BoneName;
		ExplosionEffect();
		DeathRituals(true);
	}
}


void ABaseAI::DeathRituals(bool bIsExplosionDeath)
{
	CurrentHealth = 0;
	UViolenceRegistration::RegisterViolence(this, GetActorLocation(), this, EViolenceType::EVT_PoliceKilled);
	if (Gun)
	{
		Gun->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		Gun->Destroy();
	}
	//spawn drop gun
	if (GunDrop)
	{
		GetWorld()->SpawnActor<AWeaponBase>(GunDrop, GetMesh()->GetSocketLocation(FName("Weapon")), GetMesh()->GetSocketRotation(FName()));
	}
	// UE_LOG(LogTemp,Error,TEXT("I am deaddddddd"));
	bIsDead = true;
	SetCanBeDamaged(false);
	StopShooting();
	FTimerHandle DeathTimer;
	if (MyEncounterSpace)
	{
		MyEncounterSpace->IAMDead(this);
		MyEncounterSpace->RemoveDamagedAI(this);
	}
	// else
	// 	UE_LOG(LogTemp,Warning,TEXT("No Encounter Space"));
	if (bIsExplosionDeath)
	{

		GetMesh()->SetSimulatePhysics(true);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(DeathTimer, [&]() {GetMesh()->SetSimulatePhysics(true); }, 0.01, false, 1.f);
	}
	GetCharacterMovement()->StopMovementImmediately();
	if (AIController->GetBrainComponent())
	{
		AIController->GetBrainComponent()->StopLogic(FString("Dead"));
	}
	AIController->UnPossess();
	
	DetachFromControllerPendingDestroy();
	AIController->Destroy();
	TouchSenseCapsule->DestroyComponent();
	SetCanBeDamaged(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	if (HealthPickup)
	{
		if (UKismetMathLibrary::RandomBool())
		{
			AActor* SpawnedHealth = GetWorld()->SpawnActor<AActor>(HealthPickup, GetActorLocation(), GetActorRotation());
		}

	}
	if (ShieldPickup)
	{
		if (UKismetMathLibrary::RandomBool())
		{
			AActor* SpawnedHealth = GetWorld()->SpawnActor<AActor>(ShieldPickup, GetActorLocation(), GetActorRotation());
		}
	}

}

void ABaseAI::HealthVisuals()
{
	float HelathPercent = CurrentHealth / Health;
	UE_LOG(LogTemp, Warning, TEXT("%f"), HelathPercent);
	UMaterialInstanceDynamic* HealthVisMat = GetMesh()->CreateDynamicMaterialInstance(2);
	UMaterialInstanceDynamic* HealthVisMat2 = GetMesh()->CreateDynamicMaterialInstance(1);

	if (HealthVisMat)
	{
		UE_LOG(LogTemp, Warning, TEXT("Yes"));
		HealthVisMat->SetScalarParameterValue(FName("Health"), HelathPercent);
	}
	if (HealthVisMat2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Yes"));
		HealthVisMat2->SetScalarParameterValue(FName("Health"), HelathPercent);
	}
}

void ABaseAI::Explode()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> OverlapActorObjectType;
	OverlapActorObjectType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	OverlapActorObjectType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	OverlapActorObjectType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));

	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(this);
	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, ExplosionDamageType, ActorsToIgnore);
	ExplosionEffect();
}

void ABaseAI::ExplosionEffect()
{
	GetMesh()->BreakConstraint(FVector(-100, 100, 100), FVector(0, 0, 0), FName("upperarm_l"));
	GetMesh()->BreakConstraint(FVector(100, -100, 100), FVector(0, 0, 0), FName("upperarm_r"));
	bool RandomBool = UKismetMathLibrary::RandomBool();
	FName B1 = RandomBool ? FName("thigh_r") : FName("calf_r");
	FName B2 = RandomBool ? FName("thigh_l") : FName("calf_l");

	GetMesh()->BreakConstraint(FVector(-100, -100, 100), FVector(0, 0, 0), B2);
	GetMesh()->BreakConstraint(FVector(100, 100, 100), FVector(0, 0, 0), B1);
	DeathRituals(true);
}

// Called every frame
void ABaseAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseAI::StartShooting()
{
	if (Gun && !bIsDead)
	{
		bIsShooting = true;
		Gun->StartShooting();
	}
}

void ABaseAI::StopShooting()
{
	if (Gun)
	{
		bIsShooting = false;
		Gun->StopShooting();
	}
}

float ABaseAI::MeleeAttack()
{
	float AttackTime = 0.f;
	// if(!bIsMeleeAttack)
	// {
	bIsMeleeAttack = true;
	if (MeleeAttackMontage)
	{
		AttackTime = GetMesh()->GetAnimInstance()->Montage_Play(MeleeAttackMontage);
	}
	// GetWorld()->GetTimerManager().SetTimer(MeleeTimerHandle,[&](){bIsMeleeAttack = false;},0.1,false,AttackTime);
// }
	return AttackTime;
}

float ABaseAI::GetRemainingHealth()
{
	float RemainingHealth = CurrentHealth / Health;
	return RemainingHealth * 100.f;
}

void ABaseAI::SetReload(bool IsReloading)
{
	StopShooting();
	this->bIsReloading = IsReloading;
}

bool ABaseAI::GetIsReloading()
{
	if (Gun)
	{
		return Gun->GetIsReloading();
	}
	return false;
}

USkeletalMeshComponent* ABaseAI::GetWeaponMesh()
{
	if (Gun)
	{
		return Gun->GunMesh;
	}
	return nullptr;
}

