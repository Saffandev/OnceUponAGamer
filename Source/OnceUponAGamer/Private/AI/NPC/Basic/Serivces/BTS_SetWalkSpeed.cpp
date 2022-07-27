// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Serivces/BTS_SetWalkSpeed.h"
#include "AI/NPC/Basic/BasicNPCAI.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "GameframeWork/CharacterMovementComponent.h"
#include "GameframeWork/Character.h"

UBTS_SetWalkSpeed::UBTS_SetWalkSpeed()
{
    NodeName = TEXT("SetWalkSpeed");
}

void UBTS_SetWalkSpeed::TickNode(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory,float DeltaSeconds)
{
    OwnerCharacter = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if(OwnerCharacter)
    {
        if(OwnerCharacter->GetCharacterMovement())
        {
            OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
            UE_LOG(LogTemp,Warning,TEXT("Set movement speed"));
        }
    }
}


// ABasicNPCAIController* OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());
    // OwnerComp.GetAIOwner()->GetCharacter();

    // ABasicNPCAI* OwnerCharacter = Cast<ABasicNPCAI>(OwnerController->GetPawn());
    // OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
    