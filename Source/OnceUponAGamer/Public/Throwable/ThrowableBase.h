// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/PickupWeaponInterface.h"
#include "ThrowableBase.generated.h"

UCLASS()
class ONCEUPONAGAMER_API AThrowableBase : public AActor,public IPickupWeaponInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThrowableBase();
	virtual void PickupWeapon() override;
	virtual void SetPickupWeaponName() override;
	virtual bool IsPickupGun() override;
	virtual void Tick(float DeltaTime) override;
	void Throw(FVector ThrowVelocity);
	void Initiate();	
	
protected:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Explode();
	void SetupPickupThrowable(TSubclassOf<AThrowableBase> LThrowable);

private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent,AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult &Hit);

protected:
	UPROPERTY(EditAnywhere)
	float ExplodeTimer;
	UPROPERTY(EditAnywhere)
	float DamageRadius;
	UPROPERTY(EditAnywhere)
	float Damage;
	UPROPERTY(EditAnywhere)
	UParticleSystem* ExplodeParticle;
	UPROPERTY(EditAnywhere)
	USoundBase* ExplodeSound;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UDamageType> ExplosionDamageType;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ThrowableMesh;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AThrowableBase> BP_Throwable;
	class APlayerCharacter* PlayerCharacter;

private:
	
	uint32 HitCount;
	FTimerHandle ThrowTimer;
	TArray<TEnumAsByte<EObjectTypeQuery>> OverlapActorObjectType;
};
