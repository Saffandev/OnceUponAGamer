// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Task/BTT_Shooting.h"
#include "Interface/NPCAI_Action.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "Kismet/GameplayStatics.h"

UBTT_Shooting::UBTT_Shooting()
{
    NodeName = TEXT("Start/Stop Shooting");
}

EBTNodeResult::Type UBTT_Shooting::ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp,NodeMemory);
    ABasicNPCAIController* OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());

    INPCAI_Action* ActionInterface = Cast<INPCAI_Action>(OwnerComp.GetAIOwner()->GetPawn());
    if(ActionInterface != nullptr)
    {
        if(bCanShoot)
        {
            ActionInterface->StartShooting();
            OwnerComp.GetAIOwner()->SetFocus(UGameplayStatics::GetPlayerPawn(this,0));
        }
        else if(bCanMeleeAttack)
        {
            // OwnerController->StopMovement();
            ActionInterface->StopShooting();
            AttackTime = ActionInterface->MeleeAttack();
            OwnerComp.GetAIOwner()->SetFocus(UGameplayStatics::GetPlayerPawn(this,0));
            bIsDoingMeleeAttack = true;
            bNotifyTick = true;
            // UE_LOG(LogTemp,Display,TEXT("Melee Attacking"));
            return EBTNodeResult::InProgress;
        }
        else
        {
            OwnerComp.GetAIOwner()->ClearFocus(EAIFocusPriority::Gameplay);
            ActionInterface->StopShooting();
            UE_LOG(LogTemp, Warning, TEXT("Stop shooting"));
        }
    }
    else 
    {
        // UE_LOG(LogTemp,Warning,TEXT("No Action Interface"));
    }
    return EBTNodeResult::Succeeded;

}

void UBTT_Shooting::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if(!bIsDoingMeleeAttack)
    {
        // UE_LOG(LogTemp,Error,TEXT("%f"),AttackTime);
        GetWorld()->GetTimerManager().SetTimer(MeleeTimerHandle,[&](){bIsDoingMeleeAttack = false;
                                                                      FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
                                                                      },
                                                                      0.1,
                                                                      false,
                                                                      AttackTime);
    }
    else
    {
        FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
    }
}