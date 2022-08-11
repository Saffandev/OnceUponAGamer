// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/BasicNPCAI.h"
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
// Sets default values
ABasicNPCAI::ABasicNPCAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TouchSenseCapsule = CreateDefaultSubobject<UCapsuleComponent>("TouchSenseCapsule");
	TouchSenseCapsule->SetupAttachment(GetCapsuleComponent());
}

// Called when the game starts or when spawned
void ABasicNPCAI::BeginPlay()
{
	Super::BeginPlay();
	TouchSenseCapsule->OnComponentBeginOverlap.AddDynamic(this,&ABasicNPCAI::OnOverlapTouchSense);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this,&ABasicNPCAI::OnOverlap);
	AIController = UAIBlueprintHelperLibrary::GetAIController(this);
	OnTakePointDamage.AddDynamic(this,&ABasicNPCAI::TakePointDamage);
	this->OnTakeRadialDamage.AddDynamic(this,&ABasicNPCAI::TakeRadialDamage);

	if(PatrolPointObj != nullptr)
	{
		FTransform PatrolobjTransform = PatrolPointObj->GetActorTransform();
		for(FVector TempPatrolPointLoc : PatrolPointObj->PatrolPointLocation)
		{
			PatrolPoint.Add(UKismetMathLibrary::TransformLocation(PatrolobjTransform,TempPatrolPointLoc));
		}
	}

	if(GunBp)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		Gun = GetWorld()->SpawnActor<AAIGun>(GunBp,SpawnParameters);
		Gun->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,FName("Weapon"));
		Gun->OwnerMesh = GetMesh();
	}
}

// Called every frame
void ABasicNPCAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABasicNPCAI::OnOverlapTouchSense(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, bool bFromSweep ,const FHitResult &SweepResult)
{
	if(OtherActor == nullptr)
	{
		return ;
	}
	if(OtherActor == UGameplayStatics::GetPlayerPawn(this,0))
	{
		// SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),OtherActor->GetActorLocation()));
		AIController->SetFocalPoint(OtherActor->GetActorLocation());
	}
}
void ABasicNPCAI::OnOverlap(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, bool bFromSweep ,const FHitResult &SweepResult)
{
	MyEncounterSpace = Cast<AEncounterSpace>(OtherActor);
	if(MyEncounterSpace)
	{
		UE_LOG(LogTemp,Warning,TEXT("Encounterspace overlappaed"));
		MyEncounterSpace->AddAI(this);
		
	}
}
void ABasicNPCAI::StartShooting()
{
	if(Gun && !bIsDead)
	{
		bIsShooting = true;
		Gun->StartShooting();
	}
}

void ABasicNPCAI::StopShooting()
{
	if(Gun)
	{
		bIsShooting = false;
		Gun->StopShooting();
	}
}

void ABasicNPCAI::SetReload(bool IsReloading)
{
	StopShooting();
	this->bIsReloading = IsReloading;
}

bool ABasicNPCAI::GetIsReloading()
{
	if(Gun)
	{
		return Gun->GetIsReloading();
	}
	return false;
}

float ABasicNPCAI::MeleeAttack()
{
	float AttackTime = 0.f;
	// if(!bIsMeleeAttack)
	// {
		bIsMeleeAttack = true;
		if(MeleeAttackMontage)
		{
			AttackTime = GetMesh()->GetAnimInstance()->Montage_Play(MeleeAttackMontage);
		}
		// GetWorld()->GetTimerManager().SetTimer(MeleeTimerHandle,[&](){bIsMeleeAttack = false;},0.1,false,AttackTime);
	// }
	return AttackTime;
}

void ABasicNPCAI::CanTakeCover(bool bCanTakeCover)
{
	if(bCanTakeCover)
	{
		GetCharacterMovement()->Crouch();
	}
	else
	{
		GetCharacterMovement()->UnCrouch();
	}
	bCanCrouch = bCanTakeCover;
}

void ABasicNPCAI::TakePointDamage(AActor* DamagedActor,float Damage,AController* InstigatedBy, FVector HitLocation,UPrimitiveComponent* HitComp,FName BoneName,FVector ShotDirection,const UDamageType* DamageType,AActor* DamageCauser)
{
	UE_LOG(LogTemp,Warning,TEXT("Damage Taken %f"),Damage);
	Health -= Damage;
	if(Health <= 0 && bIsDead == false)
	{
		//death;
		LastHitBoneName = BoneName;	
		if(LastHitBoneName == "neck_01")
		{	
		UMaterialInterface* MeshMat = GetMesh()->GetMaterial(0);
		GetMesh()->SetSkeletalMesh(HeadlessMesh,true);
		GetMesh()->SetMaterial(0,MeshMat);
			if(Head)
			{
				AActor* SpawnedHead = GetWorld()->SpawnActor<AActor>(Head,GetMesh()->GetSocketLocation(FName("neck_01")),GetMesh()->GetSocketRotation(FName("neck_01")));
			}
		}
		if(GetVelocity().Size() < 10.f)
		{
			switch(UKismetMathLibrary::RandomIntegerInRange(0,2))
			{
				case 0:
				if(DeathAnim_1)
					UE_LOG(LogTemp,Warning,TEXT("Inside death anim"));
					GetMesh()->PlayAnimation(DeathAnim_1,false);
				break;
				
				case 1:
				if(DeathAnim_2)
					UE_LOG(LogTemp,Warning,TEXT("Inside death anim"));
					GetMesh()->PlayAnimation(DeathAnim_2,false);
				break;

				case 2:
				if(DeathAnim_3)
					UE_LOG(LogTemp,Warning,TEXT("Inside death anim"));
					GetMesh()->PlayAnimation(DeathAnim_3,false);
				break;

				default:
					UE_LOG(LogTemp,Warning,TEXT("Default case"));
			}
		}
		else
		{
			GetMesh()->SetSimulatePhysics(true);
		}
		DeathRituals(false);
	}
}

void ABasicNPCAI::TakeRadialDamage(AActor* DamagedActor,float Damage,const UDamageType* DamageType,FVector Origin,FHitResult Hit,AController* InstigatedBy,AActor* DamageCauser)
{
	Health -= Damage;
	if(Health <= 0 && bIsDead == false)
	{
		LastHitBoneName = Hit.BoneName;	
		DeathRituals(true);
	}
}

void ABasicNPCAI::DeathRituals(bool bIsExplosionDeath)
{
	Health = 0;
	UE_LOG(LogTemp,Error,TEXT("I am deaddddddd"));
	bIsDead = true;
	StopShooting();
	FTimerHandle DeathTimer;
	if(MyEncounterSpace)
		MyEncounterSpace->IAMDead();
	else
		UE_LOG(LogTemp,Warning,TEXT("No Encounter Space"));
	if(bIsExplosionDeath)
	{
		GetMesh()->SetSimulatePhysics(true);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(DeathTimer,[&](){GetMesh()->SetSimulatePhysics(true);},0.01,false,1.f);
	}
	GetCharacterMovement()->StopMovementImmediately();
	AIController->GetBrainComponent()->StopLogic(FString("Dead"));
	AIController->UnPossess();
	
	if(Gun)
	{
		Gun->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		Gun->Destroy();
	}
	//spawn drop gun
	if(GunDrop)
	{
		GetWorld()->SpawnActor<AWeaponBase>(GunDrop,GetMesh()->GetSocketLocation(FName("Weapon")),GetMesh()->GetSocketRotation(FName()));
	}
	if(ActiveCover)
	{
		ActiveCover->bIsAcquired = false;
	}
	TouchSenseCapsule->DestroyComponent();
	DetachFromControllerPendingDestroy();
	SetCanBeDamaged(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	
	// GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

}

USkeletalMeshComponent* ABasicNPCAI::GunMesh()
{
	if(Gun)
	{
		return Gun->GunMesh;
	}
	return nullptr;
	
}