// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/NPC/Basic/AIGun.h"
#include "AIShotGun.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API AAIShotGun : public AAIGun
{
	GENERATED_BODY()

public:
	virtual void ReleaseAbility();

protected:
	virtual void BeginPlay();
	virtual void ShootingInAction();

private:
	void ShootingInActionContinues();

private:
	UPROPERTY(EditAnywhere)
	float ShotgunPalletCount;
	UPROPERTY(EditAnywhere)
	float RandomDeviation;
	FTimerHandle AbilityTimer;
	UPROPERTY(EditAnywhere)
	uint32 AbilityBulletCount;
	uint32 CurrentAbilityBulletCount;
	UPROPERTY(EditAnywhere)
	float AbilitySpeed;
};
