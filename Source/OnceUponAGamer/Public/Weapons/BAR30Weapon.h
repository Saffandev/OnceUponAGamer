// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponBase.h"
#include "BAR30Weapon.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API ABAR30Weapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	ABAR30Weapon();
protected:
	virtual void BeginPlay() override;
	// virtual void PickupWeapon() override;
};
