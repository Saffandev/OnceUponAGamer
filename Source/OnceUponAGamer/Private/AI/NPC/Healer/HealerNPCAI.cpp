// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Healer/HealerNPCAI.h"

// Sets default values
AHealerNPCAI::AHealerNPCAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHealerNPCAI::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHealerNPCAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHealerNPCAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

