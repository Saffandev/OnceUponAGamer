// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_CustomMoveTo.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTT_CustomMoveTo : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_CustomMoveTo();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


private:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_MoveLocation;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_bIsPlayerTooClose;
	UPROPERTY(EditAnywhere)
	bool bCanSimpleMoveTo;
	UPROPERTY(EditAnywhere)
	bool bCanMoveToPlayerCheck;
	UPROPERTY(EditAnywhere)
	float AcceptanceRadius;
	UPROPERTY(EditAnywhere)
	float PersonalRange;
	UPROPERTY(EditAnywhere)
	float PersonalAngle;
	UPROPERTY(EditAnywhere)
	bool bCanDrawDebugSphere;
	
	class UAITask_MoveTo* MoveToTask;
	FVector MoveLocation;
};
