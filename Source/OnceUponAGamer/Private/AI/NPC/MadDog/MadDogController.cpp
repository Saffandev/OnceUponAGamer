// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/MadDog/MadDogController.h"
#include "AI/NPC/MadDog/MadDogNPCAI.h"
#include "BehaviorTree/BlackboardComponent.h"

AMadDogController::AMadDogController()
{

}

void AMadDogController::BeginPlay()
{
    Super::BeginPlay();
    OwnerPawn = Cast<AMadDogNPCAI>(GetPawn());
}

void AMadDogController::Activate()
{
    RunBehaviorTree(BehaviorTree);
    GetBlackboardComponent()->SetValueAsBool(FName("bIsHoldingHand"),true);
}