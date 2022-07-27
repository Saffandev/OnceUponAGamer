// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Heavy/HeavyAI.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/NPC/Heavy/AIShotGun.h"
#include "Kismet/KismetMathLibrary.h"

void AHeavyAI::BeginPlay()
{
    Super::BeginPlay();
   
}

void AHeavyAI::StartShooting()
{
    Super::StartShooting();
	bIsShooting = true;
    UE_LOG(LogTemp,Warning,TEXT("Heavy shooting function"));
    if(!GetWorld()->GetTimerManager().IsTimerActive(AbilityReleaseTimer))
    {
        if(AIController)
        {
            AbilityTimerValue += UKismetMathLibrary::RandomFloatInRange(-2,2);
            GetWorld()->GetTimerManager().SetTimer(AbilityReleaseTimer,
                                                   [&]()
                                                   {
                                                    AIController->GetBlackboardComponent()->SetValueAsBool(FName("bCanReleaseAbility"),true);
                                                   },
                                                   AbilityTimerValue,
                                                   true);
            
        }
    }
}

void AHeavyAI::ReleaseAbility()
{
    Gun->ReleaseAbility();
}