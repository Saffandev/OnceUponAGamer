// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MadDogHand.generated.h"

UCLASS()
class ONCEUPONAGAMER_API AMadDogHand : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMadDogHand();
	virtual void Tick(float DeltaTime) override;
	void HandRecallInAction();

private:
	UFUNCTION()
	virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta = (ExposeOnSpawn = "true"))
	FVector ThrowForce;
	
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* HandMesh;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Point1;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Point2;
	class AMadDogNPCAI* OwnerAI;
	FTimerHandle HandRecallTimerHandle;
	UPROPERTY(EditAnywhere)
	float Damage;
	bool bDamageGiven;


};
