// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_AttackMadDog.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UBTT_AttackMadDog : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_AttackMadDog();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent &OwnerComp,uint8 *NodeMemory, float DeltaTime) override;
	void MontagePlay(UAnimMontage* Montage);

private:
	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> AttackMontage;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_bIsHoldingHand;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BB_bCanThrowHand;
	UPROPERTY(EditAnywhere)
	bool bIsThisThrowHandTask;
	class AMadDogNPCAI* OwnerPawn;
	float AttackTime;
	FTimerHandle EndHandle;
	bool AttackFinished = false;


};
