// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EncounterSpace.h"
#include "AI/Cover.h"
#include "AI/NPC/BaseAIController.h"
#include "AI/NPC/BaseAI.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Interface/NPCAI_Action.h"

// Sets default values
AEncounterSpace::AEncounterSpace()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
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
		UE_LOG(LogTemp,Error,TEXT("%i"),OverlappedCovers.Num());
	}
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,this,&AEncounterSpace::SetupController,0.1,false);
}

void AEncounterSpace::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AEncounterSpace::SetupController()
{
	if(AIBp)
	{
		Box->GetOverlappingActors(OverlappedAI,AIBp);
		NoOfAiAlive = OverlappedAI.Num();
		for(auto tempActor:OverlappedAI)
		{
			ABaseAIController* TempController = Cast<ABaseAIController>(tempActor->GetInstigatorController());
			if(TempController)
			{
				// UE_LOG(LogTemp,Warning,TEXT("Encounter Space added ---------------"));
				TempController->MyEncounterSpace = this;
				OverlappedAIControllers.Add(TempController);
			}
		}
	}
}

void AEncounterSpace::AddAI(ACharacter* AI)
{
	if(!AI)
	{
		return;
	}
	ABaseAIController* TempController = Cast<ABaseAIController>(AI->GetInstigatorController());
	NoOfAiAlive++;
	if(TempController)
	{	
		OverlappedAI.Add(AI);
		OverlappedAIControllers.Add(TempController);
		TempController->MyEncounterSpace = this;
		// UE_LOG(LogTemp,Warning,TEXT("AI added"));
	}
}

// Called every frame


bool AEncounterSpace::IsPlayerVisibleToAnyone()
{
	if(OverlappedAIControllers.Num() <= 0)
	{
		return false;
	}
	for(ABaseAIController* TempController : OverlappedAIControllers)
	{
		if(TempController && TempController->GetBlackboardComponent())
		{
			// UE_LOG(LogTemp,Warning,TEXT("Controller Name : %s"),*TempController->GetName());
			if(TempController->GetBlackboardComponent()->GetValueAsBool(FName("bIsPlayerVisible")))
			{
				return true;
			}
		}
	}
	return false;
}

void AEncounterSpace::AssingInvestigation(FVector SuspectLocation)
{
	float tempDistance = 50000;
	FVector CoverPointLocation;
	AActor* ClosestActor = nullptr;
	ABaseAIController* ClosestController = nullptr;



	for(ABaseAIController* TempController : OverlappedAIControllers)
	{
		if(TempController)
		{
			if(!TempController->GetBlackboardComponent())
			{
			continue;
			}
			if(TempController->GetBlackboardComponent()->GetValueAsBool(FName("bCanInvestigate")))
			{
				return;
			}
		}
	}
		
	for(ABaseAIController* TempController : OverlappedAIControllers)
	{
		if(TempController)
		{
			APawn* AIPawn = TempController->GetControlledPawn();
			if(AIPawn)
			{
				float Distance = UKismetMathLibrary::Vector_Distance(AIPawn->GetActorLocation(),SuspectLocation);
				if(Distance < tempDistance)
				{
					tempDistance = Distance;
					ClosestController = TempController;
				}
			}
		}

	}
	if(ClosestController != nullptr)
	{	
		if(ClosestController->GetBlackboardComponent())
		{
			ClosestController->GetBlackboardComponent()->SetValueAsBool("bCanInvestigate",true);
			ClosestController->GetBlackboardComponent()->SetValueAsBool("bIsSomeoneDoingInvestigation",true);
		}
	}
	tempDistance = 5000;
}

void AEncounterSpace::MoveBackToPatrol()
{

	for(ABaseAIController* TempController : OverlappedAIControllers)
	{
		TempController->ClearSenses();
	}

}

void AEncounterSpace::IAMDead( ACharacter* DeadAI)
{
	NoOfAiAlive--;
	ABaseAIController* DeadController = Cast<ABaseAIController>(DeadAI->GetInstigatorController());
	if(DeadController)
	{
		OverlappedAIControllers.Remove(DeadController);
	}
	if(NoOfAiAlive <= 0)
	{
		//open doors
		TheratCleared();

	}
}

void AEncounterSpace::ICanSeePlayer()
{
	bPlayerSpotted = true;
	for(ABaseAIController* TempController : OverlappedAIControllers)
	{
		if(TempController && TempController->GetBlackboardComponent())
		{
			TempController->GetBlackboardComponent()->SetValueAsBool(FName("bCanSeePlayer"),true);
		}
	}
}

void AEncounterSpace::ClearAllTheAI()
{
	UE_LOG(LogTemp, Warning, TEXT("Clear of encounterspace"));	

	if (OverlappedAIControllers.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Stored Controller"));
		return ;
	}
	for (ABaseAIController* TempController : OverlappedAIControllers)
	{
		if (TempController)
		{
			UE_LOG(LogTemp, Error, TEXT("Clear of encounterspace---inside loop"));

			TempController->ClearSenses();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Temp Controller"));
		}

	}
}

ABaseAI* AEncounterSpace::GetClosestDamagedAI(AActor* Requester)
{
	//we tried to use this with the interface, but for now we are taking a direct route
	/*for (auto DamagedAI : OverlappedAI)
	{
		INPCAI_Action* AI = Cast<INPCAI_Action>(DamagedAI);
		if (AI)
		{
			if (AI->GetRemainingHealth() < 80.f)
			{
				DamagedAIs.Add(DamagedAI);
			}
		}
	}*/
	float Distance = 99999999999999;
	FVector RequsterLoc = Requester->GetActorLocation();
	ABaseAI* ClosestAI = nullptr;
	for (auto DamagedAI : DamagedAIs)
	{
		float TempDistance = UKismetMathLibrary::Vector_DistanceSquared(DamagedAI->GetActorLocation(), RequsterLoc);
		if (TempDistance < Distance)
		{
			Distance = TempDistance;
			ClosestAI = DamagedAI;
		}
	}

	return ClosestAI;
}

void AEncounterSpace::AddDamagedAI(ABaseAI* DamagedAI)
{
	DamagedAIs.Add(DamagedAI);
}

void AEncounterSpace::RemoveDamagedAI(ABaseAI* RecoveredAI)
{
	DamagedAIs.Remove(RecoveredAI);
}
