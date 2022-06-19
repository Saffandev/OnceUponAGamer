// Fill out your copyright notice in the Description page of Project Settings.


#include "HelperMethods/AngleBetweenActors.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
// #include "Engine/World.h"

float AngleBetweenActors::AngleBetween(AActor* From, AActor* To,bool bCanDrawDebug) 
{
    FVector Direction = To->GetActorLocation() - From->GetActorLocation();
    UKismetMathLibrary::Vector_Normalize(Direction);
    float Angle = UKismetMathLibrary::DegAcos( UKismetMathLibrary::Dot_VectorVector(From->GetActorForwardVector(),Direction));
    if(bCanDrawDebug)
    {
        // UWorld* World;
        // DrawDebugLine(World->GetWorld(),From->GetActorLocation(),To->GetActorLocation(),FColor::Red,false,0.1);
        // DrawDebugLine(World->GetWorld(),From->GetActorLocation(),From->GetActorForwardVector() * 200 +From->GetActorLocation() ,FColor::Red,false,0.1);

    }
    return Angle;
}