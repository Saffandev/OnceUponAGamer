// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Heavy/HeavyAIController.h"
#include "AI/NPC/Heavy/HeavyAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/PatrolPoint.h"


AHeavyAIController::AHeavyAIController()
{

}

void AHeavyAIController::BeginPlay()
{
    Super::BeginPlay();
    OwnerAI = Cast<AHeavyAI>(GetPawn());
    if(Blackboard && OwnerAI)
    {
        Blackboard->SetValueAsObject(FName("PatrolObject"),OwnerAI->PatrolPointObj);
       
    }
}