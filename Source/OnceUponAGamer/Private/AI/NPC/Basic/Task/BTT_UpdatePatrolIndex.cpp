// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Task/BTT_UpdatePatrolIndex.h"
#include "AI/NPC/Basic/BasicNPCAI.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "AI/PatrolPoint.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_UpdatePatrolIndex::UBTT_UpdatePatrolIndex()
{
    NodeName = TEXT("UpdatePatrolIndex");
}

EBTNodeResult::Type UBTT_UpdatePatrolIndex::ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory)
{
    ABasicNPCAIController* OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());
    APatrolPoint* PatrolPointObj = Cast<APatrolPoint>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(PatrolPointObject.SelectedKeyName));
    // if(PatrolPointObj == nullptr)
    // {
    //     return EBTNodeResult::Aborted;
    // }
    uint32 IndexLength = OwnerController->GetControlledPawn()->PatrolPoint.Num();
    uint32 Direction = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsInt(PatrolDirection.SelectedKeyName);
    uint32 Index = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsInt(PatrolIndex.SelectedKeyName);
    uint32 MinIndex = 0;
    if(Direction == 1)
    {
        Index = FMath::Clamp(Index + 1, MinIndex, IndexLength - 1);
        if(Index == (IndexLength - 1))
        {
            Direction = -1;
        }
    }
    else if(Direction == -1)
    {
        Index = FMath::Clamp(Index - 1, MinIndex, IndexLength - 1);
        if(Index == 0)
        {
            Direction = 1;
        }
    }
    OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsInt(PatrolDirection.SelectedKeyName,Direction);
    OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsInt(PatrolIndex.SelectedKeyName,Index);

    return EBTNodeResult::Succeeded;
}