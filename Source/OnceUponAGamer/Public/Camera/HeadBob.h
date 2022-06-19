// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "HeadBob.generated.h"

/**
 * 
 */
UCLASS()
class ONCEUPONAGAMER_API UHeadBob : public UCameraModifier
{
	GENERATED_BODY()

public:
	UHeadBob();
	virtual bool ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV);

private:
	void HeadBobSpring(float DeltaTime);

private:
	class APlayerCharacter* PlayerCharacter;
	FVector HeadbobLocation;
	FRotator HeadbobRotation;
	UPROPERTY(EditAnywhere)
	UCurveVector* HeadbobCurvePos;
	UPROPERTY(EditAnywhere)
	UCurveVector* HeadbobCurveRot;

	FVector CurrentVelocity;
    FVector PreVelocity;
    FVector VelocityChange;
	
	float SpringVelocity = 0.f;
    float SpringPosition = 0.f;
    float SpringElasticity = 0.8f;
    float SpringDamping = 0.65f;
	float SpringPosZ;
    float SpringPitch;
    float LandingMovement = 2.f;
    float LandingTilt = 30.f;
	float GroundVelocityLength;
	float StrideLengthFactor = 0.5f;
	float StrideLength;
	float HeadBobCycle = 0.f;
	float HeadBobCycleValue;
	float HeadbobFade = 0.f;
    float HeadbobFadeSpeed = 4.f;
	float TargetHeadbobFade ;
};
