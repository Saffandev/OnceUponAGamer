// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISenseConfig.h"
#include "AISense_Violence.h"
#include "AISenseConfig_Violence.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "AI Violence Config"))
class ONCEUPONAGAMER_API UAISenseConfig_Violence : public UAISenseConfig
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", NoClear, config)
	TSubclassOf<UAISense_Violence> Implementation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "Sense")
	float VoilenceDetectionRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "Sense", config)
	FAISenseAffiliationFilter DetectionByAffiliation;

	virtual TSubclassOf<UAISense> GetSenseImplementation() const override;

};
