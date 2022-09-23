// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Task/BTT_StopInvestigation.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "AI/EncounterSpace.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_StopInvestigation::UBTT_StopInvestigation()
{
    NodeName = TEXT("StopInvestigation");
}

EBTNodeResult::Type UBTT_StopInvestigation::ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory)
{
    ABasicNPCAIController* OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());
    if(OwnerController)
    {
        // UE_LOG(LogTemp,Error,TEXT("Inside of Stop Investigating"));
        OwnerController->GetBlackboardComponent()->SetValueAsBool("bIsSomeoneDoingInvestigation",false);
    //    OwnerController->MyEncounterSpace->MoveBackToPatrol();
    }
    return EBTNodeResult::Succeeded;
}