// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NPCAI_Action.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UNPCAI_Action : public UInterface
{
	GENERATED_BODY()
};

class ONCEUPONAGAMER_API INPCAI_Action
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void StartShooting() = 0;
	virtual void StopShooting() = 0;
	virtual float MeleeAttack() = 0;
};
