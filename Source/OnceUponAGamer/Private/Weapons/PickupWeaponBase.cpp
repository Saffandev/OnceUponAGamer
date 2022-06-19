// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/PickupWeaponBase.h"
#include "Weapons/WeaponBase.h"
#include "Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapons/KnifeWeapon.h"

// Sets default values
APickupWeaponBase::APickupWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	RootComponent = GunMesh;
	GunMesh->SetSimulatePhysics(true);
	
}

void APickupWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));

}

void APickupWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickupWeaponBase::PickupWeapon(){}

void APickupWeaponBase::PickupWeaponSetup(EWeaponName LWeaponName, TSubclassOf<AWeaponBase> LWeaponClass, TSubclassOf<APickupWeaponBase> LPickupWeaponClass, int LTotalAmmo,int LMaxAmmo, int LMagSize,int LCurrentMagAmmo,float LFireRate)
{
	
	if(UKismetMathLibrary::EqualEqual_ClassClass(PlayerCharacter->PrimaryWeapon.WeaponClass.Get(),LWeaponClass.Get())&&
		UKismetMathLibrary::EqualEqual_ClassClass(PlayerCharacter->SecondaryWeapon.WeaponClass.Get(),LWeaponClass.Get()))
		{
			return;
		}

	if(PlayerCharacter->PrimaryWeapon.WeaponClass->IsChildOf(AKnifeWeapon::StaticClass()))
	{
		PlayerCharacter->WeaponEquippedSlot = 0;
		PlayerCharacter->DropWeapon();
		PlayerCharacter->PrimaryWeapon.WeaponName 			= LWeaponName;
		PlayerCharacter->PrimaryWeapon.WeaponClass 			= LWeaponClass;
		PlayerCharacter->PrimaryWeapon.PickupWeaponClass 	= LPickupWeaponClass;
		PlayerCharacter->PrimaryWeapon.TotalAmmo 			= LTotalAmmo;
		PlayerCharacter->PrimaryWeapon.MaxAmmo 				= LMaxAmmo;
		PlayerCharacter->PrimaryWeapon.MagSize 				= LMagSize;
		PlayerCharacter->PrimaryWeapon.CurrentMagAmmo 		= LCurrentMagAmmo;
		PlayerCharacter->PrimaryWeapon.FireRate 			= LFireRate;
	}
	
	else if(PlayerCharacter->SecondaryWeapon.WeaponClass->IsChildOf(AKnifeWeapon::StaticClass()))
	{
		PlayerCharacter->WeaponEquippedSlot = 1;
		PlayerCharacter->DropWeapon();
		PlayerCharacter->SecondaryWeapon.WeaponClass 		= LWeaponClass;
		PlayerCharacter->SecondaryWeapon.PickupWeaponClass 	= LPickupWeaponClass;
		PlayerCharacter->SecondaryWeapon.TotalAmmo 			= LTotalAmmo;
		PlayerCharacter->SecondaryWeapon.MaxAmmo 			= LMaxAmmo;
		PlayerCharacter->SecondaryWeapon.MagSize 			= LMagSize;
		PlayerCharacter->SecondaryWeapon.CurrentMagAmmo 	= LCurrentMagAmmo;
		PlayerCharacter->SecondaryWeapon.FireRate 			= LFireRate;
	}

	else
	{
		if(PlayerCharacter->WeaponEquippedSlot == 0)
		{
			PlayerCharacter->DropWeapon();
			PlayerCharacter->PrimaryWeapon.WeaponClass 			= LWeaponClass;
			PlayerCharacter->PrimaryWeapon.PickupWeaponClass 	= LPickupWeaponClass;
			PlayerCharacter->PrimaryWeapon.TotalAmmo 			= LTotalAmmo;
			PlayerCharacter->PrimaryWeapon.MaxAmmo 				= LMaxAmmo;
			PlayerCharacter->PrimaryWeapon.MagSize 				= LMagSize;
			PlayerCharacter->PrimaryWeapon.CurrentMagAmmo 		= LCurrentMagAmmo;
			PlayerCharacter->PrimaryWeapon.FireRate 			= LFireRate;
		}

		else if(PlayerCharacter->WeaponEquippedSlot == 1)
		{
			PlayerCharacter->DropWeapon();
			PlayerCharacter->SecondaryWeapon.WeaponClass 		= LWeaponClass;
			PlayerCharacter->SecondaryWeapon.PickupWeaponClass 	= LPickupWeaponClass;
			PlayerCharacter->SecondaryWeapon.TotalAmmo 			= LTotalAmmo;
			PlayerCharacter->SecondaryWeapon.MaxAmmo 			= LMaxAmmo;
			PlayerCharacter->SecondaryWeapon.MagSize 			= LMagSize;
			PlayerCharacter->SecondaryWeapon.CurrentMagAmmo 	= LCurrentMagAmmo;
			PlayerCharacter->SecondaryWeapon.FireRate 			= LFireRate;
		}
	}

	PlayerCharacter->SwitchWeapon();
	this->Destroy();
}

void APickupWeaponBase::SetPickupWeaponName(){}


bool APickupWeaponBase::IsPickupGun()
{
	return true;
}