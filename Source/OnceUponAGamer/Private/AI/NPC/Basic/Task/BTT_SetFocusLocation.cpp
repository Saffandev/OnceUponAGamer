// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Task/BTT_SetFocusLocation.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTT_SetFocusLocation::UBTT_SetFocusLocation()
{
    NodeName = TEXT("SetFocusLocation");
}

EBTNodeResult::Type UBTT_SetFocusLocation::ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory)
{
    if(OwnerComp.GetAIOwner())
    {
        if(bCanFocus && (OwnerComp.GetAIOwner()->GetBlackboardComponent()))
        {
            OwnerComp.GetAIOwner()->SetFocalPoint(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsVector(BB_FocusLocation.SelectedKeyName));
        }
        else
        {
            OwnerComp.GetAIOwner()->ClearFocus(EAIFocusPriority::Gameplay);
        }
    }
    // ABasicNPCAIController* OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());
    // if(bCanFocus)
    // {
    //     OwnerController->SetFocalPoint(OwnerController->GetBlackboardComponent()->GetValueAsVector(BB_FocusLocation.SelectedKeyName));
    //     // DrawDebugSphere(GetWorld(),OwnerController->GetBlackboardComponent()->GetValueAsVector(BB_FocusLocation.SelectedKeyName),20,20,FColor::Blue,false,5);
    // }
    // else
    // {
    //     OwnerController->ClearFocus(EAIFocusPriority::Default);
    // }
    
    return EBTNodeResult::Succeeded;
}