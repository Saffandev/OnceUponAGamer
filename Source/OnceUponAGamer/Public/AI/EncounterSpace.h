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
	void AssingInvestigation(FVector SuspectLocation);
	void MoveBackToPatrol();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SetupController();

public:
	TArray<AActor*> OverlappedCovers;

private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* Box;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACover> CoverBp;
	TArray<AActor*> OverlappedAI;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABasicNPCAI> AIBp;
};
