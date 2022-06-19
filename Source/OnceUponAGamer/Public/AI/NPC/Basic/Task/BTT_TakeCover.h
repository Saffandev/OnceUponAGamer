// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_TakeCover.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTT_TakeCover : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_TakeCover();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere)
	bool bCanTakeCover;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_IsInCover;
	UPROPERTY(EditAnywhere)
	bool bCanInvalidateCover;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_ActiveCoverObj;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_bIsCoverAvailable;
	UPROPERTY(EditAnywhere)
	bool bIsThisReRequestInvalidation;// make this varaible true when you are invalidating the cover to check again for the new cover
	
};
