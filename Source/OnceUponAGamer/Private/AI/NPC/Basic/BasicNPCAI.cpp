// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/BasicNPCAI.h"
#include "AI/PatrolPoint.h"
#include "AI/NPC/Basic/AIGun.h"
#include "AI/Cover.h"
#include "AI/EncounterSpace.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameframeWork/CharacterMovementComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Weapons/WeaponBase.h"
#include "BrainComponent.h"
#include "HelperFunctions/ViolenceRegistration.h"
// Sets default values
ABasicNPCAI::ABasicNPCAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABasicNPCAI::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABasicNPCAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABasicNPCAI::CanTakeCover(bool bCanTakeCover)
{
	if(bCanTakeCover)
	{
		this->GetCharacterMovement()->Crouch();
		bCanCrouch = true;
	}
	else
	{
		this->GetCharacterMovement()->UnCrouch();
		bCanCrouch = false;
	}
}

void ABasicNPCAI::DeathRituals(bool bIsExplosionDeath)
{
	Super::DeathRituals(bIsExplosionDeath);
	if (ActiveCover)
	{
		ActiveCover->bIsAcquired = false;
	}

}

void ABasicNPCAI::ReleaseCover()
{
	if (ActiveCover)
	{
		ActiveCover->bIsAcquired = false;
		ActiveCover = nullptr;
	}
}