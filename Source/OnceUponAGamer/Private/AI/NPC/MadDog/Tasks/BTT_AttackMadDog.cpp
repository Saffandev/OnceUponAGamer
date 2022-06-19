// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/MadDog/Tasks/BTT_AttackMadDog.h"
#include "AI/NPC/MadDog/MadDogNPCAI.h"
#include "Kismet/KismetMathLibrary.h"
#include "BrainComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_AttackMadDog::UBTT_AttackMadDog()
{
    NodeName = TEXT("AttackMadDog");
}
EBTNodeResult::Type UBTT_AttackMadDog::ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8 *NodeMemory)
{
    OwnerPawn = Cast<AMadDogNPCAI>(OwnerComp.GetAIOwner()->GetPawn());
    OwnerComp.GetAIOwner()->StopMovement();
    OwnerComp.GetAIOwner()->SetFocus(UGameplayStatics::GetPlayerPawn(OwnerPawn,0)); 
    OwnerPawn->bCanThrowHand = bIsThisThrowHandTask;
    
    OwnerComp.GetBlackboardComponent()->SetValueAsBool(BB_bIsHoldingHand.SelectedKeyName,!bIsThisThrowHandTask);

    if(OwnerPawn)
    {
        uint32 rNumber = UKismetMathLibrary::RandomIntegerInRange(1,5);
        switch (rNumber)
        {
            case 1:
            MontagePlay(AttackMontage1);
            break;

            case 2:
            MontagePlay(AttackMontage2);
            break;

            case 3:
            MontagePlay(AttackMontage3);
            break;

            case 4:
            MontagePlay(AttackMontage4);
            break;

            case 5:
            MontagePlay(AttackMontage5);
            break;

        }
    }
    return EBTNodeResult::InProgress;
}

void UBTT_AttackMadDog::TickTask(UBehaviorTreeComponent &OwnerComp,uint8 *NodeMemory, float DeltaTime)
{
    if(!GetWorld()->GetTimerManager().TimerExists(EndHandle))
    {
        UBehaviorTreeComponent &Tree = OwnerComp;
        // FTimerDelegate AttackDelegate;
        // AttackDelegate.BindUFunction(this,FName("AttackEnded"),Tree);
        UE_LOG(LogTemp,Error,TEXT("Attack Time %f"),AttackTime);
        if(!GetWorld()->GetTimerManager().TimerExists(EndHandle))
        {
        GetWorld()->GetTimerManager().SetTimer(EndHandle,[&](){OwnerComp.GetBlackboardComponent()->SetValueAsBool(BB_bCanThrowHand.SelectedKeyName,!bIsThisThrowHandTask);
                                                            FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);}, 0.1f, false, AttackTime);
        }
        }
    
}


void UBTT_AttackMadDog::MontagePlay(UAnimMontage* Montage)
{
    if(Montage)
    {
        AttackTime = OwnerPawn->GetMesh()->GetAnimInstance()->Montage_Play(Montage);
        bNotifyTick = true;
        AttackFinished = false;
        // if(OwnerPawn)
        // {
        //     OwnerPawn->bCanHandDamage = true;
            
        // }
        return;
    }
    AttackTime = 0;
    return;
}

void UBTT_AttackMadDog::AttackEnded(UBehaviorTreeComponent &OwnerComp)
{
    if(OwnerPawn)
        {
            // OwnerPawn->bCanHandDamage = false;
            
        }
}