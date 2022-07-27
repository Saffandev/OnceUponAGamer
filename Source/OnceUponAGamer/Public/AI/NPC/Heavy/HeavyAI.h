// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/NPC/Basic/BasicNPCAI.h"
#include "HeavyAI.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API AHeavyAI : public ABasicNPCAI
{
	GENERATED_BODY()

public:
	void ReleaseAbility();	

protected:
	virtual void BeginPlay();
	virtual void StartShooting() override;

private:
	FTimerHandle AbilityReleaseTimer;
	UPROPERTY(EditAnywhere)
	float AbilityTimerValue;
};
