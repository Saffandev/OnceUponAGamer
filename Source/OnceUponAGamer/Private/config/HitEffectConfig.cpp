// Fill out your copyright notice in the Description page of Project Settings.


#include "config/HitEffectConfig.h"

const TMap<TEnumAsByte<EPhysicalSurface>, FHitEffect> UHitEffectConfig::GetHitEffectData()
{
	return HitEffectContainer;
}
