// Fill out your copyright notice in the Description page of Project Settings.


#include "HelperFunctions/HitEffectFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "config/HitEffectConfig.h"

TMap<TEnumAsByte<EPhysicalSurface>, FHitEffect> HitEffectDataContainer;
void UHitEffectFunctionLibrary::InitHitEffect()
{
	const UHitEffectConfig* HitEffectConfig = GetDefault<UHitEffectConfig>();
	HitEffectDataContainer = HitEffectConfig->HitEffectContainer;
}

void UHitEffectFunctionLibrary::PlayHitEffect(const UObject* WorldContextObject, const FHitResult Hit)
{
	EPhysicalSurface PhysicalSurface = UGameplayStatics::GetSurfaceType(Hit);
	FHitEffect* HitEffects = HitEffectDataContainer.Find(PhysicalSurface);
	if (HitEffects)
	{
		if (HitEffects->HitParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(WorldContextObject->GetWorld(), HitEffects->HitParticle, Hit.Location);
		}
		if (HitEffects->HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(WorldContextObject, HitEffects->HitSound, Hit.Location);
		}
	}
}

