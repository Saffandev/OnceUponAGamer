// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AI_GunInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAI_GunInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ONCEUPONAGAMER_API IAI_GunInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

virtual void SetReload(bool bIsReloading) = 0;

};
