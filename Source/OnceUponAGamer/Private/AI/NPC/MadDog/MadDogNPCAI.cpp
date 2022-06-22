// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/MadDog/MadDogNPCAI.h"
#include "AI/NPC/MadDog/MadDogController.h"
#include "AI/NPC/MadDog/MadDogHand.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameframeWork/CharacterMovementComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AIController.h"
#include "BrainComponent.h"

// Sets default values
AMadDogNPCAI::AMadDogNPCAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	HandMesh = CreateDefaultSubobject<UStaticMeshComponent>("HandMesh");
	HandMesh->SetupAttachment(GetMesh(),FName("MeleeWeapon"));

	ThrowArrow = CreateDefaultSubobject<UArrowComponent>("ThrowArrow");
	// ThrowArrow->SetupAttachment(GetMesh(),FName("Weapon"));

}

// Called when the game starts or when spawned
void AMadDogNPCAI::BeginPlay()
{
	Super::BeginPlay();
	AIController = Cast<AMadDogController>(GetInstigatorController());
	this->OnTakePointDamage.AddDynamic(this,&AMadDogNPCAI::TakePointDamage);
	this->OnTakeRadialDamage.AddDynamic(this,&AMadDogNPCAI::TakeRadialDamage);

	ActivateForBattle();
	if(HandMesh)
	{
		HandMesh->OnComponentBeginOverlap.AddDynamic(this,&AMadDogNPCAI::BeginOverlap);
	}	

	if(ThrowArrow)
	{
		ThrowArrow->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,FName("MeleeWeapon"));
	}
	
}

// Called every frame
void AMadDogNPCAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector ArrowNewLoc = FVector(ThrowArrow->GetComponentLocation().X,ThrowArrow->GetComponentLocation().Y,ThrowArrow->GetComponentLocation().Z - 100);
	const FVector FinalArrowLoc = ArrowNewLoc + (ThrowArrow->GetForwardVector() * PredectionDistance); 
	FRotator ArrowNewRot = UKismetMathLibrary::FindLookAtRotation(FinalArrowLoc,UGameplayStatics::GetPlayerCharacter(this,0)->GetMesh()->GetComponentLocation());
	ThrowArrow->SetWorldRotation(ArrowNewRot);

}

void AMadDogNPCAI::ActivateForBattle()
{
	SetActorTickEnabled(true);

	//tell the controller about the active state
	if(AIController)
	{
		AIController->Activate();
	}

	//convert to attack mode
	//start the shield
	//start the hand damage working and hand spark
}


void AMadDogNPCAI::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if(OtherActor == UGameplayStatics::GetPlayerPawn(this,0))
	{
		if(bCanHandDamage && OtherActor->CanBeDamaged())
		{
			// UE_LOG(LogTemp,Warning,TEXT("Player Hitted With the Hand"));
			bCanHandDamage = false;
			UGameplayStatics::ApplyDamage(OtherActor,0.1,AIController,this,UDamageType::StaticClass());
		}
	}
}


void AMadDogNPCAI::ThrowHand()
{
	// calculate the impulse
	//spawn the hand with impulse
	if(HandToThrowBP)
	{
		// HandToThrow = GetWorld()->SpawnActor<AMadDogHand>(HandToThrowBP,GetMesh()->GetSocketLocation(FName("MeleeWeapon")),GetMesh()->GetSocketRotation(FName("MeleeWeapon")));
		FTransform	HandTransform;
		HandTransform.SetLocation(GetMesh()->GetSocketLocation(FName("MeleeWeapon")));
		HandTransform.SetRotation(FRotator(GetMesh()->GetSocketRotation(FName("MeleeWeapon"))).Quaternion());
		HandTransform.SetScale3D(FVector(1,1,1));
		//this spawn is used to set the dinamic variables(setting variables before spawning the actor)
		HandToThrow = GetWorld()->SpawnActorDeferred<AMadDogHand>(HandToThrowBP,HandTransform,this);
		HandToThrow->ThrowForce = ThrowForceCalc();
		HandToThrow->FinishSpawning(HandTransform);
		HandMesh->SetVisibility(false);
	}
}

FVector AMadDogNPCAI::ThrowForceCalc()
{
	FVector ArrowNewLoc = FVector(ThrowArrow->GetComponentLocation().X,ThrowArrow->GetComponentLocation().Y,ThrowArrow->GetComponentLocation().Z - 100);
	const FVector FinalArrowLoc = ArrowNewLoc + (ThrowArrow->GetForwardVector() * PredectionDistance); 

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this,0);
	
	
	
	FVector PlayerFinalLoc = PlayerCharacter->GetMesh()->GetComponentLocation() + 
							 (PlayerCharacter->GetVelocity() * 
							 (this->GetDistanceTo(PlayerCharacter)/ ThrowForce));

	FVector FinalThrowForce = UKismetMathLibrary::FindLookAtRotation(FinalArrowLoc,PlayerFinalLoc).Vector() * ThrowForce;

	return FinalThrowForce;
}

void AMadDogNPCAI::RecallHand()
{
	// UE_LOG(LogTemp,Warning,TEXT("Recall hand inside the pawn"));
	if(HandToThrow)
	{	
		bIsRecallingHand = true;
		HandToThrow->HandRecallInAction();
	}
}

void AMadDogNPCAI::HandRecallDone()
{
	bIsRecallingHand = false;
	AIController->GetBlackboardComponent()->SetValueAsBool("bIsHoldingHand",true);
	HandMesh->SetVisibility(true);

}

void AMadDogNPCAI::TakePointDamage(AActor* DamagedActor,float Damage,AController* InstigatedBy, FVector HitLocation,UPrimitiveComponent* HitComp,FName BoneName,FVector ShotDirection,const UDamageType* DamageType,AActor* DamageCauser)
{
	Health -= Damage;
	UE_LOG(LogTemp,Error,TEXT("Health = %f"),Health);
	//Shield Damage visuals
	// body hit visuals
	//
	if(Health <= 0 && !bIsDead)
	{
		if(GetVelocity().Size() < 10.f )
		{
			if(DeathAnim_1)
			{
				GetMesh()->PlayAnimation(DeathAnim_1,false);
			}
		}
		DeathRituals(false);
	}
}
void AMadDogNPCAI::TakeRadialDamage(AActor* DamagedActor,float Damage,const UDamageType* DamageType,FVector Origin,FHitResult Hit,AController* InstigatedBy,AActor* DamageCauser)
{
	Health -= Damage;
	if(Health <= 0 && !bIsDead)
	{
		GetMesh()->SetSimulatePhysics(true);
		DeathRituals(true);
	}
}
void AMadDogNPCAI::DeathRituals(bool bIsExplosionDeath)
{
	Health = 0;
	bIsDead = true;
	SetCanBeDamaged(false);
	int timer = GetVelocity().Size() > 10 || bIsExplosionDeath? 0:1;
	FTimerHandle DeathTimer;
	GetWorld()->GetTimerManager().SetTimer(DeathTimer,[&](){GetMesh()->SetSimulatePhysics(true);},0.01,false,timer);
	GetCharacterMovement()->StopMovementImmediately();
	UAIBlueprintHelperLibrary::GetAIController(this)->GetBrainComponent()->StopLogic(FString("Dead"));
	DetachFromControllerPendingDestroy();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SetActorTickEnabled(false);
	HandMesh->DetachFromParent();
}