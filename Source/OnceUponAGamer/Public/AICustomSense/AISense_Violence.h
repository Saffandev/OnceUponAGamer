// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISense.h"
#include "AISense_Violence.generated.h"


class UAISenseConfig_Violence;
class UAISenseEvent;

USTRUCT(BlueprintType)
struct ONCEUPONAGAMER_API FAIViolenceEvent
{
	GENERATED_USTRUCT_BODY()
	
	typedef class UAISense_Violence FSenseClass;

	float Age;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
	FVector ViolenceLocation;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Sense")
	float ViolenceRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
	AActor* Instigator;

	FAIViolenceEvent();
	FAIViolenceEvent(AActor* InInstigator, FVector& InViolenceLocation, float ViolenceRange = 1.f);

	void Compile();
};

UCLASS()
class ONCEUPONAGAMER_API UAISense_Violence : public UAISense
{
	GENERATED_UCLASS_BODY()
	
	public:
	
	void RegisterEvent(const FAIViolenceEvent& Event);
	// void RegisterEventsBatch(const TArray<FAIViolenceEvent>& Events);

	// virtual void PostInitProperties() override;

	UFUNCTION(BlueprintCallable, Category = "AI|Perception",meta = (WorldContext = "WorldContextObject"))
	static void ReportViolenceEvent(UObject* WorldContextObject, FVector ViolenceLocation, AActor* Instigator = nullptr,float ViolenceRange = 1.f);

protected:
	TArray<FAIViolenceEvent> ViolenceEvent;
	float ViolenceRange;
	TMap<FPerceptionListenerID,float> ListenerPorps;

protected:
	virtual float Update() override;
	void OnNewLisntenerImpl(const FPerceptionListener& NewListner);
	void OnListenerUpdated(const FPerceptionListener& UpdatedListner);
	void OnListenerRemovedImpl(const FPerceptionListener& UpdatedListner);
	
};
