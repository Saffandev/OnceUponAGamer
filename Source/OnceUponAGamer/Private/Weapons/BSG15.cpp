// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BSG15.h"


ABSG15::ABSG15()
{

}


void ABSG15::BeginPlay()
{
    Super::BeginPlay();
}
void ABSG15::PickupWeapon() 
{
    PickupWeaponSetup(WeaponName, WeaponBP, TotalAmmo, MaxAmmo, MagSize, CurrentMagAmmo, FireRate);
    UE_LOG(LogTemp,Warning,TEXT("Pickup weapon"));
}