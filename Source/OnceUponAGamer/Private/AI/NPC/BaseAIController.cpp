// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/BaseAIController.h"
#include "AI/EncounterSpace.h"
#include "AI/NPC/BaseAI.h"
#include "AI/PatrolPoint.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "HelperMethods/AngleBetweenActors.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Player/PlayerCharacter.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"

ABaseAIController::ABaseAIController()
{
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    SightSense = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightSense"));
    SightSense->SightRadius = 3000.f;
    SightSense->LoseSightRadius = 5000.f;
    SightSense->AutoSuccessRangeFromLastSeenLocation = 4000.f;
    SightSense->PeripheralVisionAngleDegrees = 85.f;
    SightSense->DetectionByAffiliation.bDetectEnemies = true;
    SightSense->DetectionByAffiliation.bDetectFriendlies = true;
    SightSense->DetectionByAffiliation.bDetectNeutrals = true;
    SightSense->SetMaxAge(0.f);

    DamageSense = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageSense"));

    HearingSense = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingSense"));
    HearingSense->HearingRange = 3000;
    HearingSense->bUseLoSHearing = false;
    HearingSense->DetectionByAffiliation.bDetectEnemies = true;
    HearingSense->DetectionByAffiliation.bDetectFriendlies = true;
    HearingSense->DetectionByAffiliation.bDetectNeutrals = true;

    AIPerceptionComponent->ConfigureSense(*SightSense);
    AIPerceptionComponent->ConfigureSense(*DamageSense);
    AIPerceptionComponent->ConfigureSense(*HearingSense);
    AIPerceptionComponent->SetDominantSense(*SightSense->GetSenseImplementation());
    AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ABaseAIController::OnPerceptionUpdated);
}

void ABaseAIController::BeginPlay()
{
    Super::BeginPlay();
    FTimerHandle OwnerAIHandle;
    GetWorld()->GetTimerManager().SetTimer(OwnerAIHandle, this, &ABaseAIController::InitOwner, 0.2, false);
    PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    bIsOwnerAlive = true;
}

void ABaseAIController::InitOwner()
{
    OwnerAI = Cast<ABaseAI>(GetPawn());
    UE_LOG(LogTemp, Warning, TEXT("OwnerAI %s"), *OwnerAI->GetName());
    OwnerAI->MyEncounterSpace = MyEncounterSpace;
    if (OwnerAI && OwnerAI->bCanAutoActivate)
    {
        Activate();
    }
    if (Blackboard && OwnerAI)
    {
        Blackboard->SetValueAsObject(FName("PatrolObject"), OwnerAI->PatrolPointObj);
    }
}

void ABaseAIController::Activate()
{
    if (OwnerAI)
    {
        if (BehaviorTree)
        {
            RunBehaviorTree(BehaviorTree);
            Blackboard = GetBlackboardComponent();
            Blackboard->SetValueAsInt(FName("PatrolDirection"), 1);
            Blackboard->SetValueAsInt(FName("PatrolPointIndex"), 0);
            Blackboard->SetValueAsBool(FName("bIsPlayerInChildSpace"), false);
            Blackboard->SetValueAsBool(FName("bIsPlayerInParentSpace"), false);
            UE_LOG(LogTemp, Warning, TEXT("Activated"));
            FTimerHandle VisibilityTimerHandle;
            GetWorld()->GetTimerManager().SetTimer(VisibilityTimerHandle, this, &ABaseAIController::CheckPlayerVisibility, 0.1, true);
            if (!OwnerAI->bCanAutoActivate)
            {
                Blackboard->SetValueAsBool(FName("bCanSeePlayer"), true);
                Blackboard->SetValueAsVector(FName("PlayerLocation"), PlayerCharacter->GetActorLocation());
                Blackboard->SetValueAsVector(FName("PlayerLastKnownLocation"), PlayerCharacter->GetActorLocation());
            }
        }
    }
}

ABaseAI* ABaseAIController::GetControlledPawn()
{
    if (OwnerAI)
    {
        return OwnerAI;
    }
	return nullptr;
}

void ABaseAIController::OnPerceptionUpdated(TArray<AActor*> const& SensedActors)
{
    if (!OwnerAI)
    {
        return;
    }
    if (OwnerAI->bIsDead)
    {
        return;
    }
    if (AIPerceptionComponent && Blackboard)
    {
        // UE_LOG(LogTemp,Warning,TEXT("%i"),SensedActors.Num());
        for (AActor* SensedActor : SensedActors)
        {
            if (SensedActor)
            {
                FActorPerceptionBlueprintInfo Info;
                AIPerceptionComponent->GetActorsPerception(SensedActor, Info);
                if (Info.Target != nullptr)
                {
                    AActor* EnemyTarget = Info.Target;
                    if (PlayerCharacter->GetClass() == EnemyTarget->GetClass())
                    {
                        uint32 StimulusIndex = 0;
                        if (/*!Blackboard->GetValueAsBool(FName("bCanSeePlayer"))*/true)
                        {
                            for (FAIStimulus AIStimulus : Info.LastSensedStimuli)
                            {

                                TSubclassOf<UAISense> SensedClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, AIStimulus);
                                if (SensedClass)
                                {
                                    //sight sense
                                    if (!SightSense)
                                    {
                                        // UE_LOG(LogTemp,Error,TEXT("No Sight Sense"));
                                        return;
                                    }
                                    if (UKismetMathLibrary::EqualEqual_ObjectObject(SensedClass, UAISense_Sight::StaticClass()))
                                    {
                                        Blackboard->SetValueAsBool(FName("bCanSeePlayer"), true);
                                        Blackboard->SetValueAsBool(FName("bIsPlayerVisible"), true);
                                        Blackboard->SetValueAsVector(FName("PlayerLocation"), EnemyTarget->GetActorLocation());
                                        Blackboard->SetValueAsVector(FName("PlayerLastKnownLocation"), EnemyTarget->GetActorLocation());
                                        if (MyEncounterSpace)
                                        {
                                            if (!MyEncounterSpace->bPlayerSpotted)
                                            {
                                                MyEncounterSpace->ICanSeePlayer();
                                            }
                                        }
                                    }
                                    //damage sense
                                    else if (UKismetMathLibrary::EqualEqual_ObjectObject(SensedClass, UAISense_Damage::StaticClass()) ||
                                        UKismetMathLibrary::EqualEqual_ObjectObject(SensedClass, UAISense_Hearing::StaticClass()))
                                    {
                                        UE_LOG(LogTemp, Warning, TEXT("Hearing"));
                                        SetFocalPoint(AIStimulus.StimulusLocation);
                                        if (!Blackboard->GetValueAsBool(FName("bCanSeePlayer")) && MyEncounterSpace != nullptr && MyEncounterSpace->IsOverlappingActor(PlayerCharacter))
                                        {

                                            MyEncounterSpace->AssingInvestigation(AIStimulus.StimulusLocation);
                                            Blackboard->SetValueAsVector(FName("PlayerLastKnownLocation"), AIStimulus.StimulusLocation);
                                        }
                                        SetFocalPoint(AIStimulus.StimulusLocation);

                                    }

                                    else
                                    {
                                        // UE_LOG(LogTemp,Warning,TEXT("No sense sensed"));
                                    }
                                }


                                StimulusIndex++;
                            }
                        }
                    }
                }
            }
        }
    }
}

void ABaseAIController::CheckPlayerVisibility()
{
    if (!PlayerCharacter || !OwnerAI)
    {
        return;
    }
    if (Blackboard->GetValueAsBool(FName("bCanSeePlayer")))
    {
        float Angle = AngleBetweenActors::AngleBetween(OwnerAI, PlayerCharacter, true);
        bool bIsPlayerInLineOfSight = LineOfSightTo(PlayerCharacter);
        bool bIsPlayerVisible = Angle <= PlayerCheckAngle && Angle >= 0 && bIsPlayerInLineOfSight;

        Blackboard->SetValueAsBool(FName("bIsPlayerVisible"), bIsPlayerVisible);
        if (bIsPlayerVisible)
        {
            Blackboard->SetValueAsBool(FName("bCanInvestigate"), false);
            Blackboard->SetValueAsVector(FName("PlayerLastKnownLocation"), PlayerCharacter->GetActorLocation());
        }
    }
}

void ABaseAIController::ClearSenses()
{
    PerceptionComponent->ForgetActor(PlayerCharacter);
    Blackboard->SetValueAsBool(FName("bCanSeePlayer"), false);
    Blackboard->SetValueAsBool(FName("bCanInvestigate"), false);
    Blackboard->SetValueAsBool(FName("bIsPlayerInChildSpace"), false);
    Blackboard->SetValueAsBool(FName("bIsPlayerInParentSpace"), false);
}