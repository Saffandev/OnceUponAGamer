// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Throwable/ThrowableBase.h"
#include "Grenade.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API AGrenade : public AThrowableBase
{
	GENERATED_BODY()
protected:
	virtual void PickupWeapon() override;
};
