// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/MadDog/Tasks/BTT_RecallHand.h"
#include "AI/NPC/MadDog/MadDogNPCAI.h"

UBTT_RecallHand::UBTT_RecallHand()
{
    NodeName = "RecallHand";   
}

EBTNodeResult::Type UBTT_RecallHand::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory)
{
    OwnerPawn = Cast<AMadDogNPCAI>(OwnerComp.GetAIOwner()->GetPawn());
    
    if(HandMontage)
    {
        OwnerPawn->GetMesh()->GetAnimInstance()->Montage_Play(HandMontage);
    }
    OwnerPawn->RecallHand();
    return EBTNodeResult::Succeeded;
}