// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_FindPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTT_FindPatrolPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_FindPatrolPoint();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8* NodeMemory) override;

private:
	class ABasicNPCAI* OwnerCharacter;
	class ABasicNPCAIController* OwnerController;
public:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PatrolPointIndex;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PatrolPointLocation;
};
