// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Task/BTT_FindPatrolPoint.h"
#include "AI/NPC/Basic/BasicNPCAI.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "AI/PatrolPoint.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FindPatrolPoint::UBTT_FindPatrolPoint()
{
    NodeName = TEXT("BT_FindPatrolPoint");
}

EBTNodeResult::Type UBTT_FindPatrolPoint::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8* NodeMemory) 
{
    Super::ExecuteTask(OwnerComp, NodeMemory);
    // UE_LOG(LogTemp,Warning,TEXT("%s"),*OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(PatrolPointObject.SelectedKeyName)->GetName());
    // if(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(PatrolPointObject.SelectedKeyName) == nullptr)
    // {
    //     UE_LOG(LogTemp,Error,TEXT("NO Patrol key"));
    // }
    
    // APatrolPoint* PatrolPointObj = Cast<APatrolPoint>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(PatrolPointObject.SelectedKeyName));
    OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());
    OwnerCharacter = Cast<ABasicNPCAI>(OwnerController->GetPawn());
    if(OwnerCharacter)
    {
        uint32 Index = OwnerController->GetBlackboardComponent()->GetValueAsInt(PatrolPointIndex.SelectedKeyName);
        if(OwnerCharacter->PatrolPoint.Num() > 0)
        {
        FVector MoveToLocation = OwnerCharacter->PatrolPoint[Index];
        OwnerController->GetBlackboardComponent()->SetValueAsVector(PatrolPointLocation.SelectedKeyName, MoveToLocation);
        }
    }
//  if(PatrolPointObj)
//     {
//         uint32 Index = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsInt(PatrolPointIndex.SelectedKeyName);
//         if(PatrolPointObj->PatrolPointLocation.Num() > 0)
//         {
//         FVector MoveToLocation = PatrolPointObj->PatrolPointLocation[Index];
//         OwnerController->GetBlackboardComponent()->SetValueAsVector(PatrolPointLocation.SelectedKeyName, MoveToLocation);
//         }
//     }
//     if(PatrolPointObj == nullptr)
//     {
//         UE_LOG(LogTemp,Warning,TEXT("No patrol point object in find patrol point"));

//     }
    return EBTNodeResult::Succeeded;
}
