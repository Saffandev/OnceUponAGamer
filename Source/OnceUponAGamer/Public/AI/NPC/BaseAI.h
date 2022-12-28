// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/NPCAI_Action.h"
#include "Interface/AI_GunInterface.h"
#include "BaseAI.generated.h"

UCLASS()
class ONCEUPONAGAMER_API ABaseAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseAI();
	virtual void Tick(float DeltaTime) override;
	//virtual void StartShooting() override;
	//virtual void StopShooting() override;
	//virtual float MeleeAttack() override;
	//virtual void SetReload(bool bIsReloading);
	//UFUNCTION(BlueprintPure)
	//virtual bool GetIsReloading();
	//UFUNCTION(BlueprintPure)
	//virtual USkeletalMeshComponent* GetWeaponMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//UFUNCTION()
	//virtual void TakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* HitComp, FName BoneName, FVector ShotDirection, const UDamageType* DamageType, AActor* DamageCauser);
	//UFUNCTION()
	//virtual void TakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, FHitResult Hit, AController* InstigatedBy, AActor* DamageCauser);
	//UFUNCTION()
	//virtual void OnOverlapTouchSense(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//UFUNCTION()
	//virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//virtual void DeathRituals(bool bIsExplosionDeath);
	//virtual void HealthVisuals();
	//virtual void Explode();
	//virtual void ExplosionEffect();
public:	

};
