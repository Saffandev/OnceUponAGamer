// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "BTT_FindPatrolPoints.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTT_FindPatrolPoints : public UBTTask_BlueprintBase
{
	GENERATED_BODY()

public:
UBTT_FindPatrolPoints();
void ReceiveExecuteAI(AAIController* OwnerController, APawn* ControlledPawn);

public:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PatrolPointIndex;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PatrolPointLocation;
	
};
