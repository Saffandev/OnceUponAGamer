// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/PickupWeaponBase.h"
#include "KnifePickup.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API AKnifePickup : public APickupWeaponBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
};
