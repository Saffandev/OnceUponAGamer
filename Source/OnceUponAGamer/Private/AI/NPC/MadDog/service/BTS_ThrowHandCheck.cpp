// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/MadDog/service/BTS_ThrowHandCheck.h"
#include "HelperMethods/AngleBetweenActors.h"
#include "Kismet/GameplayStatics.h"
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_ThrowHandCheck::UBTS_ThrowHandCheck()
{
    NodeName = "ThrowHandCheck";
}

void UBTS_ThrowHandCheck::TickNode(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory, float DeltaSeconds)
{
    

    if(OwnerComp.GetBlackboardComponent()->GetValueAsBool(BB_bIsHoldingHand.SelectedKeyName) && bCanThrowAgain)
    {
        APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();
        ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(OwnerPawn,0);
        
        float AngleBetween = AngleBetweenActors::AngleBetween(OwnerPawn,PlayerPawn);
        bool LineOfSight = OwnerComp.GetAIOwner()->LineOfSightTo(PlayerPawn);
        float Distance = OwnerPawn->GetDistanceTo(PlayerPawn);

        bool bCanThrowHand = (Distance > GroundThrowCheck || 
                            (Distance > AirThrowCheck && 
                            PlayerPawn->GetCharacterMovement()->IsFalling())) && 
                            LineOfSight && 
                            AngleBetween < ThrowAngleCheck;
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(BB_bCanThrowHand.SelectedKeyName,bCanThrowHand);
        bCanThrowAgain = false;
        UE_LOG(LogTemp,Warning,TEXT("Inside the throw hand check "));
        if(!GetWorld()->GetTimerManager().TimerExists(ThrowTimerHandle))
        {
            GetWorld()->GetTimerManager().SetTimer(ThrowTimerHandle,[&](){bCanThrowAgain = true;},0.1f,false,6.f);
        }
    }

}

