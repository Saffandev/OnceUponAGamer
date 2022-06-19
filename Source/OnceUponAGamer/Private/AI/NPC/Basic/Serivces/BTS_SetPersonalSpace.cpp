// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/Serivces/BTS_SetPersonalSpace.h"
#include "Kismet/KismetMathLibrary.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HelperMethods/AngleBetweenActors.h"

UBTS_SetPersonalSpace::UBTS_SetPersonalSpace()
{
    NodeName = TEXT("PersonalSpaceCheck");
}

void UBTS_SetPersonalSpace::TickNode(UBehaviorTreeComponent &OwnerComp,uint8* NodeMemory,float DeltaSeconds)
{
    ABasicNPCAIController* OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());
    float Angle = AngleBetweenActors::AngleBetween(OwnerController->GetPawn(),UGameplayStatics::GetPlayerPawn(this,0));
    bool bIsPlayerInSight = OwnerController->LineOfSightTo(UGameplayStatics::GetPlayerPawn(this,0));
    bool bIsAngleInRange = UKismetMathLibrary::InRange_FloatFloat(Angle,0,PersonalAngle);
    if(bIsAngleInRange)
    {
        float Distance = OwnerController->GetPawn()->GetDistanceTo(UGameplayStatics::GetPlayerPawn(this,0));
        if(OwnerController->GetBlackboardComponent()->GetValueAsBool(BB_bIsinCover.SelectedKeyName) && Distance < crouchDistanceCheck)
        {
            OwnerController->GetBlackboardComponent()->SetValueAsBool(BB_bIsPlayerToClose.SelectedKeyName,true && bIsPlayerInSight);
        }
        else if(Distance < PersonalRange)
        {
            OwnerController->GetBlackboardComponent()->SetValueAsBool(BB_bIsPlayerToClose.SelectedKeyName,true && bIsPlayerInSight);
        }
        else if(Distance > ExitPersonalRange)
        {
            OwnerController->GetBlackboardComponent()->SetValueAsBool(BB_bIsPlayerToClose.SelectedKeyName,false);
        }

    }

}

