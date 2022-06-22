// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/NPCAI_Action.h"
#include "Interface/AI_GunInterface.h"
#include "BasicNPCAI.generated.h"

UCLASS()
class ONCEUPONAGAMER_API ABasicNPCAI : public ACharacter,public INPCAI_Action,public IAI_GunInterface
{
	GENERATED_BODY()

public:
	ABasicNPCAI();
	virtual void Tick(float DeltaTime) override;
	virtual void StartShooting();
	virtual void StopShooting();
	virtual float MeleeAttack();
	virtual void SetReload(bool bIsReloading);
	void CanTakeCover(bool bCanTakeCover);
	UFUNCTION(BlueprintPure)
	bool GetIsReloading();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void ShootingInAction();
	void Reload();
	UFUNCTION()
	void TakePointDamage(AActor* DamagedActor,float Damage,AController* InstigatedBy, FVector HitLocation,UPrimitiveComponent* HitComp,FName BoneName,FVector ShotDirection,const UDamageType* DamageType,AActor* DamageCauser);
	UFUNCTION()
	void TakeRadialDamage(AActor* DamagedActor,float Damage,const UDamageType* DamageType,FVector Origin,FHitResult Hit,AController* InstigatedBy,AActor* DamageCauser);
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, bool bFromSweep ,const FHitResult &SweepResult);
	void DeathRituals(bool bIsExplosionDeath);
	
public:
	TArray<FVector> PatrolPoint;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta = (ExposeOnSpawn = "true"))
	class APatrolPoint* PatrolPointObj;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool bIsShooting;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool bIsMeleeAttack;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool bIsReloading;
	class ACover* ActiveCover;

private:
	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* TouchSenseCapsule;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	bool bCanCrouch;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AAIGun> GunBp;
	AAIGun* Gun;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class APickupWeaponBase> DropGun;
	UPROPERTY(EditAnywhere)
	UAnimMontage* MeleeAttackMontage;
	UPROPERTY(EditAnywhere)
	UAnimationAsset* DeathAnim_1;
	UPROPERTY(EditAnywhere)
	UAnimationAsset* DeathAnim_2;
	UPROPERTY(EditAnywhere)
	UAnimationAsset* DeathAnim_3;
	UPROPERTY(EditAnywhere)
	float Health;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess = "true"))
	bool bIsDead;
	FTimerHandle MeleeTimerHandle;
	class AAIController* AIController;

};
