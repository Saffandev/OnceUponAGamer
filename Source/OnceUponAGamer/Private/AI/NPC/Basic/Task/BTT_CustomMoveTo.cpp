// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Task/BTT_CustomMoveTo.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Tasks/AITask_MoveTo.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "HelperMethods/AngleBetweenActors.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "AIController.h"


UBTT_CustomMoveTo::UBTT_CustomMoveTo()
{
    NodeName = TEXT("CustomMoveTo");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTT_CustomMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);
    OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());
    MoveLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BB_MoveLocation.SelectedKeyName);
    FNavLocation NavLocation;

    if (!UNavigationSystemV1::GetCurrent(GetWorld())->ProjectPointToNavigation(MoveLocation, NavLocation))
    {
        return EBTNodeResult::Aborted;
    }
  /*  if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(BB_bIsPlayerTooClose.SelectedKeyName))
    {
        return EBTNodeResult::Aborted;
    }*/
    if(bCanSimpleMoveTo)
    {
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(OwnerComp.GetAIOwner(),NavLocation.Location);
        return EBTNodeResult::Succeeded;
    }
    else
    {
        if(OwnerController)
        MoveToResult = OwnerController->MoveToLocation(MoveLocation,AcceptanceRadius);
    }

    return EBTNodeResult::InProgress;
}

void UBTT_CustomMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp,NodeMemory,DeltaSeconds);
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this,0);
    // UE_LOG(LogTemp,Warning,TEXT("Move REsult %i"),OwnerController->GetPathFollowingComponent()->HasReached(MoveLocation,AcceptanceRadius));
    // if(OwnerController->GetPathFollowingComponent()->HasReached(MoveLocation,AcceptanceRadius))
    // {
    //     float d = UKismetMathLibrary::Vector_Distance(OwnerController->GetControlledPawn()->GetActorLocation(),MoveLocation);
    //     UE_LOG(LogTemp,Warning,TEXT("Distance %f"),d);
    //     UE_LOG(LogTemp,Warning,TEXT("Reached"));
    // }
    if(OwnerController)
    {
        float d = UKismetMathLibrary::Vector_Distance(OwnerController->GetControlledPawn()->GetActorLocation(),MoveLocation);
        
        if(d < 100)
        {
            FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
        }
    }
    // bool bHasReached = UKismetMathLibrary::EqualEqual_VectorVector(OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation(),MoveLocation,10);
    // UE_LOG(LogTemp,Warning,TEXT("Reached %i"),bHasReached);
    // if(OwnerController)
    // {
    
    //     if(OwnerController->GetMoveStatus() == EPathFollowingRequestResult::RequestSuccessful)
    //     {
    //         FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
    //     }
    //     else
    //     {
    //         UE_LOG(LogTemp,Warning,TEXT("Moving")); 
    //     }
    // }
    // if(MoveToTask !=nullptr)
    // {
    //     FNavLocation NavLocation;
    //     if(!UNavigationSystemV1::GetCurrent(GetWorld())->ProjectPointToNavigation(MoveLocation,NavLocation))
    //     {
    //         UE_LOG(LogTemp,Error,TEXT("Move to location is not reachable"));
    //         FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
    //         return;
    //     }
    //     else if(MoveToTask->WasMoveSuccessful())
    //     {
    //         UE_LOG(LogTemp,Warning,TEXT("Move to successful"));
    //         FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
    //         return;
    //     }
    //     // else if(!MoveToTask->WasMoveSuccessful())
    //     // {   
    //     //     // UE_LOG(LogTemp,Warning,TEXT("Move to not successful"));
    //     // }
    // }

    // else
    // {
    //     // UE_LOG(LogTemp,Display,TEXT("No MoveToTask task"));
    // }

    if(bCanMoveToPlayerCheck)
    {
        float Angle = AngleBetweenActors::AngleBetween(OwnerComp.GetAIOwner()->GetPawn(),PlayerPawn);
        bool bIsAngleInRange = UKismetMathLibrary::InRange_FloatFloat(Angle,0,PersonalAngle);

        if(bIsAngleInRange)
        {
            float Distance = OwnerComp.GetAIOwner()->GetPawn()->GetDistanceTo(PlayerPawn);
            if(Distance < PersonalRange)
            {
                if(OwnerComp.GetAIOwner()->LineOfSightTo(PlayerPawn))
                {
                    OwnerComp.GetBlackboardComponent()->SetValueAsBool(BB_bIsPlayerTooClose.SelectedKeyName,true);
                    FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
                }
            }
        }
    }
}

