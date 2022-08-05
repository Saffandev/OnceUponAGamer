// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EncounterSpace.h"
#include "AI/Cover.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "AI/NPC/Basic/BasicNPCAI.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"

// Sets default values
AEncounterSpace::AEncounterSpace()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;
}

// Called when the game starts or when spawned

void AEncounterSpace::BeginPlay()
{
	Super::BeginPlay();
	if(CoverBp)
	{
		Box->GetOverlappingActors(OverlappedCovers,CoverBp);
	}
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,this,&AEncounterSpace::SetupController,0.1,false,0.1);
	
}

void AEncounterSpace::SetupController()
{
if(AIBp)
	{
		Box->GetOverlappingActors(OverlappedAI,AIBp);
		for(auto tempActor:OverlappedAI)
		{
			ABasicNPCAIController* TempController = Cast<ABasicNPCAIController>(tempActor->GetInstigatorController());
			if(TempController)
			{
				TempController->MyEncounterSpace = this;
			}
		}
	}
}

void AEncounterSpace::AddAI(ABasicNPCAI* AI)
{
	if(!AI)
	{
		return;
	}
	ABasicNPCAIController* TempController = Cast<ABasicNPCAIController>(AI->GetInstigatorController());
	if(TempController)
	{	
		OverlappedAI.Add(AI);
		TempController->MyEncounterSpace = this;
		UE_LOG(LogTemp,Warning,TEXT("AI added"));
	}
}

// Called every frame
void AEncounterSpace::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AEncounterSpace::IsPlayerVisibleToAnyone()
{
	for(auto TempActor: OverlappedAI)
	{
		ABasicNPCAIController* TempController = Cast<ABasicNPCAIController>(TempActor->GetInstigatorController());
		if(TempController == nullptr)
		{
			return false;
		}
		if(TempController->GetBlackboardComponent()->GetValueAsBool(FName("bIsPlayerVisible")))
		{
			return true;
		}
	}
	return false;
}

void AEncounterSpace::AssingInvestigation(FVector SuspectLocation)
{
	float tempDistance = 5000;
	FVector CoverPointLocation;
	AActor* ClosestActor = nullptr;

	for(auto TempActor : OverlappedAI)
	{
		ABasicNPCAIController* TempController = Cast<ABasicNPCAIController>(TempActor->GetInstigatorController());
		if(!TempController)
		{
			continue;
		}
		if(!TempController->GetBlackboardComponent())
		{
			continue;
		}
		if(TempController->GetBlackboardComponent()->GetValueAsBool(FName("bCanInvestigate")))
		{
			return;
		}
	}
	for(auto TempActor : OverlappedAI)
	{
		float Distance = UKismetMathLibrary::Vector_Distance(TempActor->GetActorLocation(), SuspectLocation);
	
		if(Distance < tempDistance)
		{
			tempDistance = Distance;
			ClosestActor = TempActor;
		}
		ABasicNPCAIController* TempController = Cast<ABasicNPCAIController>(TempActor->GetInstigatorController());
		if(TempController && TempController->GetBlackboardComponent())
		{
			TempController->GetBlackboardComponent()->SetValueAsBool("bCanInvestigate",false);
		}
	}	
	if(ClosestActor != nullptr)
	{	
		ABasicNPCAIController* TempController = Cast<ABasicNPCAIController>(ClosestActor->GetInstigatorController());
		if(TempController && TempController->GetBlackboardComponent())
		{
			TempController->GetBlackboardComponent()->SetValueAsBool("bCanInvestigate",true);
			TempController->GetBlackboardComponent()->SetValueAsBool("bIsSomeoneDoingInvestigation",true);
		}
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("Null closest actor"));
	}
	tempDistance = 5000;
}

void AEncounterSpace::MoveBackToPatrol()
{
	for(auto TempActor : OverlappedAI)
	{
		ABasicNPCAIController* TempController = Cast<ABasicNPCAIController>(TempActor->GetInstigatorController());
		if(TempController)
		{
			TempController->GetBlackboardComponent()->SetValueAsBool(FName("bCanSeePlayer"),false);
        	TempController->GetBlackboardComponent()->SetValueAsBool(FName("bCanInvestigate"),false);
			UE_LOG(LogTemp,Error,TEXT("Move Back To Patrolling Called"));
			TempController->ToggleSightSense();
		}
	}
}