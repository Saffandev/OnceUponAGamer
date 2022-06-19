// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Task/BTT_UpdatePatrolIndex.h"
#include "AI/NPC/Basic/BasicNPCAI.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_UpdatePatrolIndex::UBTT_UpdatePatrolIndex()
{
    NodeName = TEXT("UpdatePatrolIndex");
}

EBTNodeResult::Type UBTT_UpdatePatrolIndex::ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory)
{
    ABasicNPCAIController* OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());
    uint32 IndexLength = OwnerController->GetControlledPawn()->PatrolPoint.Num();
    uint32 Direction = OwnerController->GetBlackboardComponent()->GetValueAsInt(PatrolDirection.SelectedKeyName);
    uint32 Index = OwnerController->GetBlackboardComponent()->GetValueAsInt(PatrolIndex.SelectedKeyName);
    if(Direction == 1)
    {
        Index++;
        if(Index == (IndexLength - 1))
        {
            Direction = -1;
        }
    }
    else if(Direction == -1)
    {
        Index--;
        if(Index == 0)
        {
            Direction = 1;
        }
    }
    OwnerController->GetBlackboardComponent()->SetValueAsInt(PatrolDirection.SelectedKeyName,Direction);
    OwnerController->GetBlackboardComponent()->SetValueAsInt(PatrolIndex.SelectedKeyName,Index);

    return EBTNodeResult::Succeeded;
}