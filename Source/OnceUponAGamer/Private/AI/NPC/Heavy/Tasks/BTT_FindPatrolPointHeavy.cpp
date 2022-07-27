// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Heavy/Tasks/BTT_FindPatrolPointHeavy.h"
#include "AI/NPC/Heavy/HeavyAI.h"
#include "AI/NPC/Heavy/HeavyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FindPatrolPointHeavy::UBTT_FindPatrolPointHeavy()
{
    NodeName = TEXT("FindPatrolPointHeavy");
}

EBTNodeResult::Type UBTT_FindPatrolPointHeavy::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8* NodeMemory) 
{
    Super::ExecuteTask(OwnerComp, NodeMemory);
    
    AHeavyAIController* OwnerController = Cast<AHeavyAIController>(OwnerComp.GetOwner());
    AHeavyAI* OwnerCharacter = Cast<AHeavyAI>(OwnerController->GetPawn());
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
