// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_FindPatrolPointHeavy.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTT_FindPatrolPointHeavy : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_FindPatrolPointHeavy();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8* NodeMemory) override;


public:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PatrolPointIndex;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PatrolPointLocation;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PatrolPointObject;
};