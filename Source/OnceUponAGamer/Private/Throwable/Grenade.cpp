// Fill out your copyright notice in the Description page of Project Settings.


#include "Throwable/Grenade.h"

void AGrenade::PickupWeapon()
{
    UE_LOG(LogTemp,Warning,TEXT("Inside the throwable"));
    SetupPickupThrowable(BP_Throwable);
}
