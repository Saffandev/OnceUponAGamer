// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_RecallHand.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTT_RecallHand : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_RecallHand();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory) override;

private:
	UPROPERTY(EditAnywhere)
	UAnimMontage* HandMontage;
	class AMadDogNPCAI* OwnerPawn;
};
