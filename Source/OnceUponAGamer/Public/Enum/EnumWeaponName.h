// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "EnumWeaponName.generated.h"


UENUM(BlueprintType)
enum class EWeaponName: uint8
{
	EWN_None,
	EWN_BAR30,
	EWN_BSG14,
	EWN_CG01
};

