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
	/*if (OverlappedAIControllers.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT(" Stored Controller %i"), OverlappedAIControllers.Num());
	}*/
}
void AEncounterSpace::SetupController()
{
if(AIBp)
	{
		Box->GetOverlappingActors(OverlappedAI,AIBp);
		NoOfAiAlive = OverlappedAI.Num();
		for(auto tempActor:OverlappedAI)
		{
			ABasicNPCAIController* TempController = Cast<ABasicNPCAIController>(tempActor->GetInstigatorController());
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
	ABasicNPCAIController* TempController = Cast<ABasicNPCAIController>(AI->GetInstigatorController());
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
	// for(auto TempActor: OverlappedAI)
	// {
	// 	if(TempActor)
	// 	{
	// 		ABasicNPCAIController* TempController = Cast<ABasicNPCAIController>(TempActor->GetInstigatorController());
	// 		if(TempController == nullptr)
	// 		{
	// 			return false;
	// 		}
	// 		if(TempController->GetBlackboardComponent() && TempController->GetBlackboardComponent()->GetValueAsBool(FName("bIsPlayerVisible")))
	// 		{
	// 			return true;
	// 		}
	// 	}
	// }
	// return false;
	if(OverlappedAIControllers.Num() <= 0)
	{
		return false;
	}
	for(ABasicNPCAIController* TempController : OverlappedAIControllers)
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
	float tempDistance = 5000;
	FVector CoverPointLocation;
	AActor* ClosestActor = nullptr;
	ABasicNPCAIController* ClosestController = nullptr;

	// for(auto TempActor : OverlappedAI)
	// {
	// 	ABasicNPCAIController* TempController = Cast<ABasicNPCAIController>(TempActor->GetInstigatorController());
	// 	if(!TempController)
	// 	{
	// 		continue;
	// 	}
	// 	if(!TempController->GetBlackboardComponent())
	// 	{
	// 		continue;
	// 	}
	// 	if(TempController->GetBlackboardComponent()->GetValueAsBool(FName("bCanInvestigate")))
	// 	{
	// 		return;
	// 	}
	// }

	for(ABasicNPCAIController* TempController : OverlappedAIControllers)
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
	// for(auto TempActor : OverlappedAI)
	// {
	// 	float Distance = UKismetMathLibrary::Vector_Distance(TempActor->GetActorLocation(), SuspectLocation);
	
	// 	if(Distance < tempDistance)
	// 	{
	// 		tempDistance = Distance;
	// 		ClosestActor = TempActor;
	// 	}
	// 	ABasicNPCAIController* TempController = Cast<ABasicNPCAIController>(TempActor->GetInstigatorController());
	// 	if(TempController && TempController->GetBlackboardComponent())
	// 	{
	// 		TempController->GetBlackboardComponent()->SetValueAsBool("bCanInvestigate",false);
	// 	}
	// }	
	for(ABasicNPCAIController* TempController : OverlappedAIControllers)
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
		// ABasicNPCAIController* TempController = Cast<ABasicNPCAIController>(ClosestActor->GetInstigatorController());
		if(ClosestController->GetBlackboardComponent())
		{
			ClosestController->GetBlackboardComponent()->SetValueAsBool("bCanInvestigate",true);
			ClosestController->GetBlackboardComponent()->SetValueAsBool("bIsSomeoneDoingInvestigation",true);
		}
	}
	// else
	// {
	// 	// UE_LOG(LogTemp,Warning,TEXT("Null closest actor"));
	// }
	tempDistance = 5000;
}

void AEncounterSpace::MoveBackToPatrol()
{
	// for(auto TempActor : OverlappedAI)
	// {
	// 	ABasicNPCAIController* TempController = Cast<ABasicNPCAIController>(TempActor->GetInstigatorController());
	// 	if(TempController)
	// 	{
	// 		TempController->GetBlackboardComponent()->SetValueAsBool(FName("bCanSeePlayer"),false);
    //     	TempController->GetBlackboardComponent()->SetValueAsBool(FName("bCanInvestigate"),false);
	// 		UE_LOG(LogTemp,Error,TEXT("Move Back To Patrolling Called"));
	// 		TempController->ToggleSightSense();
	// 	}
	// }
	for(ABasicNPCAIController* TempController : OverlappedAIControllers)
	{
		//TempController->GetBlackboardComponent()->SetValueAsBool(FName("bCanSeePlayer"),false);
		//TempController->GetBlackboardComponent()->SetValueAsBool(FName("bCanInvestigate"),false);
		//// UE_LOG(LogTemp,Error,TEXT("Move Back To Patrolling Called"));
		//TempController->ToggleSightSense();
		TempController->ClearSenses();
	}

}

void AEncounterSpace::IAMDead( ACharacter* DeadAI)
{
	NoOfAiAlive--;
	// SomeoneIsDead();
	// UE_LOG(LogTemp,Warning,TEXT("Dead controller %s"),*DeadAI->GetInstigatorController()->GetName());
	// UE_LOG(LogTemp,Warning,TEXT("Ai count = %i"),NoOfAiAlive);
	ABasicNPCAIController* DeadController = Cast<ABasicNPCAIController>(DeadAI->GetInstigatorController());
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
	for(ABasicNPCAIController* TempController : OverlappedAIControllers)
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
	for (ABasicNPCAIController* TempController : OverlappedAIControllers)
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
