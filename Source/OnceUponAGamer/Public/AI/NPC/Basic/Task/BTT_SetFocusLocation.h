// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_SetFocusLocation.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTT_SetFocusLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_SetFocusLocation();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory) override;


private:
	UPROPERTY(EditAnywhere)
	bool bCanFocus;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_FocusLocation;
};
