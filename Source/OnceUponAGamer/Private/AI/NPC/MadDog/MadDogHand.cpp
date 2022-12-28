// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/MadDog/MadDogHand.h"
#include "AI/NPC/MadDog/MadDogNPCAI.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

// Sets default values
AMadDogHand::AMadDogHand()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	HandMesh = CreateDefaultSubobject<UStaticMeshComponent>("HandMesh");
	RootComponent = HandMesh;
	Point1 = CreateDefaultSubobject<USceneComponent>("Point1");
	Point1->SetupAttachment(RootComponent);
	Point2 = CreateDefaultSubobject<USceneComponent>("Point2");
	Point2->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void AMadDogHand::BeginPlay()
{
	Super::BeginPlay();
//	SetActorTickEnabled(false);
	OwnerAI = Cast<AMadDogNPCAI>(GetOwner());
	HandMesh->AddImpulse(ThrowForce,FName(NAME_None),true);
	HandMesh->OnComponentBeginOverlap.AddDynamic(this,&AMadDogHand::BeginOverlap);
	HandMesh->SetSimulatePhysics(true);

}

// Called every frame
void AMadDogHand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//if(OwnerAI)
	//{
	//	// UE_LOG(LogTemp,Warning,TEXT("Inside the tick of the hand"));
	//	FTransform WeaponSocketTransform = OwnerAI->GetMesh()->GetSocketTransform(FName("MeleeWeapon"));
	//	SetActorTransform(UKismetMathLibrary::TInterpTo(GetActorTransform(),WeaponSocketTransform,DeltaTime,10));
	//	if(WeaponSocketTransform.GetLocation().Equals(GetActorLocation(),20))
	//	{
	//		SetActorTransform(WeaponSocketTransform);
	//		OwnerAI->HandRecallDone();
	//		Destroy();
	//	}
	//}

	//else
	//{
	//	// UE_LOG(LogTemp,Warning,TEXT("no owner"));
	//}
}

void AMadDogHand::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	
	if(OtherActor == UGameplayStatics::GetPlayerPawn(this,0))
	{
		// UE_LOG(LogTemp,Warning,TEXT("Player Hitted"));
		if(GetOwner() && !bDamageGiven)
		{
			UGameplayStatics::ApplyPointDamage(OtherActor,
												Damage,
												SweepResult.TraceStart,
												SweepResult,
												nullptr,
												GetOwner(),
												UDamageType::StaticClass()
												);
			bDamageGiven = true;
		}
	}
}

void AMadDogHand::HandRecallInAction()
{
	// UE_LOG(LogTemp,Warning,TEXT("Recall hand inside the hand"));
	HandMesh->SetSimulatePhysics(false);
	HandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HandMesh->SetLinearDamping(1.f);
	if(!GetWorld()->GetTimerManager().TimerExists(HandRecallTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(HandRecallTimerHandle,[&](){SetActorTickEnabled(true);},0.1f,false,1.f);
		

	}
}