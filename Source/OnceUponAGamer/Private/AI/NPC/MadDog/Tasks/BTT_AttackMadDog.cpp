// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/MadDog/Tasks/BTT_AttackMadDog.h"
#include "AI/NPC/MadDog/MadDogNPCAI.h"
#include "Kismet/KismetMathLibrary.h"
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_AttackMadDog::UBTT_AttackMadDog()
{
    NodeName = TEXT("AttackMadDog");
}
EBTNodeResult::Type UBTT_AttackMadDog::ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8 *NodeMemory)
{
    OwnerPawn = Cast<AMadDogNPCAI>(OwnerComp.GetAIOwner()->GetPawn());
    OwnerComp.GetAIOwner()->SetFocus(UGameplayStatics::GetPlayerPawn(OwnerPawn,0));
    OwnerPawn->GetCharacterMovement()->StopMovementImmediately();
    OwnerPawn->bCanThrowHand = bIsThisThrowHandTask;
    OwnerComp.GetBlackboardComponent()->SetValueAsBool(BB_bIsHoldingHand.SelectedKeyName, !bIsThisThrowHandTask);
   
    UE_LOG(LogTemp, Warning, TEXT("attacking"));
    if(OwnerPawn)
    {
        uint32 length = AttackMontage.Num();
        uint32 rNumber = UKismetMathLibrary::RandomIntegerInRange(0, length-1);
        MontagePlay(AttackMontage[rNumber]);
    }
    return EBTNodeResult::InProgress;
}

void UBTT_AttackMadDog::TickTask(UBehaviorTreeComponent &OwnerComp,uint8 *NodeMemory, float DeltaTime)
{
    if(!GetWorld()->GetTimerManager().TimerExists(EndHandle))
    {
        UBehaviorTreeComponent &Tree = OwnerComp;
        if(!GetWorld()->GetTimerManager().TimerExists(EndHandle))
        {
            GetWorld()->GetTimerManager().SetTimer(EndHandle,[&](){OwnerComp.GetBlackboardComponent()->SetValueAsBool(BB_bCanThrowHand.SelectedKeyName,!bIsThisThrowHandTask);
            OwnerComp.GetAIOwner()->ClearFocus(EAIFocusPriority::Gameplay);
            FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);}, 0.1f, false, AttackTime);
        }
        }
    
}


void UBTT_AttackMadDog::MontagePlay(UAnimMontage* Montage)
{
    if(Montage)
    {
        AttackTime = OwnerPawn->GetMesh()->GetAnimInstance()->Montage_Play(Montage) + 0.5f;
        bNotifyTick = true;
        AttackFinished = false;
       
        return;
    }
    AttackTime = 0;
    return;
}
