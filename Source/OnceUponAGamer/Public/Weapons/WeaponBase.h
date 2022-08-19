// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/PickupWeaponInterface.h"
#include "Enum/EnumWeaponName.h"
#include "WeaponBase.generated.h"

UCLASS()
class ONCEUPONAGAMER_API AWeaponBase : public AActor,public IPickupWeaponInterface
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
	virtual TSubclassOf<AWeaponBase> GetPickupWeapon();
	virtual float DoMeleeAttack();
	virtual void PickupWeapon() override;
	virtual void SetPickupWeaponName() override;
	virtual bool IsPickupGun() override;
	void DropGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Recoil();
	void PickupWeaponSetup(EWeaponName LWeaponName, TSubclassOf<AWeaponBase> LWeaponClass, int LTotalAmmo,int LMaxAmmo, int LMagSize,int LCurrentMagAmmo,float LFireRate);


private:
	UFUNCTION()
	void ShootingInAction();
	void UpdateWeaponVisuals();
	void UpdateWeaponVarsInPlayer();
	void GiveDamage(FHitResult GunHit);
	float DamagePerBone(FName BoneName);
	void HitSound(FHitResult GunHit);

public:
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* GunMesh;
protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Muzzle;
	
	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimationAsset* ShootAnim;

	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimationAsset* GunReloadAnim;

	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimMontage* PlayerShootMontage;

	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimMontage* PlayerCrouchShootMontage;

	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimMontage* PlayerAdsShootingMontage;
	
	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimMontage* PlayerReloadMontage;

	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimMontage* WeaponDrawMontage;

	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimMontage* MeleeAttackMontage;
	
	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	TSubclassOf<AActor> TracerRound;
	
	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	TSubclassOf<UCameraShakeBase> CameraShake;
	
	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	float CameraShakeScale;
	
	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	TSubclassOf<AWeaponBase> WeaponBP;
	
	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	EWeaponName WeaponName;
	// UPROPERTY(EditAnywhere)
	// TSubclassOf<class APickupWeaponBase>  PickupWeapon;
	
	float ReloadTime;
	bool bIsReloading;
	float SingleShotAlpha;
	bool bCanShoot;
	class APlayerCharacter* PlayerCharacter;
	class UCameraComponent* PlayerCamera;
	UPROPERTY(EditAnywhere,Category = "Weapon Hit Visuals")
	USoundBase* WoodHit;
	UPROPERTY(EditAnywhere,Category = "Weapon Hit Visuals")
	USoundBase* MetalHit;
	UPROPERTY(EditAnywhere,Category = "Weapon Hit Visuals")
	USoundBase* StoneHit;
	UPROPERTY(EditAnywhere,Category = "Weapon Hit Visuals")
	UParticleSystem* WoodHitParticle;
	UPROPERTY(EditAnywhere,Category = "Weapon Hit Visuals")
	UParticleSystem* MetalHitParticle;
	UPROPERTY(EditAnywhere,Category = "Weapon Hit Visuals")
	UParticleSystem* StoneHitParticle;
private:
	FTimerHandle ShootingTimerHandle;
	UPROPERTY(EditAnywhere)
	float BaseDamage;
	UPROPERTY(EditAnywhere)
	float CriticalDamage;
	UPROPERTY(EditAnywhere)
	float HeadshotDamage;

public:
	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	int TotalAmmo;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Weapon Vars")
	int MaxAmmo;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Weapon Vars")
	int MagSize;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Weapon Vars")
	int CurrentMagAmmo;

	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	float Accuracy;

	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	float FireRate;

	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	float Range;

	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	bool bIsWeaponShootable;

	bool bIsPrimaryWeapon;

	bool bIsPlayerHoldingTheWeapon;


};
