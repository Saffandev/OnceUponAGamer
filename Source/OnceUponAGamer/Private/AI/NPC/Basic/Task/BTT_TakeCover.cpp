#include "AI/NPC/Basic/Task/BTT_TakeCover.h"
#include "AI/NPC/Basic/BasicNPCAI.h"
#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "AI/Cover.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTT_TakeCover::UBTT_TakeCover()
{
    NodeName = TEXT("Take/OutCover");
}

EBTNodeResult::Type UBTT_TakeCover::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8* NodeMemory)
{
    ABasicNPCAIController* OwnerController = Cast<ABasicNPCAIController>(OwnerComp.GetOwner());
    if(OwnerController)
    {
        ABasicNPCAI* OwnerPawn = Cast<ABasicNPCAI>(OwnerController->GetPawn());
        if(OwnerPawn)
        {
            OwnerPawn->CanTakeCover(bCanTakeCover);
            OwnerController->GetBlackboardComponent()->SetValueAsBool(BB_IsInCover.SelectedKeyName,bCanTakeCover);
            ACover* aCurrentCover = Cast<ACover>(OwnerController->GetBlackboardComponent()->GetValueAsObject(BB_ActiveCoverObj.SelectedKeyName));
            if(aCurrentCover)
            {
            OwnerController->SetFocalPoint(aCurrentCover->GetActorLocation());
            DrawDebugSphere(GetWorld(),aCurrentCover->GetActorLocation(),20,20,FColor::Red,false,5);
            }
            if(bCanInvalidateCover)
            {
                ACover* CurrentCover = Cast<ACover>(OwnerController->GetBlackboardComponent()->GetValueAsObject(BB_ActiveCoverObj.SelectedKeyName));
                if(CurrentCover != nullptr)
                {
                    CurrentCover->bIsAcquired = false;
                }
                // UE_LOG(LogTemp,Error,TEXT("Inside the cover invalidation condition"));
                OwnerController->GetBlackboardComponent()->SetValueAsObject(BB_ActiveCoverObj.SelectedKeyName,nullptr);
                OwnerController->GetBlackboardComponent()->SetValueAsBool(BB_bIsCoverAvailable.SelectedKeyName,bIsThisReRequestInvalidation);
            }        
        }
    }

    return EBTNodeResult::Succeeded;
}