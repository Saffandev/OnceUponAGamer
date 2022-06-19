// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Task/BTT_FindPatrolPoint.h"
#include "AI/NPC/Basic/BasicNPCAI.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FindPatrolPoint::UBTT_FindPatrolPoint()
{
    NodeName = TEXT("BT_FindPatrolPoint");
}

EBTNodeResult::Type UBTT_FindPatrolPoint::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8* NodeMemory) 
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

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

    return EBTNodeResult::Succeeded;
}
