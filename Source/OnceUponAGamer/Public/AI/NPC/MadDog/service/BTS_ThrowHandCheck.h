// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_ThrowHandCheck.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTS_ThrowHandCheck : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTS_ThrowHandCheck();
	virtual void TickNode(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere)
	float ThrowAngleCheck;
	UPROPERTY(EditAnywhere)
	float AirThrowCheck;
	UPROPERTY(EditAnywhere)
	float GroundThrowCheck;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_bIsHoldingHand;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_bCanThrowHand;
	bool bCanThrowAgain = true;
	FTimerHandle ThrowTimerHandle;
};
