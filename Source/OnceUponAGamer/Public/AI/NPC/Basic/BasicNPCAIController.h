// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BasicNPCAIController.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API ABasicNPCAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABasicNPCAIController();
	class ABasicNPCAI* GetControlledPawn();
	void CoverRequest();
	void ToggleSightSense();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnPerceptionUpdated(TArray<AActor*>const& SensedActors);
	void CheckPlayerVisibility();

public:
	class AEncounterSpace* MyEncounterSpace;
	bool bIsOwnerAlive;

private:
	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* AIPerceptionComponent;
	class UAISenseConfig_Sight* SightSense;
	class UAISenseConfig_Damage* DamageSense;
	class UAISenseConfig_Hearing* HearingSense;
	class UAISenseConfig_Touch* TouchSense;
	class APlayerCharacter* PlayerCharacter;
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;
	class UBlackboardComponent* Blackboard;
	class ABasicNPCAI* OwnerAI;
    FTimerHandle CoverTimerHandle;

	
};
