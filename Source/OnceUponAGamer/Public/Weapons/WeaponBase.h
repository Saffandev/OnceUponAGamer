// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS()
class ONCEUPONAGAMER_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();
	virtual void Tick(float DeltaTime) override;
	virtual void Shoot();
	virtual void StopShooting();
	virtual void Reload();
	virtual void ReloadEffect();
	virtual void DrawWeapon();
	virtual TSubclassOf<class APickupWeaponBase> GetPickupWeapon();
	virtual float DoMeleeAttack();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Recoil();

private:
	UFUNCTION()
	void ShootingInAction();
	void UpdateWeaponVisuals();
	void UpdateWeaponVarsInPlayer();
	void GiveDamage(FHitResult GunHit);
	float DamagePerBone(FName BoneName);


protected:
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* GunMesh;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Muzzle;
	
	UPROPERTY(EditAnywhere)
	UAnimationAsset* ShootAnim;

	UPROPERTY(EditAnywhere)
	UAnimationAsset* GunReloadAnim;

	UPROPERTY(EditAnywhere)
	UAnimMontage* PlayerShootMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* PlayerCrouchShootMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* PlayerAdsShootingMontage;
	
	UPROPERTY(EditAnywhere)
	UAnimMontage* PlayerReloadMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* WeaponDrawMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* MeleeAttackMontage;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> TracerRound;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShakeBase> CameraShake;
	
	UPROPERTY(EditAnywhere)
	float CameraShakeScale;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> WeaponBP;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APickupWeaponBase>  PickupWeapon;
	
	float ReloadTime;
	bool bIsReloading;
	float SingleShotAlpha;
	bool bCanShoot;
	class APlayerCharacter* PlayerCharacter;
	class UCameraComponent* PlayerCamera;

private:
	FTimerHandle ShootingTimerHandle;
	UPROPERTY(EditAnywhere)
	float BaseDamage;
	UPROPERTY(EditAnywhere)
	float CriticalDamage;
	UPROPERTY(EditAnywhere)
	float HeadshotDamage;

public:
	UPROPERTY(EditAnywhere)
	int TotalAmmo;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int MaxAmmo;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int MagSize;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	int CurrentMagAmmo;

	UPROPERTY(EditAnywhere)
	float Accuracy;

	UPROPERTY(EditAnywhere)
	float FireRate;

	UPROPERTY(EditAnywhere)
	float Range;

	UPROPERTY(EditAnywhere)
	bool bIsWeaponShootable;

	bool bIsPrimaryWeapon;


};
