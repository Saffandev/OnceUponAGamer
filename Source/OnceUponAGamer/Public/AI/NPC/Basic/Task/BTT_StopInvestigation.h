// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_StopInvestigation.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTT_StopInvestigation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_StopInvestigation();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory) override;

};
