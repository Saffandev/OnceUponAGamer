// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_CoverRequest.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTS_CoverRequest : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTS_CoverRequest();
	virtual void TickNode(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory,float DeltaSeconds);
};
