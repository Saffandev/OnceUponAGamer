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
	UFUNCTION(BlueprintPure)
	USkeletalMeshComponent* GunMesh();
	
protected:
	virtual void BeginPlay() override;
	// virtual void Reload();
	UFUNCTION()
	void TakePointDamage(AActor* DamagedActor,float Damage,AController* InstigatedBy, FVector HitLocation,UPrimitiveComponent* HitComp,FName BoneName,FVector ShotDirection,const UDamageType* DamageType,AActor* DamageCauser);
	UFUNCTION()
	void TakeRadialDamage(AActor* DamagedActor,float Damage,const UDamageType* DamageType,FVector Origin,FHitResult Hit,AController* InstigatedBy,AActor* DamageCauser);
	UFUNCTION()
	void OnOverlapTouchSense(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, bool bFromSweep ,const FHitResult &SweepResult);
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, bool bFromSweep ,const FHitResult &SweepResult);	
	virtual void DeathRituals(bool bIsExplosionDeath);
	
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
	class AEncounterSpace* MyEncounterSpace;
	UPROPERTY(EditAnywhere)
	bool bCanAutoActivate = true;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool bIsDead;

protected:
	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* TouchSenseCapsule;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AAIGun> GunBp;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	AAIGun* Gun;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeaponBase> GunDrop;

	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimMontage* MeleeAttackMontage;
	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimationAsset* DeathAnim_1;
	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimationAsset* DeathAnim_2;
	UPROPERTY(EditAnywhere,Category = "Animation")
	UAnimationAsset* DeathAnim_3;

	UPROPERTY(EditAnywhere)
	float Health;
	FTimerHandle MeleeTimerHandle;
	class AAIController* AIController;
	UPROPERTY(EditAnywhere)
	USkeletalMesh* HeadlessMesh;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Head;
	FName LastHitBoneName;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	bool bCanCrouch;//	
	UPROPERTY(EditAnywhere)
	UParticleSystem* HitParticle;
	UPROPERTY(EditAnywhere)
	USoundBase* HitSound;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> HealthPickup;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ShieldPickup;
};
