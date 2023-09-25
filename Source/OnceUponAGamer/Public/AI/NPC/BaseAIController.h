// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseAIController.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API ABaseAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ABaseAIController();
	class ABaseAI* GetControlledPawn();
	UFUNCTION(BlueprintCallable)
	void Activate();
	void InitOwner();
	UFUNCTION(BlueprintCallable)
	void ClearSenses();

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnPerceptionUpdated(TArray<AActor*>const& SensedActors);
	void CheckPlayerVisibility();

public:
	class AEncounterSpace* MyEncounterSpace;
	bool bIsOwnerAlive;

protected:
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
	class ABaseAI* OwnerAI;
	bool bIsUnpossed;
	UPROPERTY(EditAnywhere)
	float PlayerCheckAngle;
};
