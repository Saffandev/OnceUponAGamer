// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/MadDog/service/BTS_HandCallBackCondition.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTS_HandCallBackCondition::UBTS_HandCallBackCondition()
{
    NodeName = "HandCallBackCondition";
}

void UBTS_HandCallBackCondition::TickNode(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory, float DeltaTime)
{
    float Distance = OwnerComp.GetAIOwner()->GetPawn()->GetDistanceTo(UGameplayStatics::GetPlayerPawn(OwnerComp.GetAIOwner(),0));
    bool bIsHoldingHand = OwnerComp.GetBlackboardComponent()->GetValueAsBool(BB_bIsHoldingHand.SelectedKeyName);
    bool bCanCallHand = OwnerComp.GetBlackboardComponent()->GetValueAsBool(BB_bCanCallBackHand.SelectedKeyName);
    if(!bIsHoldingHand)
    {
        if(Distance > 300 && !bCanCallHand)
        {
        // OwnerComp.GetBlackboardComponent()->SetValueAsBool(BB_bCanCallBackHand.SelectedKeyName,true);
            if(!GetWorld()->GetTimerManager().TimerExists(CallBackTimerHandle))
            {
                GetWorld()->GetTimerManager().SetTimer(CallBackTimerHandle,[&](){OwnerComp.GetBlackboardComponent()->SetValueAsBool(BB_bCanCallBackHand.SelectedKeyName,true);},0.1f,false,2);
            }
        }
    }
    else
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(BB_bCanCallBackHand.SelectedKeyName,false);
    }


}