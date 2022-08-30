// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPoint.generated.h"

UCLASS()
class ONCEUPONAGAMER_API APatrolPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APatrolPoint();	

	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta = (MakeEditWidget = "true"))
	TArray<FVector> PatrolPointLocation;
	

};
