// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Task/BTT_IsPlayerVisibleToAnyAI.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "AI/EncounterSpace.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_IsPlayerVisibleToAnyAI::UBTT_IsPlayerVisibleToAnyAI()
{
    NodeName = TEXT("IsPlayerVisibleToAnyAI");
}

EBTNodeResult::Type UBTT_IsPlayerVisibleToAnyAI::ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory)
{
    ABasicNPCAIController* OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());
    if(OwnerController)
    {
        if(OwnerController->MyEncounterSpace == nullptr)
        {
            return EBTNodeResult::Succeeded;
        }
        // UE_LOG(LogTemp,Warning,TEXT("IsPlayerVisibleToAnyAI"));
        bool IsPlayerVisible = OwnerController->MyEncounterSpace->IsPlayerVisibleToAnyone();
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(BB_IsPlayerVisibleToAnyAI.SelectedKeyName,IsPlayerVisible);
        // OwnerController->GetBlackboardComponent()->SetValueAsBool(BB_IsPlayerVisibleToAnyAI.SelectedKeyName,IsPlayerVisible);
    }
    return EBTNodeResult::Succeeded;

}
