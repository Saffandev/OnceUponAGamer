// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_Shooting.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTT_Shooting : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTT_Shooting();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


private:
	UPROPERTY(EditAnywhere)
	bool bCanShoot;
	UPROPERTY(EditAnywhere)
	bool bCanMeleeAttack;
	float AttackTime = 0;
	bool bIsDoingMeleeAttack;
    FTimerHandle MeleeTimerHandle;

};
