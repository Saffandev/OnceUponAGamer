// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Task/BTT_FindPatrolPoints.h"
#include "AI/NPC/Basic/BasicNPCAI.h"

UBTT_FindPatrolPoints::UBTT_FindPatrolPoints()
{
    NodeName = TEXT("PatrolPoint");

}

void UBTT_FindPatrolPoints::ReceiveExecuteAI(AAIController* OwnerController, APawn* ControlledPawn)
{
    UE_LOG(LogTemp,Warning,TEXT("Running"));
}