// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_Behind.generated.h"

struct FEnvQueryContextData;
struct FEnvQueryInstance;

UCLASS()
class ONCEUPONAGAMER_API UEnvQueryContext_Behind : public UEnvQueryContext
{
	GENERATED_BODY()
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

};
