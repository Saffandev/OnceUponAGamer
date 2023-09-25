// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EncounterSpace.generated.h"

UCLASS()
class ONCEUPONAGAMER_API AEncounterSpace : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEncounterSpace();
	virtual void Tick(float DeltaTime) override;
	bool IsPlayerVisibleToAnyone();
	UFUNCTION(BlueprintCallable)
	void AssingInvestigation(FVector SuspectLocation);
	void MoveBackToPatrol();
	void AddAI(class ACharacter* AI);
	void IAMDead(class ACharacter* DeadAI);
	UFUNCTION(BlueprintImplementableEvent)
	void TheratCleared();
	UFUNCTION(BlueprintImplementableEvent)
	void ThreatDetected();
	void RequestAllies(FVector SpawnLocation);
	UFUNCTION(BlueprintImplementableEvent)
	void SomeoneIsDead();
	void ICanSeePlayer();
	UFUNCTION(BlueprintCallable)
	void ClearAllTheAI();
	UFUNCTION(BlueprintCallable)
	class ABaseAI* GetClosestDamagedAI(AActor* Requester);
	void AddDamagedAI(class ABaseAI* DamagedAI);
	void RemoveDamagedAI(class ABaseAI* RecoveredAI);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	void SetupController();

public:
	TArray<AActor*> OverlappedCovers;
	TArray<AActor*> OverlappedAI;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TArray<class ABaseAIController*> OverlappedAIControllers;

	bool bPlayerSpotted;

private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* Box;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACover> CoverBp;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABaseAI> AIBp;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	int32 NoOfAiAlive;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "trues"))
	TSet<class ABaseAI*> DamagedAIs;
};
