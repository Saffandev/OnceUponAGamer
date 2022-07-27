// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "HeavyAIController.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API AHeavyAIController : public ABasicNPCAIController
{
	GENERATED_BODY()
	

public:
	AHeavyAIController();

protected:
	virtual void BeginPlay();
};
