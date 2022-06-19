// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/PickupWeaponInterface.h"
#include "Enum/EnumWeaponName.h"
#include "PickupWeaponBase.generated.h"

UCLASS()
class ONCEUPONAGAMER_API APickupWeaponBase : public AActor,public IPickupWeaponInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupWeaponBase();
	virtual void PickupWeapon() override;
	virtual void SetPickupWeaponName() override;
	virtual bool IsPickupGun() override;
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void PickupWeaponSetup(EWeaponName LWeaponName, TSubclassOf<class AWeaponBase> LWeaponClass, TSubclassOf<APickupWeaponBase> LPickupWeaponClass, int LTotalAmmo,int LMaxAmmo, int LMagSize,int LCurrentMagAmmo,float LFireRate);

private:

public:	
	class APlayerCharacter* PlayerCharacter;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* GunMesh;

	UPROPERTY(EditAnywhere)
	int MaxAmmo;

	UPROPERTY(EditAnywhere)
	int TotalAmmo;

	UPROPERTY(EditAnywhere)
	int MagSize;

	UPROPERTY(EditAnywhere)
	int CurrentMagAmmo;
	
	UPROPERTY(EditAnywhere)
	float Accuracy;

	UPROPERTY(EditAnywhere)
	float FireRate;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeaponBase> WeaponClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APickupWeaponBase> PickupBP;

	UPROPERTY(EditAnywhere)
	EWeaponName WeaponName;

	APickupWeaponBase* PickupWeaponClass;

};
