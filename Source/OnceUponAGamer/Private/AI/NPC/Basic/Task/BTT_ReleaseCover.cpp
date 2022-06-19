// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Task/BTT_ReleaseCover.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "AI/Cover.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTT_ReleaseCover::UBTT_ReleaseCover()
{
    NodeName = TEXT("ReleaseCover");
}

EBTNodeResult::Type UBTT_ReleaseCover::ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory)
{
    return EBTNodeResult::Succeeded;
}