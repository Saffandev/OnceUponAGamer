// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_SetAttackingRange.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTS_SetAttackingRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTS_SetAttackingRange();

private:
	virtual void TickNode(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory,float DeltaTime) override;


private:
	UPROPERTY(EditAnywhere)
	float MaxShootingRange;
	UPROPERTY(EditAnywhere)
	float MinShootingRange;
	UPROPERTY(EditAnywhere)
	float AttackingAngle;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_bInShootingRange;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_bInMeleeRange;
	bool bCanCheckForMelee = true;
	FTimerHandle MeleeTimerHandle;
};
