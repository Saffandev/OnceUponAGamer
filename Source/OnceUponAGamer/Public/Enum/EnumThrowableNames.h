// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "EnumThrowableNames.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EThrowableName: uint8
{
	EWN_None,
	EWN_Grenade,
	EWN_EMP
};
