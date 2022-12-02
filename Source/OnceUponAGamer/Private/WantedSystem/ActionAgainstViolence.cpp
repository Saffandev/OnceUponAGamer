// Fill out your copyright notice in the Description page of Project Settings.


#include "WantedSystem/ActionAgainstViolence.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interface/NPCAI_Action.h"

// Sets default values
AActionAgainstViolence::AActionAgainstViolence()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AActionAgainstViolence::BeginPlay()
{
	Super::BeginPlay();
	
}


void AActionAgainstViolence::RegisterViolenceType(EViolenceType ViolenceType)
{
	CurrentViolenceType = ViolenceType;
	if(CurrentViolenceType == EViolenceType::EVT_PoliceKilled)
	{
		UE_LOG(LogTemp,Warning,TEXT("Police Killed ============"));
		PoliceDead();
	}
	UE_LOG(LogTemp,Warning,TEXT("Violence Registered"));
	StartAction();
}

void AActionAgainstViolence::StartAction()
{
	// FName v = (FName)(uint8)CurrentViolenceType;
	FName RowName = UKismetStringLibrary::Conv_StringToName(UKismetStringLibrary::Conv_ByteToString((uint8)CurrentViolenceType));
	const FString ContextString = "Name";
	const FViolenceTypeData* ViolenceTypeData = ViolenceTypeDataTable->FindRow<FViolenceTypeData>(RowName,ContextString);
	if(ViolenceTypeData == nullptr)
	{
		UE_LOG(LogTemp,Error,TEXT("ViolenceTypeData is empty"));
		return;
	}
	MaxWantedLevelForCurrentActiveViolence = FMath::Max(ViolenceTypeData->MaxWantedLevel,MaxWantedLevelForCurrentActiveViolence);
	CurrentViolenceValue += ViolenceTypeData->ViolenceValue * ViolenceValueFallOff; 
	UE_LOG(LogTemp,Warning,TEXT("Backup Called"));
	CurrentWantedLevel = FMath::Clamp((int)CurrentViolenceValue,0,MaxWantedLevelForCurrentActiveViolence);
	UE_LOG(LogTemp,Warning,TEXT("Current violence Value  %f"),CurrentViolenceValue);
	if(CurrentWantedLevel > 0 && CurrentWantedLevel > PreviousWantedLevel)
	{
		
		ViolenceValueFallOff = 0.5f/(float)CurrentWantedLevel;
		PreviousWantedLevel = CurrentWantedLevel;
		CallForPolice();
	}
}

void AActionAgainstViolence::CallForPolice()
{
	EnemySpawnIndex++;
	SpawnPolice(EnemySpawnIndex);
	GetWorld()->GetTimerManager().SetTimer(SpawnDelayTimerHandle,this,&AActionAgainstViolence::SpawnPoliceDelayHandeler,(int)(CurrentWantedLevel * 1.5),false);
}

void AActionAgainstViolence::SpawnPoliceDelayHandeler()
{
	if(EnemySpawnIndex == CurrentWantedLevel)
	{
		EnemySpawnIndex = 0;
		if(!GetWorld()->GetTimerManager().IsTimerActive(BackupTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(BackupTimerHandle,this,&AActionAgainstViolence::CallForBackup,7.f,true);
		}
		else
		{
			CallForPolice();
		}
		
	}
}

void AActionAgainstViolence::CallForBackup()
{
	UE_LOG(LogTemp,Warning,TEXT("Backup Called"));
	if(MaxPolicePerWantedLevel[CurrentWantedLevel - 1] > CurrentLivingPolice)
	{
		SpawnPolice(UKismetMathLibrary::RandomIntegerInRange(1,CurrentWantedLevel));
	}
}

void AActionAgainstViolence::SpawnPolice(int SpawnNumber)
{
	FName RowName = UKismetStringLibrary::Conv_StringToName(UKismetStringLibrary::Conv_IntToString(SpawnNumber));
	const FString ContextString = "None";
	FEnemyTypeData* EnemyTypeData = EnemyTypeDataTable->FindRow<FEnemyTypeData>(RowName,ContextString);
	int max = MaxPolicePerWantedLevel[CurrentWantedLevel - 1] > CurrentLivingPolice ? MaxPolicePerWantedLevel[CurrentWantedLevel - 1] : 0;
	int loopIndex = FMath::Clamp(EnemyTypeData->SpawnCount + UKismetMathLibrary::RandomIntegerInRange(-1,1),0,max);
	if(EnemyTypeData == nullptr)
	{
		UE_LOG(LogTemp,Error,TEXT("Enemy Type Datatable is empty"));
		return;
	}
	for( int32 i = 0; i < loopIndex; i++)
	{
		if(TargetPoints.Num() == 0 && TargetPoints[i] == nullptr)
		{
			UE_LOG(LogTemp,Warning,TEXT("Target Point NUll"));
			continue;
		}
		const FVector SpawnLocation = TargetPoints[UKismetMathLibrary::RandomIntegerInRange(0,TargetPoints.Num()-1)]->GetActorLocation();
		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(EnemyTypeData->Enemy,SpawnLocation,FRotator(0,0,0));
		CurrentLivingPolice++;
		INPCAI_Action* AIAction = Cast<INPCAI_Action>(SpawnedActor);
		if(AIAction)
		{
			AIAction->SetWantedLevelCooldown(false);
		}
		//call the cooldown function on spawned actors
	}

}

void AActionAgainstViolence::PoliceDead()
{
	CurrentLivingPolice--;
	if(CurrentLivingPolice < 0)
	{
		CurrentLivingPolice = 0;
	}
}

void AActionAgainstViolence::DecreaseWantedLevel()
{
	CurrentWantedLevel--;
	if(CurrentWantedLevel <= 0)
	{
		CurrentWantedLevel = 0;
		PreviousWantedLevel = 0;
		CurrentViolenceValue = 0;
		ViolenceValueFallOff = 1.f;
		MaxWantedLevelForCurrentActiveViolence = 0;
		GetWorld()->GetTimerManager().ClearTimer(BackupTimerHandle);
		CurrentViolenceType = EViolenceType::EVT_None;
	}
}