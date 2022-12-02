// Fill out your copyright notice in the Description page of Project Settings.


#include "HelperFunctions/ViolenceRegistration.h"
#include "WantedSystem/ActionAgainstViolence.h"
#include "Kismet/GameplayStatics.h"
#include "AICustomSense/AISense_Violence.h"

AActionAgainstViolence* ViolenceHandler;

void UViolenceRegistration::SetViolenceDetector(AActionAgainstViolence* ActionAgainstViolenceBP)
{
    if(ActionAgainstViolenceBP)
    {
        ViolenceHandler = ActionAgainstViolenceBP;
    }
     else
    {
        UE_LOG(LogTemp,Warning,TEXT("Invalid violence"));
    }
}

void UViolenceRegistration::RegisterViolence(UObject* WorldContextObject,FVector ViolenceLocation, AActor* Instigator, EViolenceType ViolenceType)
{
    UAISense_Violence::ReportViolenceEvent(WorldContextObject,ViolenceLocation,Instigator);
    if(ViolenceHandler)
    {
	    // UE_LOG(LogTemp,Warning,TEXT("Violence handeler called"));
        ViolenceHandler->RegisterViolenceType(ViolenceType);
    }
    else
    {
    //	UE_LOG(LogTemp,Warning,TEXT("No violence Handeler"));

    }
}   
