// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/NPC/BaseAIController.h"
#include "BasicNPCAIController.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API ABasicNPCAIController : public ABaseAIController
{
	GENERATED_BODY()

public:
	void CoverRequest();

protected:
	virtual void BeginPlay() override;
	void CoverRequestEntry();

protected:
	FTimerHandle CoverTimerHandle;

};
