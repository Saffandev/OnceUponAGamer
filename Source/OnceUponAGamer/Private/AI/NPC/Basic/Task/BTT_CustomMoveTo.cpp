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
#


UBTT_CustomMoveTo::UBTT_CustomMoveTo()
{
    NodeName = TEXT("CustomMoveTo");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTT_CustomMoveTo::ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp,NodeMemory);
    ABasicNPCAIController* OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());

    MoveLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BB_MoveLocation.SelectedKeyName);
    FNavLocation NavLocation;
    if(!UNavigationSystemV1::GetCurrent(GetWorld())->ProjectPointToNavigation(MoveLocation,NavLocation))
    {
        UE_LOG(LogTemp,Warning,TEXT("MOveToFailed"));
        return EBTNodeResult::Succeeded;
    }
    
    if(bCanSimpleMoveTo)
    {
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(OwnerComp.GetAIOwner(),NavLocation.Location);
        return EBTNodeResult::Succeeded;
    }
    else
    {
        
        MoveToTask = UAITask_MoveTo::AIMoveTo(OwnerComp.GetAIOwner(),
                                             NavLocation.Location,
                                              nullptr,
                                              AcceptanceRadius,
                                              EAIOptionFlag::Default,
                                              EAIOptionFlag::Default,
                                              true,
                                              true,
                                              false,
                                              EAIOptionFlag::Enable);
        MoveToTask->ConditionalPerformMove();

    }
    if(bCanDrawDebugSphere)
        DrawDebugSphere(GetWorld(),OwnerController->GetBlackboardComponent()->GetValueAsVector(BB_MoveLocation.SelectedKeyName),30,20,FColor::Green,false,20);

    return EBTNodeResult::InProgress;
}

void UBTT_CustomMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp,NodeMemory,DeltaSeconds);
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this,0);
    if(MoveToTask !=nullptr)
    {
        FNavLocation NavLocation;
        if(!UNavigationSystemV1::GetCurrent(GetWorld())->ProjectPointToNavigation(MoveLocation,NavLocation))
        {
            UE_LOG(LogTemp,Error,TEXT("Move to location is not reachable"));
            FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
            return;
        }
        else if(MoveToTask->WasMoveSuccessful())
        {
            UE_LOG(LogTemp,Warning,TEXT("Move to successful"));
            FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
            return;
        }
        else if(!MoveToTask->WasMoveSuccessful())
        {   
            // UE_LOG(LogTemp,Warning,TEXT("Move to not successful"));
        }
    }

    else
    {
        UE_LOG(LogTemp,Display,TEXT("No MoveToTask task"));
    }

    if(bCanMoveToPlayerCheck)
    {
        ABasicNPCAIController* OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());
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
