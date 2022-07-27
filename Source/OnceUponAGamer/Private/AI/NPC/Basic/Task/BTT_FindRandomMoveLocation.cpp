// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Task/BTT_FindRandomMoveLocation.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "Kismet/KismetMathLibrary.h"


UBTT_FindRandomMoveLocation::UBTT_FindRandomMoveLocation()
{
    NodeName = TEXT("RandomMoveLocation");
}

EBTNodeResult::Type UBTT_FindRandomMoveLocation::ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory)
{
    FVector RandomPoint;
    ABasicNPCAIController* OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());
    AAIController* AIController = OwnerComp.GetAIOwner();
    FVector Origin;
    if(bShouldFindRandomMovePointsBehind)
    {
        Origin = AIController->GetPawn()->GetActorLocation() + 
                (AIController->GetPawn()->GetActorForwardVector() * -1 * SearchOriginRangeBehind);
    }
    else
    {
        Origin = AIController->GetPawn()->GetActorLocation();
    }

    // UNavigationSystemV1::GetRandomPointInNavigableRadius(OwnerController,Origin,UKismetMathLibrary::RandomFloatInRange(SearchRadiusForPoints,SearchRadiusForPoints - 100));
    RandomPoint = UNavigationSystemV1::GetCurrent(GetWorld())->GetRandomPointInNavigableRadius(AIController,Origin,UKismetMathLibrary::RandomFloatInRange(SearchRadiusForPoints,SearchRadiusForPoints - RandomDeviation));
    if(RandomPoint == FVector::ZeroVector)
    {
        AIController->GetBlackboardComponent()->SetValueAsVector(BB_RandomMoveLocation.SelectedKeyName,AIController->GetPawn()->GetActorLocation());
        return EBTNodeResult::Succeeded;
    }
    AIController->GetBlackboardComponent()->SetValueAsVector(BB_RandomMoveLocation.SelectedKeyName,RandomPoint);
    return EBTNodeResult::Succeeded;
}