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
	AWeaponBase();
	virtual void Tick(float DeltaTime) override;
	virtual void Shoot();
	virtual void StopShooting();
	virtual void Reload();
	virtual void ReloadEffect();
	virtual void DrawWeapon(float Drawspeed = 1.f);
	virtual void UnDrawWeapon(float DrawSpeed = 1.f);
	virtual void PickupWeapon() override;
	virtual void SetPickupWeaponName() override;
	virtual bool IsPickupGun() override;
	virtual float DoMeleeAttack();
	virtual TSubclassOf<AWeaponBase> GetPickupWeapon();
	void DropGun();
	void SetCanShoot(bool bLCanShoot);


protected:
	virtual void BeginPlay() override;
	virtual void Recoil();

private:
	UFUNCTION()
	void ShootingInAction();
	void UpdateWeaponVisuals();
	void UpdateWeaponVarsInPlayer();
	void GiveDamage(FHitResult GunHit);
	void HitEffect(FHitResult GunHit);
	float DamagePerBone(FName BoneName);
	void ImpactEffect(FHitResult GunHit,UParticleSystem* HitParticle,USoundBase* HitSound, UMaterialInterface* HitDecal);

public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USkeletalMeshComponent* GunMesh;

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Muzzle;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* SightMesh;
	
	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimationAsset* GunShootAnim;

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
	UAnimMontage* WeaponUnDrawMontage;
	
	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimMontage* MeleeAttackMontage;
	
	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	TSubclassOf<AActor> TracerRound;
	
	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	TSubclassOf<UCameraShakeBase> CameraShake;
	
	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	float CameraShakeScale;
	
	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	float CameraShakeScaleADS;
	
	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	TSubclassOf<AWeaponBase> WeaponBP;
	
	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	EWeaponName WeaponName;

	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	bool bIsAuto;
	
	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	int32 PalletCount = 1;

	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	float TraceOffset;
	
	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	float CoolDownTime = 0.f;
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
	UPROPERTY(EditAnywhere,Category = "Weapon Hit Visuals")
	UMaterialInterface* WoodHitDecal;
	UPROPERTY(EditAnywhere,Category = "Weapon Hit Visuals")
	UMaterialInterface* MetalHitDecal;
	UPROPERTY(EditAnywhere,Category = "Weapon Hit Visuals")
	UMaterialInterface* StoneHitDecal;
	UPROPERTY(EditAnywhere,Category = "Weapon Hit Visuals")
	UParticleSystem* TrailsFX;
	
private:
	FTimerHandle ShootingTimerHandle;
	FTimerHandle ShootingCooldownTimerHandle;
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

	UPROPERTY(EditAnywhere,Category = "Weapon Vars")
	int32 PickupAmmoCount;
	bool bIsPrimaryWeapon;
	bool bIsPlayerHoldingTheWeapon;


};
