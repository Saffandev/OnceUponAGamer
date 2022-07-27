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
        UE_LOG(LogTemp,Warning,TEXT("Inside the if of heavy"));
        Blackboard->SetValueAsObject(FName("PatrolObject"),OwnerAI->PatrolPointObj);
        // if(OwnerAI->PatrolPointObj == nullptr)
        // {
        //     UE_LOG(LogTemp,Error,TEXT("No patrol obj with AI"));
        // }
    }
}