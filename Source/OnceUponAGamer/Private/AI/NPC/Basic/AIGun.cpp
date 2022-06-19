// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Basic/AIGun.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Interface/AI_GunInterface.h"

// Sets default values
AAIGun::AAIGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	RootComponent = GunMesh;
	Muzzel = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzel"));
	Muzzel->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AAIGun::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmo = MaxAmmo;
}

// Called every frame
void AAIGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAIGun::StartShooting()
{
	GetWorld()->GetTimerManager().SetTimer(ShootingTimerHandle,this,&AAIGun::ShootingInAction,FireRate,true,0);
}

void AAIGun::ShootingInAction()
{
	if(GetOwner() == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("No Owner"));
		return;
	}
	
	if(CurrentAmmo > 0)
	{
		FVector EndTrace = Muzzel->GetComponentLocation() + 5000 * GetOwner()->GetActorForwardVector() ;
		FHitResult GunTraceHit;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(GetOwner());
		UKismetSystemLibrary::LineTraceSingle(this,
											Muzzel->GetComponentLocation(),
											EndTrace,
											UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
											false,
											ActorsToIgnore,
											EDrawDebugTrace::None,
											GunTraceHit,
											true);
		if(TracerRound)
		{
			GetWorld()->SpawnActor<AActor>(TracerRound,Muzzel->GetComponentLocation(),GetOwner()->GetActorForwardVector().Rotation())->SetLifeSpan(1.f);
		}
		if(GunShootAnim)
		{
			GunMesh->PlayAnimation(GunShootAnim,false);
		}

		if(OwnerMesh && CharacterShootMontage)
		{
			UE_LOG(LogTemp,Warning,TEXT("Inside the owner shooting condition"));
			OwnerMesh->GetAnimInstance()->Montage_Play(CharacterShootMontage);
		}

		AActor* HitActor = GunTraceHit.GetActor();
		if(HitActor)
		{
			if(HitActor->CanBeDamaged())
			{
				UGameplayStatics::ApplyPointDamage(HitActor,Damage,GunTraceHit.TraceStart,GunTraceHit,GetOwner()->GetInstigatorController(),this,UDamageType::StaticClass());
				UE_LOG(LogTemp,Warning,TEXT("Damaged"));
			}
		}
		CurrentAmmo--;
	}
	else 
	{
		if(!bIsReloading)
		{
			Reload();
		}
	}
}

void AAIGun::StopShooting()
{
	if(ShootingTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ShootingTimerHandle);
	}
}

void AAIGun::Reload()
{
	if(!GetOwner())
	{
		return;
	}

	bIsReloading = true;
	// IAI_GunInterface* OwnerInterface = Cast<IAI_GunInterface>(GetOwner());
	// if(OwnerInterface)
	// {
	// 	OwnerInterface->SetReload(true);
	// }

	float ReloadTime = 0;
	if(OwnerMesh)
	{
		ReloadTime = OwnerMesh->GetAnimInstance()->Montage_Play(ReloadMontage);
	}

	FTimerHandle ReloadTimer;
	GetWorld()->GetTimerManager().SetTimer(ReloadTimer,[&](){CurrentAmmo = MaxAmmo;
															bIsReloading = false;
															},
															0.1,
															false,
															ReloadTime);
}
