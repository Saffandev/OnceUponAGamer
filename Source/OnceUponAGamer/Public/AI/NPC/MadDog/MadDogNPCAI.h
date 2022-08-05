// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "MadDogNPCAI.generated.h"

UCLASS()
class ONCEUPONAGAMER_API AMadDogNPCAI : public ACharacter
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	FVector ThrowForceCalc();
	UFUNCTION()
	void TakePointDamage(AActor* DamagedActor,float Damage,AController* InstigatedBy, FVector HitLocation,UPrimitiveComponent* HitComp,FName BoneName,FVector ShotDirection,const UDamageType* DamageType,AActor* DamageCauser);
	UFUNCTION()
	void TakeRadialDamage(AActor* DamagedActor,float Damage,const UDamageType* DamageType,FVector Origin,FHitResult Hit,AController* InstigatedBy,AActor* DamageCauser);
	void DeathRituals(bool bIsExplosionDeath);
	void ShieldDamageTaken(float Damage,FVector HitLocation);
	UFUNCTION()
	void ShieldHitEffect();

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
	class UStaticMeshComponent* Shield;
	UPROPERTY(EditAnywhere)
	class UArrowComponent* ThrowArrow;
	class AMadDogHand* HandToThrow;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMadDogHand> HandToThrowBP;
	UPROPERTY(EditAnywhere)
	float PredectionDistance;
	UPROPERTY(EditAnywhere)
	float ThrowForce;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	bool bIsDead;
	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimationAsset* DeathAnim_1;
	UPROPERTY(EditAnywhere)
	float Health;
	UPROPERTY(EditAnywhere)
	float ShieldHealth;
	UPROPERTY(EditAnywhere)
	float ShieldHitRadius;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ShieldDebris;
	FTimeline ShieldHitTimeline;
	UPROPERTY(EditAnywhere)
	UCurveFloat* ShieldHitCurve;
	UPROPERTY(EditAnywhere)
	UCurveLinearColor* ShieldHitColorCurve;
	bool bIsShieldActive = true;

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bCanHandDamage;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	bool bCanThrowHand;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool bIsRecallingHand;
	
};
