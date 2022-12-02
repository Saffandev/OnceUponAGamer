// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "EnumViolenceType.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EViolenceType: uint8
{
	EVT_None = 0  UMETA(DisplayName = "None"),
	EVT_AirFire = 1  UMETA(DisplayName = "AirFire"),
	EVT_HittedSomethingNonDamaging = 2  UMETA(DisplayName = "HittedSomethingNonDamaging"),
	EVT_HittedSomethingDamaging = 3  UMETA(DisplayName = "HittedSomethingDamaging"),
	EVT_Explosion = 4  UMETA(DisplayName = "Explosion"),
	EVT_CivilianKilled = 5  UMETA(DisplayName = "CivilianKilled"),
	EVT_PoliceKilled = 6  UMETA(DisplayName = "PoliceKilled")
};