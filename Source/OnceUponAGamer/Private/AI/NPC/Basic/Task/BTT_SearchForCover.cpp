// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Task/BTT_SearchForCover.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_SearchForCover::UBTT_SearchForCover()
{
    NodeName = TEXT("SearchForCover");
}

EBTNodeResult::Type UBTT_SearchForCover::ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory)
{
    ABasicNPCAIController* OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());
    if(OwnerController != nullptr)
    {
        if(OwnerController->GetBlackboardComponent()->GetValueAsBool(BB_bIsCoverAvailable.SelectedKeyName) && !bIsReRequest)
        {
            return EBTNodeResult::Succeeded;
        }
        if(bIsReRequest)
        {
            OwnerController->GetBlackboardComponent()->SetValueAsBool(BB_bCanDoAnotherCoverRequest.SelectedKeyName,false);

        }
        OwnerController->CoverRequest();
        return EBTNodeResult::Succeeded;
    }
    return EBTNodeResult::Failed;

}