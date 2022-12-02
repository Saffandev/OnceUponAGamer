// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enum/EnumViolenceType.h"
#include "Engine/DataTable.h"
#include "ActionAgainstViolence.generated.h"

USTRUCT(BlueprintType)
struct FViolenceTypeData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	EViolenceType ViolenceType;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float ViolenceValue;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int32 MaxWantedLevel;
};

USTRUCT(BlueprintType)
struct FEnemyTypeData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<AActor> Enemy;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int32 SpawnCount;
};

UCLASS()
class ONCEUPONAGAMER_API AActionAgainstViolence : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActionAgainstViolence();
	UFUNCTION()
	void RegisterViolenceType(EViolenceType ViolenceType);

private:
	void StartAction();
	void PoliceDead();
	void CallForPolice();
	void CallForBackup();
	void SpawnPolice(int SpawnCount);
	void SpawnPoliceDelayHandeler();
	UFUNCTION(BlueprintCallable)
	void DecreaseWantedLevel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<AActor*> TargetPoints;

private:
	UPROPERTY(EditAnywhere)
	UDataTable* ViolenceTypeDataTable;
	UPROPERTY(EditAnywhere)
	UDataTable* EnemyTypeDataTable;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	EViolenceType CurrentViolenceType;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	int32 CurrentWantedLevel;
	int32 PreviousWantedLevel;
	UPROPERTY(EditAnywhere)
	int32 MaxWantedLevel;
	int32 MaxWantedLevelForCurrentActiveViolence;
	int32 CurrentLivingPolice;
	int32 EnemySpawnIndex;
	UPROPERTY(EditAnywhere)
	TArray<int32> MaxPolicePerWantedLevel;
	
	float CurrentViolenceValue;
	float ViolenceValueFallOff = 1;
	float BackupTimerValue = 7.f;
	FTimerHandle BackupTimerHandle;
	FTimerHandle SpawnDelayTimerHandle;

};
