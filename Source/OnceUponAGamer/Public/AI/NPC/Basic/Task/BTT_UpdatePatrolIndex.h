// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_UpdatePatrolIndex.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTT_UpdatePatrolIndex : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_UpdatePatrolIndex();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PatrolDirection;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PatrolIndex;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PatrolPointObject;
};
