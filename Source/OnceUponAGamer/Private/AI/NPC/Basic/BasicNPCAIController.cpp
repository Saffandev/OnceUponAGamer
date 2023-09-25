// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "AI/EncounterSpace.h"
#include "AI/NPC/Basic/BasicNPCAI.h"
#include "AI/Cover.h"
#include "AI/PatrolPoint.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "HelperMethods/AngleBetweenActors.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Player/PlayerCharacter.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"


void ABasicNPCAIController::BeginPlay()
{
    Super::BeginPlay();
}

void ABasicNPCAIController::CoverRequest()
{
    if(MyEncounterSpace && OwnerAI)
    {
        TArray<AActor*> CoverActor = MyEncounterSpace->OverlappedCovers;
        float CoverDistance = 1000000;
        ACover* MainCover = nullptr;
        FVector CoverPoints;
        FNavLocation NavLocation;
        for(auto TempCoverActor : CoverActor)
        {
            if(!TempCoverActor)
            {
                continue;
            }
            ACover* TempCover = Cast<ACover>(TempCoverActor);
            if(TempCover == nullptr)
            {
                // UE_LOG(LogTemp,Warning,TEXT("No Temp Cover"));
                continue;
            }
            else if(TempCover->bIsAcquired)
            {   
                continue;
            }
            
            else if(OwnerAI->GetDistanceTo(TempCover) < CoverDistance)
            {
                CoverPoints = TempCover->GiveCoverPoints();
                if(!UNavigationSystemV1::GetCurrent(GetWorld())->ProjectPointToNavigation(CoverPoints,NavLocation))
                {
                    continue;
                }
                CoverDistance = OwnerAI->GetDistanceTo(TempCover);
                MainCover = TempCover;

            }
        }
        CoverDistance = 1000000;
        if(MainCover == nullptr)
        {
            Blackboard->SetValueAsVector(FName("CoverLocation"),FVector::ZeroVector);
            Blackboard->SetValueAsBool(FName("bIsStandingCover"),false);
            Blackboard->SetValueAsBool(FName("bIsCoverAvailable"),false);
            Blackboard->SetValueAsObject(FName("CoverObj"),nullptr);
            // OwnerAI->ActiveCover = nullptr;
        }

        else
        {
            MainCover->bIsAcquired = true;
            Blackboard->SetValueAsVector(FName("CoverLocation"),NavLocation.Location);
            Blackboard->SetValueAsVector(FName("PeakCoverLocation"),MainCover->GivePeakPoints(OwnerAI));
            Blackboard->SetValueAsBool(FName("bIsStandingCover"),MainCover->bIsPeekCover);
            Blackboard->SetValueAsBool(FName("bIsCoverAvailable"),true);
            Blackboard->SetValueAsObject(FName("CoverObj"),MainCover);
            OwnerAI->ActiveCover = MainCover;
            if(!CoverTimerHandle.IsValid())
            {
                GetWorld()->GetTimerManager().SetTimer(CoverTimerHandle,this,&ABasicNPCAIController::CoverRequestEntry,15.f,true);
            }
        }

    }
    else
    {
        if(!OwnerAI)
        {
            // UE_LOG(LogTemp,Warning,TEXT("No Owner"));
        }
        // UE_LOG(LogTemp,Error,TEXT("No Encounter space"));

    }
    return;
}

void ABasicNPCAIController::CoverRequestEntry()
{
    Blackboard->SetValueAsBool(FName("bCanDoAnotherCoverRequest"),true);
}



