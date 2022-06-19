// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupWeaponInterface.generated.h"

/**
 * 
 */

UINTERFACE()
class ONCEUPONAGAMER_API UPickupWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

class IPickupWeaponInterface
{
	GENERATED_BODY()

public:
	virtual void PickupWeapon() ;
	virtual void SetPickupWeaponName() ;	
	virtual bool IsPickupGun() ;
};
