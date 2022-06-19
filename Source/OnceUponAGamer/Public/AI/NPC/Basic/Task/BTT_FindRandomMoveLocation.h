// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_FindRandomMoveLocation.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTT_FindRandomMoveLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTT_FindRandomMoveLocation();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_RandomMoveLocation;
	UPROPERTY(EditAnywhere)
	float SearchRadiusForPoints;
	UPROPERTY(EditAnywhere)
	float SearchOriginRangeBehind;
	UPROPERTY(EditAnywhere)
	float RandomDeviation;
	UPROPERTY(EditAnywhere)
	bool bShouldFindRandomMovePointsBehind;


};
