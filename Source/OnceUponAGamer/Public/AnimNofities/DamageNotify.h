// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "DamageNotify.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UDamageNotify : public UAnimNotifyState
{
	GENERATED_BODY()
	

private:
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

private:
	bool bCanApplyDamage = true;
	UPROPERTY(EditAnywhere)
	FName StartBoneName;
	UPROPERTY(EditAnywhere)
	FName EndBoneName;
	UPROPERTY(EditAnywhere)
	float Damage;
};
