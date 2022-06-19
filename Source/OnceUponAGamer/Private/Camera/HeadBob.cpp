// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/HeadBob.h"
#include "Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveVector.h"

UHeadBob::UHeadBob()
{
    AlphaInTime = 0.2f;
    AlphaOutTime = 0.2f;
}

bool UHeadBob::ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV)
{
    if(PlayerCharacter == nullptr)
    {
        PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
    }
    
    HeadBobSpring(DeltaTime);

    FVector ViewLocation = InOutPOV.Location;
    FRotator ViewRotation = InOutPOV.Rotation;

    //FinalViewLocation
    FVector NewViewLocation = InOutPOV.Location + UKismetMathLibrary::TransformLocation(UKismetMathLibrary::MakeTransform(FVector::ZeroVector,ViewRotation,FVector::OneVector),
                                                                    HeadbobLocation);
    UE_LOG(LogTemp,Error,TEXT("%s"),*InOutPOV.Rotation.ToString());
    // InOutPOV.Location = FMath::Lerp(InOutPOV.Location,NewViewLocation,GetTargetAlpha());//FinalViewLocation
    InOutPOV.Location = NewViewLocation;
    //FinalViewRotation
    FRotator NewViewRotation = ViewRotation + NewViewRotation;

    // InOutPOV.Rotation = FMath::Lerp(InOutPOV.Rotation,NewViewRotation,GetTargetAlpha());//FinalViewRotation
    InOutPOV.Rotation = NewViewRotation;
    UE_LOG(LogTemp,Warning,TEXT("%s"),*InOutPOV.Rotation.ToString());
    return false;

}   

void UHeadBob::HeadBobSpring(float DeltaTime)
{
    // UE_LOG(LogTemp,Warning,TEXT("Camera Headbob"));

    //Velocity Update
   
    
    CurrentVelocity = PlayerCharacter->GetCharacterMovement()->GetLastUpdateVelocity() / 100;//dividing by 100 to convert m/s to cm/s
    VelocityChange = CurrentVelocity - PreVelocity;
    PreVelocity = CurrentVelocity; 

    //Spring Rotation and position for jumping
    SpringVelocity = ((SpringVelocity - VelocityChange.Z) - (SpringPosition * SpringElasticity)) * SpringDamping;
    SpringPosition = FMath::Clamp((SpringPosition + SpringVelocity * DeltaTime),-0.32f,0.32f);

    //Set Spring position and pitch
    SpringPosZ = SpringPosition * LandingMovement;
    // SpringPitch = SpringPosition * LandingTilt * (PlayerCharacter->IsADSButtonDown() ? 0 : 1);
    SpringPitch = SpringPosition * LandingTilt;

    //Setting Ground Velocity
    // bool GroundVelocityCondition = (PlayerCharacter->GetCharacterMovement()->IsMovingOnGround() || PlayerCharacter->GetWallRunning()) 
    //                                 && PlayerCharacter->IsADSButtonDown() 
    //                                 && PlayerCharacter->bIsReloading;
    bool GroundVelocityCondition = PlayerCharacter->GetCharacterMovement()->IsMovingOnGround();
    GroundVelocityLength = GroundVelocityCondition ? CurrentVelocity.Size() : 0 ;

    //Stride Length
    StrideLength =  GroundVelocityLength * StrideLengthFactor + 1;

    //Headbob Cycle
    
    HeadBobCycleValue = (((GroundVelocityLength / StrideLength) * (DeltaTime / 2.f)) + HeadBobCycle);
    HeadBobCycle = HeadBobCycleValue >= 2.f ? 0 : HeadBobCycleValue;//used to loop the heaedbob, remove to check the effect

    //Fading Headbob
    TargetHeadbobFade = FMath::Clamp((GroundVelocityLength * 140) / PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed, 0.f, 1.f);
    HeadbobFade = UKismetMathLibrary::FInterpTo(HeadbobFade,TargetHeadbobFade,DeltaTime,HeadbobFadeSpeed);
    
    //Headbob Animation
    FVector HeadbobPos = HeadbobCurvePos->GetVectorValue(HeadBobCycle) * HeadbobFade;
    FVector HeadbobRot = HeadbobCurveRot->GetVectorValue(HeadBobCycle) * HeadbobFade;

    //FinalHeadbob Location
    HeadbobLocation = FVector(HeadbobPos.X,HeadbobPos.Y,HeadbobPos.Z + SpringPosZ);
    UE_LOG(LogTemp,Display,TEXT("%s "),*HeadbobLocation.ToString());

    //FinalHeadbob Rotation
    HeadbobRotation = FRotator(HeadbobRot.Y + SpringPitch,HeadbobRot.Z,HeadbobRot.X);
}