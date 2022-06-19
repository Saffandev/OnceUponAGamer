// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_HandCallBackCondition.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTS_HandCallBackCondition : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTS_HandCallBackCondition();

private:
	virtual void TickNode(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory, float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_bIsHoldingHand;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_bCanCallBackHand;
	FTimerHandle CallBackTimerHandle;

	
};
