// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MadDogController.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API AMadDogController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMadDogController();
	void Activate();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;

public:
	class AMadDogNPCAI* OwnerPawn;


};
