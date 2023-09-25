// Fill out your copyright notice in the Description page of Project Settings.


#include "Throwable/ThrowableBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/PlayerCharacter.h"
#include "AI/NPC/Basic/BasicNPCAI.h"
#include "DrawDebugHelpers.h"
#include "HelperFunctions/ViolenceRegistration.h"

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
	// UE_LOG(LogTemp,Warning,TEXT("ThrowableSpawned"));
	ThrowableMesh->OnComponentHit.AddDynamic(this,&AThrowableBase::OnHit);
	ThrowableMesh->SetSimulatePhysics(true);
	if(GetOwner())
	{
		// UE_LOG(LogTemp,Warning,TEXT("Owner of the throwable"));
		ThrowableMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,ECollisionResponse::ECR_Ignore);
		ThrowableMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
		ThrowableMesh->SetSimulatePhysics(false);
	}
	ThrowableMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Ignore);
	OverlapActorObjectType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	OverlapActorObjectType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	OverlapActorObjectType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	bIsExploded = false;

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
		PlayerCharacter->PrimaryThrowableData.ThrowableName = ThrowableName;
	}
	else if(PlayerCharacter->SecondaryThrowableData.BP_Throwable == nullptr)
	{
		PlayerCharacter->SecondaryThrowableData.BP_Throwable = LThrowable;
		PlayerCharacter->SecondaryThrowableData.Count++;
		PlayerCharacter->SecondaryThrowableData.ThrowableName = ThrowableName;

	}
	else
	{
		if(PlayerCharacter->ThrowableEquippedSlot == 0)
		{
			PlayerCharacter->DropThrowable(true);
			PlayerCharacter->PrimaryThrowableData.BP_Throwable = LThrowable;
			PlayerCharacter->PrimaryThrowableData.Count++;
			PlayerCharacter->PrimaryThrowableData.ThrowableName = ThrowableName;

		}
		else if(PlayerCharacter->ThrowableEquippedSlot == 1)
		{
			PlayerCharacter->DropThrowable(false);
			PlayerCharacter->SecondaryThrowableData.BP_Throwable = LThrowable;
			PlayerCharacter->SecondaryThrowableData.Count++;
			PlayerCharacter->SecondaryThrowableData.ThrowableName = ThrowableName;

		}
	}

	Destroy();


}
void AThrowableBase::PickupWeapon()
{
	
}
void AThrowableBase::SetPickupWeaponName()
{
	PlayerCharacter->ThrowableName = ThrowableName;
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
	// UE_LOG(LogTemp,Warning,TEXT("ThrowVelocity = %s"),*ThrowVelocity.ToString());
	ThrowableMesh->SetSimulatePhysics(true);
	ThrowableMesh->AddImpulse(ThrowVelocity,NAME_None,true);
	ThrowableMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
}

void AThrowableBase::Initiate()
{
	UE_LOG(LogTemp,Warning,TEXT("Timer Initiated"));
	GetWorld()->GetTimerManager().SetTimer(ThrowTimer,this,&AThrowableBase::Explode,ExplodeTimer,false);
}

void AThrowableBase::Explode()
{	
	UE_LOG(LogTemp,Warning,TEXT("Explode"));
	if(PlayerCharacter)
	{
		if(PlayerCharacter->GetIsThrowingThrowable())
		{
			PlayerCharacter->ReEquipAfterGrenade();
		}
	}
	PlayerCharacter->bIsThrowableExploded = true;
	bIsExploded = true;
	if(ExplodeParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ExplodeParticle,GetActorLocation());
	}
	if(ExplodeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,ExplodeSound,GetActorLocation());
	}
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	TArray<AActor*> OverlappedActors;
	
	UClass* SeekClass = nullptr;
	// DrawDebugSphere(GetWorld(),GetActorLocation(),DamageRadius,12,FColor::Red,true,5.f);
	/*UKismetSystemLibrary::SphereOverlapActors(this,
											  GetActorLocation(),
											  DamageRadius,
											  OverlapActorObjectType,
											  SeekClass,
											  ActorsToIgnore,
											  OverlappedActors);*/
	// UE_LOG(LogTemp,Warning,TEXT("%i"),OverlappedActors.Num());
	// UE_LOG(LogTemp,Warning,TEXT("Overlap object type count %i"),OverlapActorObjectType.Num());

	if(PlayerCharacter)
	{
		float Distance = this->GetDistanceTo(PlayerCharacter);
		// UE_LOG(LogTemp,Warning,TEXT("Grenade Distance %f"),Distance);
		float CameraShakeValue = (1 - Distance/ShakeDistance) * CameraShakeScale;
		// UE_LOG(LogTemp,Warning,TEXT("Camera shake value %f"),CameraShakeValue);
		PlayerCharacter->PlayCameraShake(CameraShake,CameraShakeValue);
	}
	//UViolenceRegistration::RegisterViolence(this,GetActorLocation(),PlayerCharacter,EViolenceType::EVT_Explosion);

	
			// UE_LOG(LogTemp,Warning,TEXT("%s"),*(HitActor->GetName()));			
			UGameplayStatics::ApplyRadialDamage(this,Damage,GetActorLocation(),DamageRadius,ExplosionDamageType,ActorsToIgnore);
			// UE_LOG(LogTemp,Warning,TEXT("Radial Damage applied"));
	Destroy();

}

