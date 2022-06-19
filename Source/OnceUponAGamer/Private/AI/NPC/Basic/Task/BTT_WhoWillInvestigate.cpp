// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/NPC/Basic/Task/BTT_WhoWillInvestigate.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "AI/EncounterSpace.h"
#include "BehaviorTree/BlackboardComponent.h"
	
UBTT_WhoWillInvestigate::UBTT_WhoWillInvestigate()
{
    NodeName = TEXT("WhoWillInvestigate");
}

EBTNodeResult::Type UBTT_WhoWillInvestigate::ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory)
{
    ABasicNPCAIController* OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());
    if(OwnerController->MyEncounterSpace)
    {
        if(OwnerController->GetBlackboardComponent())
        {
            OwnerController->MyEncounterSpace->AssingInvestigation(OwnerController->GetBlackboardComponent()->GetValueAsVector(BB_SuspectLocation.SelectedKeyName));
        }
    }
    return EBTNodeResult::Succeeded;
}