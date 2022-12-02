// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CivilianAIController.generated.h"


UCLASS()
class ONCEUPONAGAMER_API ACivilianAIController : public AAIController
{
	// GENERATED_UCLASS_BODY()
	GENERATED_BODY()
	
public:
	ACivilianAIController();
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
	void Roaming();
	void RoamingTimer();
	bool SafeLocationCalc(FVector StimilusLocation,FVector& MoveLocation);

private:
	FTimerHandle MoveTimerHandle;
	FTimerHandle ViolenceClearedTimerHandle;
	UPROPERTY(EditAnywhere)
	float SafeDistance;
	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* AIPerceptionComponent;
	class UAISenseConfig_Violence* ViolenceSenseConfig;
	UPROPERTY(EditAnywhere)
	float ViolenceDetectionRange;
	class ACharacter* ControlledPawn;
};
