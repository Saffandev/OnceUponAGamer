// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BAR30Pickup.h"

void ABAR30Pickup::PickupWeapon() 
{
    PickupWeaponSetup(WeaponName, WeaponClass, PickupBP, TotalAmmo, MaxAmmo, MagSize, CurrentMagAmmo, FireRate);
    // UE_LOG(LogTemp,Warning,TEXT("Pickup weapon"));
}