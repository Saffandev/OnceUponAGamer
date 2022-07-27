// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIGun.generated.h"

UCLASS()
class ONCEUPONAGAMER_API AAIGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIGun();
	virtual void Tick(float DeltaTime) override;
	virtual void StartShooting();
	virtual void StopShooting();
	virtual void ReleaseAbility() ;
	virtual bool GetIsReloading()
	{
		return bIsReloading;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:	
	virtual void ShootingInAction();
	virtual void Reload();
	void ReloadAfterEffect();
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* GunMesh;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Muzzel;
	UPROPERTY(EditAnywhere)
	UAnimMontage* CharacterShootMontage;
	UPROPERTY(EditAnywhere)
	UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere)
	UAnimationAsset* GunShootAnim;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> TracerRound;
	UPROPERTY(EditAnywhere)
	uint32 MaxAmmo;
	UPROPERTY(EditAnywhere)
	float FireRate;
	UPROPERTY(EditAnywhere)
	float Damage;
	uint32 CurrentAmmo;
	bool bIsReloading;
	FTimerHandle ShootingTimerHandle;

public:
	USkeletalMeshComponent* OwnerMesh;

};
