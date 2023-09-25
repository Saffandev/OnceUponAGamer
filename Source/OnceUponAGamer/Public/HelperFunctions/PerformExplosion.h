// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PerformExplosion.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UPerformExplosion : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Damage",meta = (AutoCreateRefTerm = "ActorsToIgnore", WorldContext = "WorldContextObject"))
	static void StartExplosion(const UObject* WorldContextObject,float BaseDamage, FVector Origin, float DamageRadius, AActor* DamageCauser, TArray<AActor*> ActorsToIgnore, USoundBase* ExplosionSound = nullptr, UParticleSystem* ExplosionParticles = nullptr, TSubclassOf<UDamageType> ExplosionDamageType = NULL, TSubclassOf<UCameraShakeBase> CameraShake = NULL, float CameraShakeValue = 0.f);
};
