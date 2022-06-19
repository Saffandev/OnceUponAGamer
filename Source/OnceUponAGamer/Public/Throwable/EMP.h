// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Throwable/ThrowableBase.h"
#include "EMP.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API AEMP : public AThrowableBase
{
	GENERATED_BODY()
protected:
	virtual void PickupWeapon() override;
};
