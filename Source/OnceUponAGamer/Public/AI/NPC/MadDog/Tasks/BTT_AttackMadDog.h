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
	void End();
	void MontagePlay(UAnimMontage* Montage);

private:
	void AttackEnded(UBehaviorTreeComponent &OwnerComp);
private:
	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage1;
	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage2;
	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage3;
	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage4;
	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage5;
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
