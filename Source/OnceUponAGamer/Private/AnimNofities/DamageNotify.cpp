// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNofities/DamageNotify.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

void UDamageNotify::NotifyTick(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,float FrameDeltaTime)
{
    if(bCanApplyDamage)
    {
        // UE_LOG(LogTemp,Warning,TEXT("Inside Notify Tick"));
        FVector TraceStart = MeshComp->GetSocketLocation(StartBoneName);
        FVector TraceEnd = MeshComp->GetSocketLocation(EndBoneName);
        FHitResult TraceHit;
        TArray<AActor*> ActorsToIgnore;
        if(MeshComp->GetOwner())
        {
            ActorsToIgnore.Add(MeshComp->GetOwner());
        }
        UKismetSystemLibrary::SphereTraceSingle(MeshComp->GetOwner(),
                                                TraceStart,
                                                TraceEnd,
                                                10,
                                                UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
                                                false,
                                                ActorsToIgnore,
                                                EDrawDebugTrace::None,
                                                TraceHit,
                                                true
                                                );
        AActor* HitActor = TraceHit.GetActor();
        if(HitActor)
        {   
            if(HitActor->CanBeDamaged() && HitActor == UGameplayStatics::GetPlayerPawn(MeshComp->GetOwner(),0))
            {
                
                // UE_LOG(LogTemp,Warning,TEXT("Player Hitted"))
                // UGameplayStatics::ApplyDamage(HitActor,
                //                             Damage,
                //                             MeshComp->GetOwner()->GetInstigatorController(),
                //                             MeshComp->GetOwner(),
                //                             UDamageType::StaticClass()
                //                             );
                UGameplayStatics::ApplyPointDamage(HitActor,
                                                   Damage,
                                                   TraceHit.TraceStart,
                                                   TraceHit,
                                                   MeshComp->GetOwner()->GetInstigatorController(),
                                                   MeshComp->GetOwner(),
                                                   UDamageType::StaticClass());
                bCanApplyDamage = false;
            }
        }
    }
}

void UDamageNotify::NotifyEnd(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation)
{
    bCanApplyDamage = true;
}