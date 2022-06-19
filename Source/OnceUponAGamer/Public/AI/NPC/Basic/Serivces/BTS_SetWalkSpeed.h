// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_SetWalkSpeed.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTS_SetWalkSpeed : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTS_SetWalkSpeed();
	virtual void TickNode(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory,float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere)
	float MovementSpeed;	
	class ACharacter* OwnerCharacter;

};
