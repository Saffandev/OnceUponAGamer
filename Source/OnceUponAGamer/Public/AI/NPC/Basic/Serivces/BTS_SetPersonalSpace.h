// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_SetPersonalSpace.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTS_SetPersonalSpace : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTS_SetPersonalSpace();
	virtual void TickNode(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory,float DeltaSeconds) override;

private:
	float AngleBetween(AActor* From,AActor* To);

public:
	UPROPERTY(EditAnywhere)
	float PersonalAngle;
	UPROPERTY(EditAnywhere)
	float PersonalRange;
	UPROPERTY(EditAnywhere)
	float ExitPersonalRange;
	UPROPERTY(EditAnywhere)
	float crouchDistanceCheck;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_bIsinCover;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_bIsPlayerToClose;
	

};
