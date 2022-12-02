// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Task/BTT_FindPatrolPoint.h"
#include "AI/PatrolPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBTT_FindPatrolPoint::UBTT_FindPatrolPoint()
{
    NodeName = TEXT("BT_FindPatrolPoint");
}

EBTNodeResult::Type UBTT_FindPatrolPoint::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8* NodeMemory) 
{
    Super::ExecuteTask(OwnerComp, NodeMemory);
    
    APatrolPoint* PatrolPointObj = Cast<APatrolPoint>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(PatrolPointObject.SelectedKeyName));
    
    if(PatrolPointObj)
    {
        uint32 Index = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsInt(PatrolPointIndex.SelectedKeyName);
        if(PatrolPointObj->PatrolPointLocation.Num() > 0)
        {
        FVector MoveToLocation = PatrolPointObj->PatrolPointLocation[Index];
       // UE_LOG(LogTemp,Warning,TEXT("Patrol Location %s"),*MoveToLocation.ToString());
        MoveToLocation = UKismetMathLibrary::TransformLocation(PatrolPointObj->GetActorTransform(),MoveToLocation);
        OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsVector(PatrolPointLocation.SelectedKeyName, MoveToLocation);
        }
    }
    // if(PatrolPointObj == nullptr)
    // {
    //     // UE_LOG(LogTemp,Warning,TEXT("No patrol point object in find patrol point"));
    // }
    return EBTNodeResult::Succeeded;
}
