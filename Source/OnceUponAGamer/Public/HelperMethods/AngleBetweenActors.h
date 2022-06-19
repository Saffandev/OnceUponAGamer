// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

class ONCEUPONAGAMER_API AngleBetweenActors
{
public:
	static float AngleBetween(AActor* From, AActor* To, bool bCanDrawDebug = false) ;
};
