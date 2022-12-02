// Fill out your copyright notice in the Description page of Project Settings.


#include "AICustomSense/AISenseConfig_Violence.h"


UAISenseConfig_Violence::UAISenseConfig_Violence(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{
	DebugColor = FColor::Blue;
    // Implementation = GetSenseImplementation();
}

TSubclassOf<UAISense> UAISenseConfig_Violence::GetSenseImplementation() const
{
    return Implementation;
}