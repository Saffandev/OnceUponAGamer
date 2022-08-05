// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cover.generated.h"

UCLASS()
class ONCEUPONAGAMER_API ACover : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACover();
	FVector GiveCoverPoints();
	FVector GivePeakPoints(AActor* Requester);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FVector ClosestPointToActor(AActor* CompareActor,TArray<FVector> Points);

private:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Box;
	UPROPERTY(EditAnywhere,meta = (MakeEditWidget = "true"))
	TArray<FVector> CoverPoints;
	UPROPERTY(EditAnywhere,meta = (MakeEditWidget = "true"))
	TArray<FVector> PeakCoverPoints;

public:	
	bool bIsAcquired;
	UPROPERTY(EditAnywhere)
	bool bIsPeekCover;



};
