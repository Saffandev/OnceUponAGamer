// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Enum/EnumViolenceType.h"
#include "ViolenceRegistration.generated.h"

class AActionAgainstViolence;
/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UViolenceRegistration : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	// static TSubclassOf<AActionAgainstViolence> ViolenceHandler;
	UFUNCTION(BlueprintCallable, Category = "Violence")
	static void SetViolenceDetector(class AActionAgainstViolence* ActionAgainstViolenceBP);
	UFUNCTION(BlueprintCallable, Category = "Violence")
	static void RegisterViolence(UObject* WorldContextObject,FVector ViolenceLocation, AActor* Instigator, EViolenceType ViolenceType);
};

