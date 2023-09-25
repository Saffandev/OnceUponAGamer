// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Task/BTT_SetFocusLocation.h"
#include "GameFramework/Actor.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTT_SetFocusLocation::UBTT_SetFocusLocation()
{
    NodeName = TEXT("SetFocus");
    BB_FocusPoint.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_SetFocusLocation, BB_FocusPoint), AActor::StaticClass());
    BB_FocusPoint.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_SetFocusLocation, BB_FocusPoint));

}

EBTNodeResult::Type UBTT_SetFocusLocation::ExecuteTask(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory)
{
    if(OwnerComp.GetAIOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("Has Owner"));
        if(bCanFocus && (OwnerComp.GetAIOwner()->GetBlackboardComponent()))
        {
            UE_LOG(LogTemp, Warning, TEXT("can Focus"));
            if (BB_FocusPoint.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
            {
                UE_LOG(LogTemp, Warning, TEXT("loc Focusing"));
                OwnerComp.GetAIOwner()->SetFocalPoint(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsVector(BB_FocusPoint.SelectedKeyName));
            }
            else
            {
                    UE_LOG(LogTemp, Warning, TEXT("object Focusing"));
                UObject* FocusObject = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(BB_FocusPoint.SelectedKeyName);
                AActor* FocusActor = Cast<AActor>(FocusObject);
                if (IsValid(FocusActor))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Focusing"));
                    OwnerComp.GetAIOwner()->SetFocus(FocusActor);
                    DrawDebugSphere(GetWorld(), FocusActor->GetActorLocation() + FVector(0,0,100.f), 20, 12, FColor::Blue, false, 4.f);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("No Focusing"));
                }
            }
        }
        else
        {
            OwnerComp.GetAIOwner()->ClearFocus(EAIFocusPriority::Gameplay);
        }
    }
    return EBTNodeResult::Succeeded;
}