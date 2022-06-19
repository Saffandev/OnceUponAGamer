// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_SearchForCover.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTT_SearchForCover : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_SearchForCover();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere)
	bool bIsReRequest;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_bIsCoverAvailable;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_bCanDoAnotherCoverRequest;//used just to reset the value after invaladiting the cover

};
