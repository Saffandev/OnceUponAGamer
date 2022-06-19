// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Serivces/BTS_SetAttackingRange.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HelperMethods/AngleBetweenActors.h"

UBTS_SetAttackingRange::UBTS_SetAttackingRange()
{
    NodeName = TEXT("SetAttackingRange");
}

void UBTS_SetAttackingRange::TickNode(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory,float DeltaTime)
{
    ABasicNPCAIController* OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());
    float Distance = OwnerComp.GetAIOwner()->GetPawn()->GetDistanceTo(UGameplayStatics::GetPlayerPawn(this,0));
    float Angle = AngleBetweenActors::AngleBetween(OwnerComp.GetAIOwner()->GetPawn(),UGameplayStatics::GetPlayerPawn(this,0));

    if(Angle < AttackingAngle)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(BB_bInShootingRange.SelectedKeyName,(Distance < MaxShootingRange && Distance >= MinShootingRange));
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(BB_bInMeleeRange.SelectedKeyName,(Distance < MinShootingRange) && bCanCheckForMelee);
        // if(OwnerController->GetBlackboardComponent()->GetValueAsBool(BB_bInMeleeRange.SelectedKeyName))
        // {
        //     bCanCheckForMelee = false;
        //     GetWorld()->GetTimerManager().SetTimer(MeleeTimerHandle,[&](){bCanCheckForMelee = true;},0.1,false,3);
        // }
        
    }
}


