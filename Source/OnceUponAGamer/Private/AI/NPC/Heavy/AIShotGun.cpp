// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Heavy/AIShotGun.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


void AAIShotGun::BeginPlay()
{
	Super::BeginPlay();
	CurrentAbilityBulletCount = AbilityBulletCount;
}
void AAIShotGun::ShootingInAction()
{
    // UE_LOG(LogTemp,Warning,TEXT("Shooting"));
    if(GetOwner() == nullptr)
	{
		// UE_LOG(LogTemp,Warning,TEXT("No Owner"));
		return;
	}
	ShootingInActionContinues();
	// else 
	// {
	// 	if(!bIsReloading)
	// 	{
	// 		Reload();
	// 	}
	// }
}

void AAIShotGun::ShootingInActionContinues()
{
	FHitResult GunTraceHit;
	TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(GetOwner());

	for(int i = 0 ; i < ShotgunPalletCount ; i++)
	{
        FVector RandomVector = FVector(UKismetMathLibrary::RandomFloatInRange(-RandomDeviation,RandomDeviation),UKismetMathLibrary::RandomFloatInRange(-RandomDeviation,RandomDeviation),UKismetMathLibrary::RandomFloatInRange(-RandomDeviation,RandomDeviation));
		FVector EndTrace = Muzzel->GetComponentLocation() + 5000 * GetOwner()->GetActorForwardVector() + RandomVector;
		UKismetSystemLibrary::LineTraceSingle(this,
											Muzzel->GetComponentLocation(),
											EndTrace,
											UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
											false,
											ActorsToIgnore,
											EDrawDebugTrace::None,
											GunTraceHit,
											true);
		AActor* HitActor = GunTraceHit.GetActor();
		if(HitActor)
		{
			if(HitActor->CanBeDamaged())
			{
				UGameplayStatics::ApplyPointDamage(HitActor,Damage,GunTraceHit.TraceStart,GunTraceHit,GetOwner()->GetInstigatorController(),this,UDamageType::StaticClass());
				// UE_LOG(LogTemp,Warning,TEXT("Damaged"));
			}
		}
	}
											
		// if(TracerRound)
		// {
		// 	GetWorld()->SpawnActor<AActor>(TracerRound,Muzzel->GetComponentLocation(),GetOwner()->GetActorForwardVector().Rotation())->SetLifeSpan(1.f);
		// }
		if(GunShootAnim)
		{
			GunMesh->PlayAnimation(GunShootAnim,false);
		}

		if(OwnerMesh && CharacterShootMontage)
		{
			// UE_LOG(LogTemp,Warning,TEXT("Inside the owner shooting condition"));
			OwnerMesh->GetAnimInstance()->Montage_Play(CharacterShootMontage);
		}
		CurrentAmmo--;
		CurrentAbilityBulletCount-- ;

	if (AbilityTimer.IsValid() && CurrentAbilityBulletCount <= 0)
		{
			// UE_LOG(LogTemp,Error,TEXT("Ability Count 0"));
			GetWorld()->GetTimerManager().ClearTimer(AbilityTimer);
		}
}
void AAIShotGun::ReleaseAbility()
{
			// UE_LOG(LogTemp,Warning,TEXT("ReleaseAbility Called"));
	CurrentAbilityBulletCount = AbilityBulletCount;
	if(CurrentAbilityBulletCount > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(AbilityTimer,this,&AAIShotGun::ShootingInActionContinues,AbilitySpeed,true);
	}
	else
	{
		if(GetWorld()->GetTimerManager().IsTimerActive(AbilityTimer))
		{
			AbilityTimer.Invalidate();
		}
		CurrentAbilityBulletCount = AbilityBulletCount;
	}	
}