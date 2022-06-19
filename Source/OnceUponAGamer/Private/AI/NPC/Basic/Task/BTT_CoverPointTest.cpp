// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Task/BTT_CoverPointTest.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"


UBTT_CoverPointTest::UBTT_CoverPointTest()
{
    NodeName = TEXT("Cover Point Test");
}

EBTNodeResult::Type UBTT_CoverPointTest::ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory)
{
    ABasicNPCAIController* OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());
    UE_LOG(LogTemp,Display,TEXT("Cover Request Called"));
    if(OwnerController != nullptr)
    {
        OwnerController->CoverRequest();
        return EBTNodeResult::Succeeded;
    }
    else
    {
        return EBTNodeResult::Failed;
    }
    return EBTNodeResult::Failed;
}