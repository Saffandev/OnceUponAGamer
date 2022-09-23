// Fill out your copyright notice in the Description page of Project Settings.


#include "Throwable/ImpactGrenade.h"

void AImpactGrenade::PickupWeapon()
{
    // UE_LOG(LogTemp,Warning,TEXT("Inside the throwable"));
    SetupPickupThrowable(BP_Throwable);
}