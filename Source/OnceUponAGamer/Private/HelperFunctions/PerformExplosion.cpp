// Fill out your copyright notice in the Description page of Project Settings.


#include "HelperFunctions/PerformExplosion.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UPerformExplosion::StartExplosion(const UObject* WorldContextObject,float BaseDamage, FVector Origin, float DamageRadius, AActor* DamageCauser, TArray<AActor*> ActorsToIgnore, USoundBase* ExplosionSound, UParticleSystem* ExplosionParticles, TSubclassOf<UDamageType> ExplosionDamageType, TSubclassOf<UCameraShakeBase> CameraShake, float CameraShakeValue)
{
	if (!ExplosionDamageType)
	{
		ExplosionDamageType = UDamageType::StaticClass();
	}
	
	UGameplayStatics::ApplyRadialDamage(WorldContextObject, BaseDamage, Origin, DamageRadius, ExplosionDamageType, ActorsToIgnore, DamageCauser);
		
	if (ExplosionParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(WorldContextObject->GetWorld(), ExplosionParticles, Origin);
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(WorldContextObject, ExplosionSound, Origin);
	}

	if (CameraShake)
	{
		float Distance = UKismetMathLibrary::Vector_DistanceSquared(Origin, UGameplayStatics::GetPlayerPawn(WorldContextObject, 0)->GetActorLocation());
		// UE_LOG(LogTemp,Warning,TEXT("Grenade Distance %f"),Distance);
		 CameraShakeValue = FMath::Clamp((40000.f/Distance),0.1f,1.0f) * CameraShakeValue;
		UGameplayStatics::GetPlayerCameraManager(WorldContextObject, 0)->StartCameraShake(CameraShake, CameraShakeValue);
	}
}
