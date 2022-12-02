// Fill out your copyright notice in the Description page of Project Settings.


#include "AICustomSense/AISense_Violence.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AIPerceptionComponent.h"
#include "AICustomSense/AISenseConfig_Violence.h"


FAIViolenceEvent::FAIViolenceEvent()
                :Age(0.f), ViolenceLocation(FAISystem::InvalidLocation), ViolenceRange(1.f), Instigator(nullptr)
{

}

FAIViolenceEvent::FAIViolenceEvent(AActor* InInstigator, FVector& InViolenceLocation, float InViolenceRange)
                    :Age(0.f), Instigator(InInstigator),ViolenceLocation(InViolenceLocation),ViolenceRange(InViolenceRange)
{
    Compile();
}

void FAIViolenceEvent::Compile()
{
    if(FAISystem::IsValidLocation(ViolenceLocation) == false && Instigator != nullptr)
    {
        ViolenceLocation = Instigator->GetActorLocation();
    }
}

UAISense_Violence::UAISense_Violence(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
    // if(HasAnyFlags(RF_ClassDefaultObject) == false)
    // {
         UE_LOG(LogTemp,Warning,TEXT("Violence Constructor")); 
        OnNewListenerDelegate.BindUObject(this,&UAISense_Violence::OnNewLisntenerImpl);
        OnListenerUpdateDelegate.BindUObject(this,&UAISense_Violence::OnListenerUpdated);
        OnListenerRemovedDelegate.BindUObject(this,&UAISense_Violence::OnListenerRemovedImpl);
    // }
}


float UAISense_Violence::Update()
{
            UE_LOG(LogTemp,Warning,TEXT("SensedStarted")); 

    AIPerception::FListenerMap& ListenerMap = *GetListeners();
    
    for(AIPerception::FListenerMap::TIterator ListenerIT(ListenerMap); ListenerIT; ++ListenerIT)
    {
        FPerceptionListener& Listener = ListenerIT->Value;

        if(Listener.HasSense(GetSenseID()) == false)
        {
            continue;
        }

        for(const FAIViolenceEvent& Event : ViolenceEvent)
        {
            FVector OwnerLocation = Listener.GetBodyActor()->GetActorLocation();
            // float DetectionRange = ListenerPorps.Find(Listener.GetListenerID());
            float DetectionRange = ListenerPorps[Listener.GetListenerID()];
            FVector DetectionLocation = Event.ViolenceLocation;
            float DistBwActors = FVector::Distance(OwnerLocation,DetectionLocation);
            if(DistBwActors > DetectionRange)
            {
                // UE_LOG(LogTemp,Warning,TEXT("Distance between %f"),DistBwActors);
                continue;
            }
            // UE_LOG(LogTemp,Warning,TEXT("Sensed")); 
            Listener.RegisterStimulus(Event.Instigator,FAIStimulus(*this,Event.ViolenceRange,Event.ViolenceLocation,OwnerLocation));
        }
    }
    ViolenceEvent.Reset();
    return SuspendNextUpdate;
}

void UAISense_Violence::RegisterEvent(const FAIViolenceEvent& Event)
{
    ViolenceEvent.Add(Event);
    RequestImmediateUpdate();
}

void UAISense_Violence::ReportViolenceEvent(UObject* WorldContextObject, FVector ViolenceLocation, AActor* Instigator ,float ViolenceRange)
{
    UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(WorldContextObject);
    if(PerceptionSystem)
    {
        FAIViolenceEvent Event(Instigator, ViolenceLocation, ViolenceRange);
        PerceptionSystem->OnEvent(Event);
    }
}


void UAISense_Violence::OnNewLisntenerImpl(const FPerceptionListener& NewListener)
{
    UAIPerceptionComponent* ListenerPerception = NewListener.Listener.Get();
    check(ListenerPerception);
    const UAISenseConfig_Violence* SenseConfig = Cast<const UAISenseConfig_Violence>(ListenerPerception->GetSenseConfig(GetSenseID()));
    check(SenseConfig);
    ListenerPorps.Add(NewListener.GetListenerID(),SenseConfig->VoilenceDetectionRange);
}

void UAISense_Violence::OnListenerUpdated(const FPerceptionListener& UpdatedListener)
{
    const FPerceptionListenerID ListenerId = UpdatedListener.GetListenerID();
    if(UpdatedListener.HasSense(GetSenseID()))
    {
        const UAISenseConfig_Violence* SenseConfig = Cast<const UAISenseConfig_Violence>(UpdatedListener.Listener->GetSenseConfig(GetSenseID()));
        check(SenseConfig);
        float& NewRange = ListenerPorps.FindOrAdd(ListenerId);
        NewRange = SenseConfig->VoilenceDetectionRange;
    }
    else
    {
        ListenerPorps.Remove(ListenerId);
    }
}

void UAISense_Violence::OnListenerRemovedImpl(const FPerceptionListener& UpdatedListner)
{
    ListenerPorps.FindAndRemoveChecked(UpdatedListner.GetListenerID());
}
