// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/BasicNPCAIController.h"
#include "AI/EncounterSpace.h"
#include "AI/NPC/Basic/BasicNPCAI.h"
#include "AI/Cover.h"
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


ABasicNPCAIController::ABasicNPCAIController()
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
    AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this,&ABasicNPCAIController::OnPerceptionUpdated);
}

void ABasicNPCAIController::BeginPlay()
{
    Super::BeginPlay();
   
    FTimerHandle OwnerAIHandle;
    GetWorld()->GetTimerManager().SetTimer(OwnerAIHandle,this,&ABasicNPCAIController::InitOwner,0.2,false);
    if(!OwnerAI)
        UE_LOG(LogTemp,Error,TEXT("No owner Ai "));
    PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
    bIsOwnerAlive = true;

    // if(Blackboard && OwnerAI)
    // {
    //     Blackboard->SetValueAsObject(FName("PatrolObject"),OwnerAI->PatrolPointObj);
    // }
}

void ABasicNPCAIController::InitOwner()
{
    UE_LOG(LogTemp,Warning,TEXT("Init Owner-------"));
    OwnerAI = Cast<ABasicNPCAI>(GetPawn());
    OwnerAI->MyEncounterSpace = MyEncounterSpace;
    if(OwnerAI && OwnerAI->bCanAutoActivate)
    {
        Activate();
    }
}
void ABasicNPCAIController::Activate()
{
    UE_LOG(LogTemp,Warning,TEXT("Activated"));
    if(OwnerAI)
    {
        if(BehaviorTree)
        {
            RunBehaviorTree(BehaviorTree);
            Blackboard = GetBlackboardComponent();
            Blackboard->SetValueAsInt(FName("PatrolDirection"),1);
            FTimerHandle VisibilityTimerHandle;
            GetWorld()->GetTimerManager().SetTimer(VisibilityTimerHandle,this,&ABasicNPCAIController::CheckPlayerVisibility,0.5,true);
            if(!OwnerAI->bCanAutoActivate)
            {
                Blackboard->SetValueAsBool(FName("bCanSeePlayer"),true);
                Blackboard->SetValueAsVector(FName("PlayerLocation"),PlayerCharacter->GetActorLocation());
                Blackboard->SetValueAsVector(FName("PlayerLastKnownLocation"),PlayerCharacter->GetActorLocation()); 
            }
        }
        
        else
        {
            UE_LOG(LogTemp,Warning,TEXT("No BehaviorTree"));
        }
    
    }
    else
    {
        UE_LOG(LogTemp,Warning,TEXT("No Owner"));
    }
   

    
}

ABasicNPCAI* ABasicNPCAIController::GetControlledPawn()
{
    if(OwnerAI)
    {
        return OwnerAI;
    }
    return nullptr;
}
void ABasicNPCAIController::OnPerceptionUpdated(TArray<AActor*>const& SensedActors)
{
    if(!OwnerAI)
    {
        return;
    }
    if(OwnerAI->bIsDead)
    {
        return;
    }
    if(AIPerceptionComponent && Blackboard)
    {
        UE_LOG(LogTemp,Warning,TEXT("%i"),SensedActors.Num());
        for(AActor* SensedActor:SensedActors)
        {
        if(SensedActor)
        {
            FActorPerceptionBlueprintInfo Info;
            AIPerceptionComponent->GetActorsPerception(SensedActor,Info);
            if(Info.Target != nullptr)
            {
                AActor* EnemyTarget = Info.Target;
                if(PlayerCharacter->GetClass() == EnemyTarget->GetClass())
                {
                    uint32 StimulusIndex = 0;
                    if(!Blackboard->GetValueAsBool(FName("bCanSeePlayer")))
                    {
                    for( FAIStimulus AIStimulus:Info.LastSensedStimuli )
                    {   
                        
                        TSubclassOf<UAISense> SensedClass = UAIPerceptionSystem::GetSenseClassForStimulus(this,AIStimulus);
                            if(SensedClass)
                            {
                                //sight sense
                                if(!SightSense)
                                {
                                    UE_LOG(LogTemp,Error,TEXT("No Sight Sense"));
                                    return;
                                }
                                if(UKismetMathLibrary::EqualEqual_ObjectObject(SensedClass,UAISense_Sight::StaticClass()))
                                {
                                    Blackboard->SetValueAsBool(FName("bCanSeePlayer"),true);
                                    UE_LOG(LogTemp,Warning,TEXT("PlayerSpotted"));
                                    Blackboard->SetValueAsVector(FName("PlayerLocation"),EnemyTarget->GetActorLocation());
                                    Blackboard->SetValueAsVector(FName("PlayerLastKnownLocation"),EnemyTarget->GetActorLocation());   
                                }
                                //damage sense
                                else if(UKismetMathLibrary::EqualEqual_ObjectObject(SensedClass,DamageSense->GetSenseImplementation()) ||
                                UKismetMathLibrary::EqualEqual_ObjectObject(SensedClass,HearingSense->GetSenseImplementation()))
                                {
                                    SetFocalPoint(AIStimulus.StimulusLocation);
                                    if(!Blackboard->GetValueAsBool(FName("bCanSeePlayer")) && MyEncounterSpace != nullptr && MyEncounterSpace->IsOverlappingActor(PlayerCharacter))
                                    {

                                        MyEncounterSpace->AssingInvestigation(AIStimulus.StimulusLocation);
                                        Blackboard->SetValueAsVector(FName("PlayerLastKnownLocation"),AIStimulus.StimulusLocation);
                                    }
                                }

                                else
                                {
                                    UE_LOG(LogTemp,Warning,TEXT("No sense sensed"));
                                }
                            }
                            

                        StimulusIndex ++;
                    }
                    }
                }
            }
        }
    }
    }
}

void ABasicNPCAIController::CoverRequest()
{
    if(MyEncounterSpace && OwnerAI)
    {
        TArray<AActor*> CoverActor = MyEncounterSpace->OverlappedCovers;
        float CoverDistance = 1000000;
        ACover* MainCover = nullptr;
        FVector CoverPoints;
        FNavLocation NavLocation;
        for(auto TempCoverActor : CoverActor)
        {
            if(!TempCoverActor)
            {
                continue;
            }
            ACover* TempCover = Cast<ACover>(TempCoverActor);
            if(TempCover == nullptr)
            {
                UE_LOG(LogTemp,Warning,TEXT("No Temp Cover"));
                continue;
            }
            else if(TempCover->bIsAcquired)
            {   
                continue;
            }
            
            else if(OwnerAI->GetDistanceTo(TempCover) < CoverDistance)
            {
                CoverPoints = TempCover->GiveCoverPoints();
                if(!UNavigationSystemV1::GetCurrent(GetWorld())->ProjectPointToNavigation(CoverPoints,NavLocation))
                {
                    continue;
                }
                CoverDistance = OwnerAI->GetDistanceTo(TempCover);
                MainCover = TempCover;

            }
        }
        CoverDistance = 1000000;
        if(MainCover == nullptr)
        {
            Blackboard->SetValueAsVector(FName("CoverLocation"),FVector::ZeroVector);
            Blackboard->SetValueAsBool(FName("bIsStandingCover"),false);
            Blackboard->SetValueAsBool(FName("bIsCoverAvailable"),false);
            Blackboard->SetValueAsObject(FName("CoverObj"),nullptr);
            // OwnerAI->ActiveCover = nullptr;
        }

        else
        {
            MainCover->bIsAcquired = true;
            Blackboard->SetValueAsVector(FName("CoverLocation"),NavLocation.Location);
            Blackboard->SetValueAsVector(FName("PeakCoverLocation"),MainCover->GivePeakPoints(OwnerAI));
            Blackboard->SetValueAsBool(FName("bIsStandingCover"),MainCover->bIsPeekCover);
            Blackboard->SetValueAsBool(FName("bIsCoverAvailable"),true);
            Blackboard->SetValueAsObject(FName("CoverObj"),MainCover);
            OwnerAI->ActiveCover = MainCover;
            if(!CoverTimerHandle.IsValid())
            {
                GetWorld()->GetTimerManager().SetTimer(CoverTimerHandle,this,&ABasicNPCAIController::CoverRequestEntry,15.f,true);
            }
        }

    }
    else
    {
        if(!OwnerAI)
        {
            UE_LOG(LogTemp,Warning,TEXT("No Owner"));
        }
        UE_LOG(LogTemp,Error,TEXT("No Encounter space"));

    }
    return;
}
void ABasicNPCAIController::CoverRequestEntry()
{
    Blackboard->SetValueAsBool(FName("bCanDoAnotherCoverRequest"),true);
}
void ABasicNPCAIController::CheckPlayerVisibility()
{
    if(!PlayerCharacter || !OwnerAI)
    {
        return ;
    }
    if(Blackboard->GetValueAsBool(FName("bCanSeePlayer")))
    {
        float Angle = AngleBetweenActors::AngleBetween(OwnerAI,PlayerCharacter,true);
        bool bIsPlayerInLineOfSight = LineOfSightTo(PlayerCharacter);
        bool bIsPlayerVisible = Angle <= 100 && Angle >= 0 && bIsPlayerInLineOfSight;

        // DrawDebugLine(GetWorld(),OwnerAI->GetActorLocation(),PlayerCharacter->GetActorLocation(),FColor::Red,false,0.1);
        // DrawDebugLine(GetWorld(),OwnerAI->GetActorLocation(),OwnerAI->GetActorForwardVector() * 200 + OwnerAI->GetActorLocation() ,FColor::Green,false,0.1);
        // UE_LOG(LogTemp,Warning,TEXT("%f   %i"),Angle,bIsPlayerInLineOfSight);
        Blackboard->SetValueAsBool(FName("bIsPlayerVisible"),bIsPlayerVisible);
        if(bIsPlayerVisible)
        {
            Blackboard->SetValueAsBool(FName("bCanInvestigate"),false);
            Blackboard->SetValueAsVector(FName("PlayerLastKnownLocation"),PlayerCharacter->GetActorLocation());
        }
        else
        {
            // look here and there for player, only rotation
        }
    }
}

void ABasicNPCAIController::ToggleSightSense()
{
    AIPerceptionComponent->SetSenseEnabled(SightSense->GetSenseImplementation(),false);
    AIPerceptionComponent->SetSenseEnabled(SightSense->GetSenseImplementation(),true);
}
//  void ABasicNPCAIController::Dead()
//  {
    
//     MyEncounterSpace->IAMDead();
//     UE_LOG(LogTemp,Warning,TEXT("Deadddddddddddddd"));
//  }
