// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Heavy/Tasks/BTT_ReleaseAbility.h"
#include "AI/NPC/Heavy/HeavyAI.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_ReleaseAbility::UBTT_ReleaseAbility()
{
    NodeName = TEXT("BTT_ReleaseAbility");
}

EBTNodeResult::Type UBTT_ReleaseAbility::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp,NodeMemory);
    AAIController* OwnerController = OwnerComp.GetAIOwner();
    AHeavyAI* OwnerPawn = Cast<AHeavyAI>(OwnerController->GetPawn());
    if(OwnerPawn)
    {
        OwnerPawn->ReleaseAbility();
        OwnerController->GetBlackboardComponent()->SetValueAsBool(FName("bCanReleaseAbility"),false);
    }
    return EBTNodeResult::Succeeded;
}