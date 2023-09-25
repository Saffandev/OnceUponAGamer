// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/NPC/BaseAI.h"
#include "BasicNPCAI.generated.h"

UCLASS()
class ONCEUPONAGAMER_API ABasicNPCAI : public ABaseAI
{
	GENERATED_BODY()

public:
	ABasicNPCAI();
	virtual void Tick(float DeltaTime) override;
	void CanTakeCover(bool bCanTakeCover);
	void ReleaseCover();

protected:
	virtual void BeginPlay() override;
	// virtual void Reload();
	virtual void DeathRituals(bool bIsExplosionDeath) override;


protected:
	
	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimationAsset* DeathAnim_1;
	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimationAsset* DeathAnim_2;
	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimationAsset* DeathAnim_3;

	
};
