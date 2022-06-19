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

    FVector Origin;
    if(bShouldFindRandomMovePointsBehind)
    {
        Origin = OwnerController->GetPawn()->GetActorLocation() + 
                (OwnerController->GetPawn()->GetActorForwardVector() * -1 * SearchOriginRangeBehind);
    }
    else
    {
        Origin = OwnerController->GetPawn()->GetActorLocation();
    }

    // UNavigationSystemV1::GetRandomPointInNavigableRadius(OwnerController,Origin,UKismetMathLibrary::RandomFloatInRange(SearchRadiusForPoints,SearchRadiusForPoints - 100));
    RandomPoint = UNavigationSystemV1::GetCurrent(GetWorld())->GetRandomPointInNavigableRadius(OwnerController,Origin,UKismetMathLibrary::RandomFloatInRange(SearchRadiusForPoints,SearchRadiusForPoints - RandomDeviation));
    if(RandomPoint == FVector::ZeroVector)
    {
        OwnerController->GetBlackboardComponent()->SetValueAsVector(BB_RandomMoveLocation.SelectedKeyName,OwnerController->GetPawn()->GetActorLocation());
        return EBTNodeResult::Succeeded;
    }
    OwnerController->GetBlackboardComponent()->SetValueAsVector(BB_RandomMoveLocation.SelectedKeyName,RandomPoint);
    return EBTNodeResult::Succeeded;
}