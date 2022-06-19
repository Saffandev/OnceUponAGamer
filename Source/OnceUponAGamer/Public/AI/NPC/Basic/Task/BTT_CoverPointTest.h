// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "BTT_CoverPointTest.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTT_CoverPointTest : public UBTTask_BlueprintBase
{
	GENERATED_BODY()

public:
	UBTT_CoverPointTest();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory) override;
};
