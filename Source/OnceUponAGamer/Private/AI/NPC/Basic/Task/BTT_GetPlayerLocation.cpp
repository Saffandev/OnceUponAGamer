// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Task/BTT_GetPlayerLocation.h"
#include "Kismet/GameplayStatics.h"
//#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_GetPlayerLocation::UBTT_GetPlayerLocation()
{
    NodeName = TEXT("GetPlayerLocation");
}

EBTNodeResult::Type UBTT_GetPlayerLocation::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8* NodeMemory)
{
    FVector PlayerLocation = UGameplayStatics::GetPlayerPawn(this,0)->GetActorLocation();
    OwnerComp.GetBlackboardComponent()->SetValueAsVector(BB_PlayerLocation.SelectedKeyName,PlayerLocation);
    return EBTNodeResult::Succeeded;
}