// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/NPCAI_Action.h"
#include "Interface/AI_GunInterface.h"
#include "BaseAI.generated.h"


UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	EET_Assault,
	EET_Shotgun,
	EET_Healer
};

UCLASS()
class ONCEUPONAGAMER_API ABaseAI : public ACharacter, public INPCAI_Action, public IAI_GunInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseAI();
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	virtual void StartShooting() override;
	UFUNCTION(BlueprintCallable)
	virtual void StopShooting() override;
	virtual float MeleeAttack() override;
	UFUNCTION(BlueprintCallable)
	virtual float GetRemainingHealth();
	virtual void SetReload(bool bIsReloading);
	UFUNCTION(BlueprintPure)
	virtual bool GetIsReloading();
	UFUNCTION(BlueprintPure)
	virtual USkeletalMeshComponent* GetWeaponMesh();
	UFUNCTION(BlueprintPure)
		EEnemyType GetEnemyType()
	{
		return EnemyType;
	}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnOverlapTouchSense(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void TakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* HitComp, FName BoneName, FVector ShotDirection, const UDamageType* DamageType, AActor* DamageCauser);
	UFUNCTION()
	virtual void TakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, FHitResult Hit, AController* InstigatedBy, AActor* DamageCauser);
	virtual void DeathRituals(bool bIsExplosionDeath);
	virtual void HealthVisuals();
	virtual void Explode();
	virtual void ExplosionEffect();

public:	
	TArray<FVector> PatrolPoint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
	class APatrolPoint* PatrolPointObj;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsShooting;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsMeleeAttack;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsReloading;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class AEncounterSpace* MyEncounterSpace;
	UPROPERTY(EditAnywhere)
	bool bCanAutoActivate = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDead;
	class ACover* ActiveCover;

protected:
	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* TouchSenseCapsule;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AAIGun> GunBp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AAIGun* Gun;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeaponBase> GunDrop;
	UPROPERTY(EditAnywhere)
		FName GunSocketName;
	UPROPERTY(EditAnywhere, Category = "Animation")
		UAnimMontage* MeleeAttackMontage;
	UPROPERTY(EditAnywhere)
	float Health;
	float CurrentHealth;
	FTimerHandle MeleeTimerHandle;
	class AAIController* AIController;
	UPROPERTY(EditAnywhere)
		USkeletalMesh* HeadlessMesh;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> Head;
	FName LastHitBoneName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		bool bCanCrouch;//	
	UPROPERTY(EditAnywhere)
		UParticleSystem* HitParticle;
	UPROPERTY(EditAnywhere)
		USoundBase* HitSound;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> ExplosionActor;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> HealthPickup;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> ShieldPickup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsWantedLevelCooldown = true;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UDamageType> ExplosionDamageType;
	UPROPERTY(EditAnywhere)
		float ExplosionDamage;
	UPROPERTY(EditAnywhere)
		float ExplosionRadius;
	UPROPERTY(EditAnywhere)
		EEnemyType EnemyType;
};
