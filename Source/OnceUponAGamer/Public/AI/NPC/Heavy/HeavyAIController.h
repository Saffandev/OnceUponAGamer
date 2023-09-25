// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/NPC/BaseAIController.h"
#include "HeavyAIController.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API AHeavyAIController : public ABaseAIController
{
	GENERATED_BODY()
	

public:
	AHeavyAIController();

protected:
	virtual void BeginPlay();
};
