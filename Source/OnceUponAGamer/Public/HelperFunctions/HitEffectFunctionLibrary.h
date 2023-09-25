// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HitEffectFunctionLibrary.generated.h"

/**
 * 
 */
USTRUCT()
struct FHitEffects
{
	GENERATED_BODY()
};
UCLASS()
class ONCEUPONAGAMER_API UHitEffectFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HitEffect")
	static void InitHitEffect();
	UFUNCTION(BlueprintCallable, Category = "HitEffect",meta = (WorldContext = "WorldContextObject"))
	static void PlayHitEffect(const UObject* WorldContextObject,const FHitResult Hit);
};
