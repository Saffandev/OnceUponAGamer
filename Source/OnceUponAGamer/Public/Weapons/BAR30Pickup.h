// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/PickupWeaponBase.h"
#include "BAR30Pickup.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API ABAR30Pickup : public APickupWeaponBase
{
	GENERATED_BODY()

public:
virtual void PickupWeapon() override;
};
