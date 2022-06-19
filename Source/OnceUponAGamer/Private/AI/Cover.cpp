// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Cover.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
ACover::ACover()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;

}

// Called when the game starts or when spawned
void ACover::BeginPlay()
{
	Super::BeginPlay();
}

FVector ACover::GiveCoverPoints()
{
	
	float CoverDistance = 0;
	FVector CoverPointLocation;
	for(FVector TempCoverLoc : CoverPoints)
	{
		TempCoverLoc = UKismetMathLibrary::TransformLocation(GetActorTransform(),TempCoverLoc);
		float Distance = UKismetMathLibrary::Vector_Distance(UGameplayStatics::GetPlayerPawn(this,0)->GetActorLocation(), TempCoverLoc);
		if(Distance > CoverDistance)
		{
			CoverDistance = Distance;
			CoverPointLocation = TempCoverLoc;
		}
	}	
	CoverDistance = 0;
	return CoverPointLocation;
	// return ClosestPointToActor(UGameplayStatics::GetPlayerPawn(this,0),CoverPoints);
}

FVector ACover::GivePeakPoints(AActor* Requester)
{
	float CoverDistance = 1000;
	FVector CoverPointLocation;
	for(FVector TempCoverLoc : PeakCoverPoints)
	{
		TempCoverLoc = UKismetMathLibrary::TransformLocation(GetActorTransform(),TempCoverLoc);
		float Distance = UKismetMathLibrary::Vector_Distance(Requester->GetActorLocation(), TempCoverLoc);
		if(Distance < CoverDistance)
		{
			CoverDistance = Distance;
			CoverPointLocation = TempCoverLoc;
		}
	}	
	CoverDistance = 1000;
	return CoverPointLocation;
	// return ClosestPointToActor(Requester,PeakCoverPoints);
}

// FVector ACover::ClosestPointToActor(AActor* CompareActor,TArray<FVector> Points)
// {
// 	float CoverDistance = 0;
// 	FVector CoverPointLocation;
// 	for(FVector TempCoverLoc : Points)
// 	{
// 		TempCoverLoc = UKismetMathLibrary::TransformLocation(GetActorTransform(),TempCoverLoc);
// 		float Distance = UKismetMathLibrary::Vector_Distance(CompareActor->GetActorLocation(), TempCoverLoc);
// 		if(Distance > CoverDistance)
// 		{
// 			CoverDistance = Distance;
// 			CoverPointLocation = TempCoverLoc;
// 		}
// 	}	
// 	CoverDistance = 0;
// 	return CoverPointLocation;
// }