// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponBase.h"
#include "KnifeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API AKnifeWeapon : public AWeaponBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
};
