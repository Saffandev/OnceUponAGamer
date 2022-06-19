// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "EnumWeaponName.generated.h"


UENUM(BlueprintType)
enum class EWeaponName: uint8
{
	EWN_None,
	EWN_BAR30
};
// UCLASS()
// class ONCEUPONAGAMER_API UEnumWeaponName : public UUserDefinedEnum
// {
// 	GENERATED_BODY()
	
// };
