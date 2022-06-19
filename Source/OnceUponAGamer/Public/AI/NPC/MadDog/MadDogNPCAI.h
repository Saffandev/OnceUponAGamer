// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MadDogNPCAI.generated.h"

UCLASS()
class ONCEUPONAGAMER_API AMadDogNPCAI : public ACharacter
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	FVector ThrowForceCalc();

public:
	// Sets default values for this character's properties
	AMadDogNPCAI();
	virtual void Tick(float DeltaTime) override;
	void ActivateForBattle();
	UFUNCTION(BlueprintCallable)
	void ThrowHand();
	void RecallHand();
	void HandRecallDone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	class AMadDogController* AIController;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* HandMesh;
	UPROPERTY(EditAnywhere)
	class UArrowComponent* ThrowArrow;
	class AMadDogHand* HandToThrow;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMadDogHand> HandToThrowBP;
	UPROPERTY(EditAnywhere)
	float PredectionDistance;
	UPROPERTY(EditAnywhere)
	float ThrowForce;

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bCanHandDamage;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	bool bCanThrowHand;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool bIsRecallingHand;
	
};
