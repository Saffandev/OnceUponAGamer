// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Heavy/Tasks/BTT_UpdatePatrolPointIndexHeavy.h"
#include "AI/NPC/Heavy/HeavyAIController.h"
#include "AI/NPC/Heavy/HeavyAI.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_UpdatePatrolPointIndexHeavy::UBTT_UpdatePatrolPointIndexHeavy()
{
    NodeName = TEXT("UpdatePatrolPointIndexHeavy");
}

EBTNodeResult::Type UBTT_UpdatePatrolPointIndexHeavy::ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory)
{
    AHeavyAIController* OwnerController = Cast<AHeavyAIController>(OwnerComp.GetOwner());
    uint32 IndexLength = OwnerController->GetControlledPawn()->PatrolPoint.Num();
    uint32 Direction = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsInt(PatrolDirection.SelectedKeyName);
    uint32 Index = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsInt(PatrolIndex.SelectedKeyName);
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
    OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsInt(PatrolDirection.SelectedKeyName,Direction);
    OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsInt(PatrolIndex.SelectedKeyName,Index);

    return EBTNodeResult::Succeeded;
}