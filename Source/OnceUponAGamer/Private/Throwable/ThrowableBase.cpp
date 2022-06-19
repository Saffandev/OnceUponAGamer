// Fill out your copyright notice in the Description page of Project Settings.


#include "Throwable/ThrowableBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/PlayerCharacter.h"

// Sets default values
AThrowableBase::AThrowableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	ThrowableMesh = CreateDefaultSubobject<UStaticMeshComponent>("ThrowableMesh");
	RootComponent = ThrowableMesh;
}

// Called when the game starts or when spawned
void AThrowableBase::BeginPlay()
{
	Super::BeginPlay();
	ThrowableMesh->OnComponentHit.AddDynamic(this,&AThrowableBase::OnHit);
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
}

// Called every frame
void AThrowableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AThrowableBase::SetupPickupThrowable(TSubclassOf<AThrowableBase> LThrowable)
{
if(!PlayerCharacter || !LThrowable)
	{
		return;
	}

	if(PlayerCharacter->PrimaryThrowableData.BP_Throwable == nullptr)
	{
		PlayerCharacter->PrimaryThrowableData.BP_Throwable = LThrowable;
		PlayerCharacter->PrimaryThrowableData.Count++;
	}
	else if(PlayerCharacter->SecondaryThrowableData.BP_Throwable == nullptr)
	{
		PlayerCharacter->SecondaryThrowableData.BP_Throwable = LThrowable;
		PlayerCharacter->SecondaryThrowableData.Count++;
	}
	else
	{
		if(PlayerCharacter->ThrowableEquippedSlot == 0)
		{
			PlayerCharacter->PrimaryThrowableData.BP_Throwable = LThrowable;
			PlayerCharacter->PrimaryThrowableData.Count++;
		}
		else if(PlayerCharacter->ThrowableEquippedSlot == 1)
		{
			PlayerCharacter->SecondaryThrowableData.BP_Throwable = LThrowable;
			PlayerCharacter->SecondaryThrowableData.Count++;
		}
	}

	Destroy();


}
void AThrowableBase::PickupWeapon()
{
	
}
void AThrowableBase::SetPickupWeaponName()
{

}
bool AThrowableBase::IsPickupGun() 
{
	return false;
}

void AThrowableBase::OnHit(UPrimitiveComponent* HitComponent,AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
	HitCount++;
	if(HitCount == 1)
	{
		ThrowableMesh->SetLinearDamping(2.f);
	}
	else if(HitCount == 2)
	{
		ThrowableMesh->SetLinearDamping(6.f);
		HitCount = 0;
	}
}


void AThrowableBase::Throw(FVector ThrowVelocity)
{
	UE_LOG(LogTemp,Warning,TEXT("ThrowVelocity = %s"),*ThrowVelocity.ToString());
	ThrowableMesh->SetSimulatePhysics(true);
	ThrowableMesh->AddImpulse(ThrowVelocity,NAME_None,true);
}

void AThrowableBase::Initiate()
{
	GetWorld()->GetTimerManager().SetTimer(ThrowTimer,this,&AThrowableBase::Explode,0.01,false,ExplodeTimer);
}

void AThrowableBase::Explode()
{	
	if(ExplodeParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ExplodeParticle,GetActorLocation());
	}
	if(ExplodeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,ExplodeSound,GetActorLocation());
	}
	TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> OverlappedActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> OverlapActorObjectType;
	UKismetSystemLibrary::SphereOverlapActors(this,
											  GetActorLocation(),
											  DamageRadius,
											  OverlapActorObjectType,
											  UClass::StaticClass(),
											  ActorsToIgnore,
											  OverlappedActors);

	for(AActor* HitActor:OverlappedActors)
	{	
		if(HitActor && HitActor->CanBeDamaged())
		{
			UGameplayStatics::ApplyRadialDamage(this,Damage,GetActorLocation(),DamageRadius,ExplosionDamageType,ActorsToIgnore);
		}
	}
	Destroy();

}

