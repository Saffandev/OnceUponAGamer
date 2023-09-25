// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "HitEffectConfig.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct FHitEffect
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HitEffects")
	class USoundBase* HitSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HitEffects")
	class UParticleSystem* HitParticle;
};
UCLASS(Config = Game, defaultconfig)
class ONCEUPONAGAMER_API UHitEffectConfig : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, config, Category = "HitEffects")
	TMap<TEnumAsByte<EPhysicalSurface>, FHitEffect> HitEffectContainer;
	const TMap<TEnumAsByte<EPhysicalSurface>, FHitEffect> GetHitEffectData();
};
