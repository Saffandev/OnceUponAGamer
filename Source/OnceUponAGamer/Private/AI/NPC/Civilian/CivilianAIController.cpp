// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Civilian/CivilianAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "AICustomSense/AISenseConfig_Violence.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


// ACivilianAIController::ACivilianAIController(const FObjectInitializer& ObjectInitializer)
//     :Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("CrowdFollowingComponent")))
ACivilianAIController::ACivilianAIController()
    {
        AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
        ViolenceSenseConfig = CreateDefaultSubobject<UAISenseConfig_Violence>(TEXT("ViolenceSenseConfig"));
        ViolenceSenseConfig->VoilenceDetectionRange = ViolenceDetectionRange;
        ViolenceSenseConfig->DetectionByAffiliation.bDetectEnemies = true;
        ViolenceSenseConfig->DetectionByAffiliation.bDetectFriendlies = true;
        ViolenceSenseConfig->DetectionByAffiliation.bDetectNeutrals = true;
        ViolenceSenseConfig->SetMaxAge(0.f);

        AIPerceptionComponent->ConfigureSense(*ViolenceSenseConfig);
        AIPerceptionComponent->SetDominantSense(*ViolenceSenseConfig->GetSenseImplementation());
        AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this,&ACivilianAIController::OnTargetPerceptionUpdated);
    }


void ACivilianAIController::BeginPlay()
{
    Super::BeginPlay();
    if(GetPawn())
    {
        ControlledPawn = Cast<ACharacter>(GetPawn());
    }
    Roaming();
    RoamingTimer();
}
void ACivilianAIController::Roaming()
{
    if(ControlledPawn == nullptr)
    {
        UE_LOG(LogTemp,Warning,TEXT("No Pawn"));
        return;
    }
    FVector MoveToLocation = UNavigationSystemV1::GetCurrent(GetWorld())->GetRandomReachablePointInRadius(this,ControlledPawn->GetActorLocation(),1000);
    // UE_LOG(LogTemp,Error,TEXT("%s"),*MoveToLocation.ToString());
    this->MoveToLocation(MoveToLocation);
    
}

void ACivilianAIController::RoamingTimer()
{
    if(ControlledPawn)
    {
        ControlledPawn->GetCharacterMovement()->MaxWalkSpeed = 180;
    }
    GetWorld()->GetTimerManager().SetTimer(MoveTimerHandle,this,&ACivilianAIController::Roaming,3,true);
}

void ACivilianAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus AIStimulus)
{
    // UE_LOG(LogTemp,Warning,TEXT("Violence Sesned"));
    GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
    if(ControlledPawn)
    {
        ControlledPawn->GetCharacterMovement()->MaxWalkSpeed = 500.f;
    }
    FVector RunAwayLocation;
    bool bIsValidLocation = SafeLocationCalc(AIStimulus.StimulusLocation,RunAwayLocation);
    this->MoveToLocation(RunAwayLocation);
    // DrawDebugSphere(GetWorld(),RunAwayLocation,100,12,FColor::Red,false,10);

    GetWorld()->GetTimerManager().SetTimer(ViolenceClearedTimerHandle,this,&ACivilianAIController::RoamingTimer,7.f, false);
}

bool ACivilianAIController::SafeLocationCalc(FVector StimilusLocation, FVector& MoveLocation)
{
    if(ControlledPawn)
    {
        // FVector PlayerLocation = UGameplayStatics::GetPlayerPawn(this,0)->GetActorLocation();
        FVector RunAwayDirection = ControlledPawn->GetActorLocation() - StimilusLocation;
        RunAwayDirection.Normalize();
        FVector Origin = (RunAwayDirection * SafeDistance) + StimilusLocation;
        FNavLocation NavLocation;
        bool bCanMove = UNavigationSystemV1::GetCurrent(GetWorld())->GetRandomPointInNavigableRadius(Origin,100,NavLocation);
        MoveLocation = NavLocation.Location;
        return bCanMove;

    }
    return false;
}