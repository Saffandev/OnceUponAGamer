// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponBase.h"
#include "BSG15.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API ABSG15 : public AWeaponBase
{
	GENERATED_BODY()
public:
	ABSG15();

protected:
	virtual void BeginPlay() override;
	// virtual void PickupWeapon() override;
	
};
