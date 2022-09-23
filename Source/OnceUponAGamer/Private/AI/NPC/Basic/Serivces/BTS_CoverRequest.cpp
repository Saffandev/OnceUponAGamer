// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Serivces/BTS_CoverRequest.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"

UBTS_CoverRequest::UBTS_CoverRequest()
{
    NodeName = TEXT("CoverRequest");
}

void UBTS_CoverRequest::TickNode(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory,float DeltaSeconds)
{
    ABasicNPCAIController* OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());
    if(OwnerController != nullptr)
    {
        // UE_LOG(LogTemp,Warning,TEXT("Cover service"));
        OwnerController->CoverRequest();
    }

}