// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/PickupWeaponBase.h"
#include "Weapons/KnifeWeapon.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Touch.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/PickupWeaponInterface.h"
#include "DrawDebugHelpers.h"
#include "Throwable/ThrowableBase.h"

//=====================constructor=================================//

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SceneComp->SetupAttachment(RootComponent);
	SceneComp->SetRelativeLocation(FVector(-14.75f, 0.f, 75.f));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SceneComp);
	Camera->bUsePawnControlRotation = true;
	Camera->SetRelativeLocation(FVector(-14.75f, 0.f, 75.f));

	PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerMesh"));
	PlayerMesh->SetupAttachment(Camera);
	PlayerMesh->CastShadow = false;

	// PredictionSpline = CreateDefaultSubobject<USplineComponent>("Spline");
	// PredictionSpline->SetupAttachment(RootComponent);

	AIPerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIPerceptionStimuliSourcee"));
	AIPerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
	AIPerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense_Hearing>());
	AIPerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense_Damage>());
	AIPerceptionStimuliSource->RegisterWithPerceptionSystem();

	GetCapsuleComponent()->InitCapsuleSize(40.f, 96.f);
	bIsWallRunning = false;
	bCanDoWallRunAgain = true;
	bCanEndWallRun = false;
	
	//Setting Default Values for the WeaponStruct
	PrimaryWeapon.WeaponName = EWeaponName::EWN_None;
	PrimaryWeapon.MaxAmmo = 0;
	PrimaryWeapon.TotalAmmo = 0;
	PrimaryWeapon.CurrentMagAmmo = 0;

	SecondaryWeapon.WeaponName = EWeaponName::EWN_None;
	SecondaryWeapon.MaxAmmo = 0;
	SecondaryWeapon.TotalAmmo = 0;
	SecondaryWeapon.CurrentMagAmmo = 0;
}

//=====================Begin Play==================================//

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Components Setup
	CharacterMovement = GetCharacterMovement();
	CharacterMovement->AirControl = 0.2f;
	CharacterMovement->SetPlaneConstraintEnabled(true);

	Capsule = GetCapsuleComponent();
	Capsule->SetCapsuleHalfHeight(96.f);
	StandingCapsuleHeight = Capsule->GetUnscaledCapsuleHalfHeight();
	CrouchCapsuleHeight = StandingCapsuleHeight * CrouchScale;

	StandingCameraHeight = Camera->GetRelativeLocation().Z;
	CrouchCameraHeight = StandingCameraHeight * CrouchScale;
	PlayerMeshTransform = PlayerMesh->GetRelativeTransform();

	PlayerController = GetController();

	// Binding Section
	Capsule->OnComponentHit.AddDynamic(this, &APlayerCharacter::OnHit);
	OnTakeAnyDamage.AddDynamic(this,&APlayerCharacter::TakeAnyDamage);
	OnTakePointDamage.AddDynamic(this,&APlayerCharacter::TakePointDamage);
	OnTakeRadialDamage.AddDynamic(this,&APlayerCharacter::TakeRadialDamage);
	
	// CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(InitialWeapon);
	// CurrentWeapon->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("WeaponSocket_r"));
	
	// EqPrimaryWeapon = CurrentWeapon;
	// EqSecondaryWeapon = CurrentWeapon;
	// PrimaryWeapon.WeaponClass = InitialWeapon;//Here initial weapon is knife(can also be null but for that some code need to be changed)
	// SecondaryWeapon.WeaponClass = InitialWeapon;

	JumpsLeft = MaxJumps;
	WalkSpeed = CharacterMovement->MaxWalkSpeed;
	CurrentMovementType = EMovementType::EMT_Walking;

	DefaultFOV = Camera->FieldOfView;
	WeaponEquippedSlot = 0;

	CurrentHealth = MaxHealth;
	CurrentShield = MaxShield;
	bCanThrowAgain = true;
	bCanSwitchWeapon = true;
	bCanPickupWeapon = true;
	//=========================================================================timeline setup==========================================//

	if (CrouchCurve)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("ToggleCrouch"));
		CrouchTimeline.AddInterpFloat(CrouchCurve, TimelineCallback);
	}

	if (SlideCurve)
	{
		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic FinishedCallback;
		TimelineCallback.BindUFunction(this, FName("PerformSlide"));
		FinishedCallback.BindUFunction(this, FName("EndSlide"));
		SlideTimeline.AddInterpFloat(SlideCurve, TimelineCallback);
		SlideTimeline.SetTimelineFinishedFunc(FinishedCallback);
	}

	if (CameraTiltCurve)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("CameraTiltAction"));
		CameraTiltTimeline.AddInterpFloat(CameraTiltCurve, TimelineCallback);
	}

	if (WallRunUpdateCurve)
	{
		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic FinishedCallback;
		TimelineCallback.BindUFunction(this, FName("WallRunInAction"));
		FinishedCallback.BindUFunction(this, FName("WallRunOutOfAction"));
		WallRunUpdateTimeline.AddInterpFloat(WallRunUpdateCurve, TimelineCallback);
		WallRunUpdateTimeline.SetTimelineFinishedFunc(FinishedCallback);
	}

	if (CrouchCurve)
	{
		FOnTimelineFloat TimelineCallBackADSOn;
		TimelineCallBackADSOn.BindUFunction(this, FName("ADSOnInAction"));
		ADS_OnTimeline.AddInterpFloat(CrouchCurve, TimelineCallBackADSOn);

		FOnTimelineFloat TimelineCallBackADSOff;
		TimelineCallBackADSOff.BindUFunction(this, FName("ADSOffInAction"));
		ADS_OffTimeline.AddInterpFloat(CrouchCurve, TimelineCallBackADSOff);
	}
}

//=============================================================================Tick Section=========================================//

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CrouchTimeline.TickTimeline(DeltaTime);
	SlideTimeline.TickTimeline(DeltaTime);
	CameraTiltTimeline.TickTimeline(DeltaTime);
	WallRunUpdateTimeline.TickTimeline(DeltaTime);
	ADS_OnTimeline.TickTimeline(DeltaTime);
	ADS_OffTimeline.TickTimeline(DeltaTime);

	PickupInAction();//Function that tracks pickables hit with player

	if (bCanPredictPath)
	{
		ThrowPredection();//Grenade Throw predection
	}
}

//=================================================================================Setup Player Input==============================//

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"),   this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"),        this, &APlayerCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"),      this, &APlayerCharacter::LookUp);
	
	PlayerInputComponent->BindAction(TEXT("Jump"), 			 	EInputEvent::IE_Pressed, 	this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), 			 	EInputEvent::IE_Released, 	this, &APlayerCharacter::StopJump);
	PlayerInputComponent->BindAction(TEXT("Crouch"), 		 	EInputEvent::IE_Pressed, 	this, &APlayerCharacter::Crouch);
	PlayerInputComponent->BindAction(TEXT("Sprint"), 		 	EInputEvent::IE_Pressed, 	this, &APlayerCharacter::Sprint);
	PlayerInputComponent->BindAction(TEXT("Sprint"), 		 	EInputEvent::IE_Released, 	this, &APlayerCharacter::CancelSprint);
	PlayerInputComponent->BindAction(TEXT("Shoot"), 		 	EInputEvent::IE_Pressed, 	this, &APlayerCharacter::Shoot);
	PlayerInputComponent->BindAction(TEXT("Shoot"), 		 	EInputEvent::IE_Released, 	this, &APlayerCharacter::StopShooting);
	PlayerInputComponent->BindAction(TEXT("Reload"), 		 	EInputEvent::IE_Pressed, 	this, &APlayerCharacter::Reload);
	PlayerInputComponent->BindAction(TEXT("ADS"), 			 	EInputEvent::IE_Pressed, 	this, &APlayerCharacter::ADSON);
	PlayerInputComponent->BindAction(TEXT("ADS"), 			 	EInputEvent::IE_Released, 	this, &APlayerCharacter::ADSOFF);
	PlayerInputComponent->BindAction(TEXT("Pickup"), 		 	EInputEvent::IE_Pressed, 	this, &APlayerCharacter::Pickup);
	PlayerInputComponent->BindAction(TEXT("Pickup"), 		 	EInputEvent::IE_Released, 	this, &APlayerCharacter::NoPickup);
	PlayerInputComponent->BindAction(TEXT("PrimaryWeapon"),  	EInputEvent::IE_Pressed, 	this, &APlayerCharacter::EquipPrimaryWeapon);
	PlayerInputComponent->BindAction(TEXT("SecondaryWeapon"),	EInputEvent::IE_Pressed, 	this, &APlayerCharacter::EquipSecondaryWeapon);
	PlayerInputComponent->BindAction(TEXT("Melee"), 		 	EInputEvent::IE_Pressed, 	this, &APlayerCharacter::MeleeAttack);
	PlayerInputComponent->BindAction(TEXT("Throw"), 		 	EInputEvent::IE_Pressed, 	this, &APlayerCharacter::PrimaryThrowStart);
	PlayerInputComponent->BindAction(TEXT("Throw"), 		 	EInputEvent::IE_Released, 	this, &APlayerCharacter::PrimaryThrowEnd);
	PlayerInputComponent->BindAction(TEXT("ThrowableSwitch"),  	EInputEvent::IE_Released, 	this, &APlayerCharacter::SwitchThrowable);

}

//====================================================================OnComponentHit for capsule======================================//
void APlayerCharacter::OnHit(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalizeImpulse, const FHitResult &Hit)
{
	LedgeGrab(Hit.ImpactPoint);
	if(Hit.GetActor())
	{
		WallRun(Hit.GetActor(), Hit.ImpactNormal);
	}
}

//=================================================================Damage Events=====================================================//
void APlayerCharacter::TakeAnyDamage(AActor* DamagedActor,float Damage,const UDamageType* DamageType,AController* InstigatedBy, AActor* DamageCauser)
{
	TakeDamage(Damage);
	DamageMarker(DamageCauser);
}

void APlayerCharacter::TakePointDamage(AActor* DamagedActor,float Damage,AController* InstigatedBy, FVector HitLocation,UPrimitiveComponent* HitComp,FName BoneName,FVector ShotDirection,const UDamageType* DamageType,AActor* DamageCauser)
{
	DamageMarker(DamageCauser);
	TakeDamage(Damage);
}

void APlayerCharacter::TakeRadialDamage(AActor* DamagedActor,float Damage,const UDamageType* DamageType,FVector Origin,FHitResult Hit,AController* InstigatedBy,AActor* DamageCauser)
{
	DamageMarker(DamageCauser);
	TakeDamage(Damage);
}

void APlayerCharacter::TakeDamage(float Damage)
{
	if(CurrentShield <= 0)
		{
			CurrentShield = 0;
			CurrentHealth -= Damage;
			if(BreathingSoundComp == nullptr)
			{
				BreathingSoundComp = UGameplayStatics::SpawnSoundAttached(BreathingSound,Capsule);
			}
			BreathingSoundComp->SetVolumeMultiplier(1 - (CurrentHealth/100));
			if(ShieldRechargeTimer.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(ShieldRechargeTimer);
			}
			if(CurrentHealth <= 0)
			{
				CurrentHealth = 0;
				DisablePlayerInput();
				Dead();
			}
		}
		else
		{
			CurrentShield -= Damage;
			if(CurrentShield <= 0)
			{
				TakeDamage(UKismetMathLibrary::Abs(CurrentShield));
				CurrentShield = 0;
			}
			if(ShieldRechargeTimer.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(ShieldRechargeTimer);
			}
			GetWorld()->GetTimerManager().SetTimer(ShieldRechargeTimer,
												   this,
												   &APlayerCharacter::RegainShield,
												   0.1f,
												   true,
												   TimeForShieldRecharge);
		}
}

//==============================================================Healing Events=================================================//
void APlayerCharacter::Heal(float Health)
{

	CurrentHealth += Health;
	if(BreathingSoundComp)
	{
		BreathingSoundComp->SetVolumeMultiplier(1 - (CurrentHealth/100));
	}
	if(CurrentHealth > MaxHealth)
	{
		if(BreathingSoundComp)
		{
			BreathingSoundComp->SetVolumeMultiplier(0);
			BreathingSoundComp->Stop();
		}
		CurrentHealth = MaxHealth;
	}
}

void APlayerCharacter::SetHealthShield(float Health,float Shield)
{
	if(Health <= MaxHealth)
	{
		CurrentHealth = Health;
		Heal(0);
	}

	if(Shield <= MaxShield)
	{
		CurrentShield = Shield;
	}
}

void APlayerCharacter::HealShield(float ShieldHealth)
{
	CurrentShield += ShieldHealth;
	if(CurrentShield > MaxShield)
	{
		CurrentShield = MaxShield;
	}
}

void APlayerCharacter::RegainShield()// Will gain sheild overtime if its had not touched 0 && player hadn't taken damage for some time
{
	CurrentShield = UKismetMathLibrary::FInterpTo_Constant(CurrentShield,MaxShield,UGameplayStatics::GetWorldDeltaSeconds(this),ShieldRechargeRate);

	if(CurrentShield >=100)
	{
		GetWorld()->GetTimerManager().ClearTimer(ShieldRechargeTimer);
	}
}
//==========================================================================Ledge Grab===============================//

void APlayerCharacter::LedgeGrab(FVector ImpactPoint)
{
	if (CanLedgeGrab(ImpactPoint) & !bIsDoingLedgeGrab)
	{
		TArray<AActor *> ActorsToIgnore;
		FHitResult LedgeForwardOutHit;
		bool IsForwardHit = UKismetSystemLibrary::SphereTraceSingle(this,
																	ImpactPoint,
																	GetActorForwardVector() * 100 + ImpactPoint,
																	5,
																	UEngineTypes::ConvertToTraceType(ECC_Visibility),
																	false,
																	ActorsToIgnore,
																	EDrawDebugTrace::None,
																	LedgeForwardOutHit,
																	true);
		if (IsForwardHit)
		{
			FHitResult LedgeUpOutHit;
			bool IsUpHit = UKismetSystemLibrary::SphereTraceSingle(this,
																   LedgeForwardOutHit.ImpactPoint + FVector(0, 0, 150),
																   LedgeForwardOutHit.ImpactPoint,
																   20,
																   UEngineTypes::ConvertToTraceType(ECC_Visibility),
																   false,
																   ActorsToIgnore,
																   EDrawDebugTrace::None,
																   LedgeUpOutHit,
																   true);
			if (IsUpHit)
			{
				float HitDistance = FMath::Abs(LedgeUpOutHit.ImpactPoint.Z - GetActorLocation().Z);
				if (HitDistance <= 95.f)
				{
					FVector LandingLocation = FVector((GetActorLocation() + Camera->GetForwardVector() * 70).X,
													  (GetActorLocation() + Camera->GetForwardVector() * 70).Y,
													  LedgeUpOutHit.ImpactPoint.Z + Capsule->GetUnscaledCapsuleHalfHeight());

					FLatentActionInfo LatentInfo;
					LatentInfo.CallbackTarget = this;
					LatentInfo.ExecutionFunction = "EnablePlayerInput";
					LatentInfo.Linkage = 1;
					CharacterMovement->StopMovementImmediately();
					DisablePlayerInput();
					bIsDoingLedgeGrab = true;
					if(CurrentWeapon)
					{
						CurrentWeapon->UnDrawWeapon(2);
					}
					UKismetSystemLibrary::MoveComponentTo(Capsule,
														  LandingLocation,
														  GetActorRotation(),
														  true,
														  true,
														  0.3,
														  true,
														  EMoveComponentAction::Move,
														  LatentInfo);
		
				}
			}
		}
	}
}

bool APlayerCharacter::CanLedgeGrab(FVector ImpactPoint)
{
	//Canuncroch() => this function will check if there is nothing over the player that might cause issue while finishing the ledge grab
	return CanUncrouch(0.f) &&
		   CharacterMovement->IsFalling() &&
		   ForwardAxis > 0 &&
		   (GetActorLocation().Z + 90.f) >= ImpactPoint.Z &&
		   (GetActorLocation().Z - 50.f) <= ImpactPoint.Z;
}

//===============================================================================Wall Run Section==============================//

void APlayerCharacter::WallRun(AActor *HitActor, FVector HitImpactNormal)
{
	if (!bIsWallRunning)
	{
		bCanEndWallRun = false;
		if (CanDoWallRun(HitActor, HitImpactNormal))
		{

			float WallAngleWithPlayer = FVector2D::DotProduct(UKismetMathLibrary::Conv_VectorToVector2D(HitImpactNormal),
															  UKismetMathLibrary::Conv_VectorToVector2D(GetActorRightVector()));

			EWallRunDirection LWallRunSide = WallAngleWithPlayer > 0.f ? EWallRunDirection::EWRD_Right : EWallRunDirection::EWRD_Left;
			
			WallRunSide = LWallRunSide;
			
			WallRunDirection = FVector::CrossProduct(HitImpactNormal, FVector(0, 0, (WallRunSide == EWallRunDirection::EWRD_Left ? -1 : 1)));

			BeginWallRun();
		}
	}
}

//======================================Wall run conditions========================//
bool APlayerCharacter::CanDoWallRun(AActor *HitActor, FVector HitImpactNormal)
{
	return IsPlayerInStateOfWallRun(HitImpactNormal) &&
		   !(HitActor->ActorHasTag("NoWallRun")) &&
		   CanSurfaceBeWallRun(HitImpactNormal) &&
		   CharacterMovement->IsFalling();
}

bool APlayerCharacter::IsPlayerInStateOfWallRun(FVector HitImpactNormal)
{
	UKismetMathLibrary::Vector_Normalize(HitImpactNormal);
	float FloorAngleWithPlayer = UKismetMathLibrary::DegAcos(FVector::DotProduct(HitImpactNormal, GetActorForwardVector()));
	bool bAreRequiredKeyDown = ForwardAxis > 0 && UKismetMathLibrary::VSize(GetVelocity()) > 100.f;
	bool bIsFloorAngleRight = UKismetMathLibrary::InRange_FloatFloat(FloorAngleWithPlayer, 0, 150, true, true);
	return bAreRequiredKeyDown && bIsFloorAngleRight && bCanDoWallRunAgain;
}

bool APlayerCharacter::CanSurfaceBeWallRun(FVector HitImpactNormal)
{
	if (HitImpactNormal.Z < -0.05f)
	{
		return false;
	}
	float FloorInclineAngle = UKismetMathLibrary::DegAcos(FVector::DotProduct(FVector(HitImpactNormal.X, HitImpactNormal.Y, 0), HitImpactNormal));
	return FloorInclineAngle < CharacterMovement->GetWalkableFloorAngle();
}

//==========================================Wall run in action========================//
void APlayerCharacter::BeginWallRun()
{
	CanPerformCertainMovement(EMovementType::EMT_WallRun);
	CharacterMovement->GravityScale = 0.f;
	CharacterMovement->AirControl = 1.f;
	CharacterMovement->SetPlaneConstraintNormal(FVector(0, 0, 3));
	WallRunSoundComp = UGameplayStatics::SpawnSoundAttached(WallRunSound,Capsule,FName(NAME_None));
	CameraTiltTimeline.Play();
	WallRunUpdateTimeline.PlayFromStart();
}

void APlayerCharacter::CameraTiltAction()
{
	float Alpha = CameraTiltCurve->GetFloatValue(CameraTiltTimeline.GetPlaybackPosition()) *
				  (WallRunSide == EWallRunDirection::EWRD_Right ? 1 : -1);
				  
	FRotator ControlRotation = PlayerController->GetControlRotation();
	PlayerController->SetControlRotation(FRotator(ControlRotation.Pitch, ControlRotation.Yaw, Alpha));
}

void APlayerCharacter::WallRunInAction()
{
	if (bCanEndWallRun)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Delay timer runs out"));
		EndWallRun(EWallRunEndReason::EWRER_FallOfWall); // End wallrun(fall of wall)
	}
	UpdateWallRun();
}

void APlayerCharacter::WallRunOutOfAction()
{
	if (bIsWallRunning)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Timeline finished"));
		EndWallRun(EWallRunEndReason::EWRER_FallOfWall);
	}
}

void APlayerCharacter::UpdateWallRun()
{
	if (!(ForwardAxis > 0 && UKismetMathLibrary::VSize(GetVelocity()) > 100.f))//will stop the wall run if player release the forward input key or got stuck onto something
	{
		// UE_LOG(LogTemp, Warning, TEXT("Required keys not down || Got Stuck with something"));
		EndWallRun(EWallRunEndReason::EWRER_FallOfWall); // End wallrun Fall of from wall
		return;//we don't want below code if the condition satisfies
	}

	FVector EndTrace = FVector::CrossProduct(WallRunDirection, FVector(0, 0, (WallRunSide == EWallRunDirection::EWRD_Right ? 1 : -1)));
	
	/* using capsule radius to make sure that player will not continue wallrun if there is comes whose (X or Y) 
	doesn't alligns with the current wall and 20 is like error tolenrece */
	EndTrace = EndTrace * (GetCapsuleComponent()->GetScaledCapsuleRadius() + 20.f) + GetActorLocation();
	TArray<AActor *> ActorsToIgnore;
	FHitResult WallRunOutHit;
	bool bIsWallHit = UKismetSystemLibrary::SphereTraceSingle(this,
															GetActorLocation(),
															EndTrace,
															2.f,
															UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
															false,
															ActorsToIgnore,
															EDrawDebugTrace::None,
															WallRunOutHit,
															true);//using sphere trace so ingore small gaps between walls
	if (!bIsWallHit)
	{
		// UE_LOG(LogTemp, Warning, TEXT("No wall hit"));
		EndWallRun(EWallRunEndReason::EWRER_FallOfWall); // end wall run fall of from wall
		return;
	}

	EWallRunDirection LWallRunSide = FVector2D::DotProduct(UKismetMathLibrary::Conv_VectorToVector2D(WallRunOutHit.ImpactNormal),
														   UKismetMathLibrary::Conv_VectorToVector2D(GetActorRightVector())) > 0.f
										 ? EWallRunDirection::EWRD_Right
										 : EWallRunDirection::EWRD_Left;
	if (LWallRunSide != WallRunSide)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Wall run side not equal"));
		EndWallRun(EWallRunEndReason::EWRER_FallOfWall); // end wall run fall of from wall
		return;
	}
	WallRunSoundComp->SetWorldLocation(WallRunOutHit.Location);
	WallRunDirection = FVector::CrossProduct(WallRunOutHit.ImpactNormal, FVector(0, 0, (WallRunSide == EWallRunDirection::EWRD_Left ? -1 : 1)));
	FVector WallRunVelocity = FVector(WallRunDirection * CharacterMovement->GetMaxSpeed());
	WallRunVelocity.Z = 0;
	CharacterMovement->Velocity = FVector(WallRunVelocity);
}

void APlayerCharacter::EndWallRun(EWallRunEndReason WallRunEndReason)
{
	// UE_LOG(LogTemp, Warning, TEXT("End wall run"));
	if(WallRunSoundComp)
	{
		WallRunSoundComp->Stop();
	}
	bIsWallRunning = false;
	SetMovementSpeed(EMovementType::EMT_Walking);
	JumpsLeft = (WallRunEndReason == EWallRunEndReason::EWRER_FallOfWall) ? 1 : MaxJumps;
	CharacterMovement->GravityScale = 1.f;
	CharacterMovement->SetPlaneConstraintNormal(FVector::ZeroVector);
	CameraTiltTimeline.Reverse();
	bCanDoWallRunAgain = false;
	WallRunUpdateTimeline.Stop();
	FTimerHandle LTimerHandle;
	float WallRunReEnableTime = (WallRunEndReason == EWallRunEndReason::EWRER_JumpOfWall) ? 0.5 : 1;
	GetWorld()->GetTimerManager().SetTimer(LTimerHandle, 
										   [&](){ bCanDoWallRunAgain = true; },
										   WallRunReEnableTime, false);
}
//===================================================================Wall run section end ======================================//

//==============================================================Enable and disable Player Input Section============//

void APlayerCharacter::EnablePlayerInput()
{
	EnableInput(GetWorld()->GetFirstPlayerController());
	bIsDoingLedgeGrab = false;
	if(CurrentWeapon)
	{
		CurrentWeapon->DrawWeapon(1);
	}
}

void APlayerCharacter::DisablePlayerInput()
{
	DisableInput(GetWorld()->GetFirstPlayerController());
}
//============================================================================Enable Disable Input Section End=============================//

//=====================================================================Input Axis Section==========================//

void APlayerCharacter::MoveForward(float AxisValue)
{
	ForwardAxis = AxisValue;

	if (CanPerformCertainMovement(EMovementType::EMT_Walking))
	{
		AddMovementInput(GetActorForwardVector(), AxisValue);
	}
}

void APlayerCharacter::MoveRight(float AxisValue)
{
	RightAxis = AxisValue;
	HandSway(RightAxis);
	AddMovementInput(GetActorRightVector(), AxisValue);
}

void APlayerCharacter::Turn(float AxisValue)
{
	SmoothHorizontalLook = UKismetMathLibrary::FInterpTo(SmoothHorizontalLook, AxisValue, UGameplayStatics::GetWorldDeltaSeconds(this), 30.f);
	float FinalTurn = bIsADS ? SmoothHorizontalLook/2 : SmoothHorizontalLook;
	AddControllerYawInput(FinalTurn);
	float SwayValue = FMath::Clamp<float>(AxisValue,-1.0f,1.0f);
	HandSway(SwayValue);
	// weapon sway for more visual smoothness
	float LTurnInterpSpeed;
	if (bIsADS)
	{
		LTurnInterpSpeed = TurnInterpSpeedADS;
	}
	else
	{
		LTurnInterpSpeed = TurnInterpSpeed;
	}
	if(bIsADS)
	{
		return;
	}
	PlayerMesh->SetRelativeRotation(FRotator(PlayerMesh->GetRelativeRotation().Pitch,
											 PlayerMesh->GetRelativeRotation().Yaw - AxisValue,
											 PlayerMesh->GetRelativeRotation().Roll));

	FRotator NewRotation = FRotator(PlayerMesh->GetRelativeRotation().Pitch,
									Camera->GetRelativeRotation().Yaw - 90,
									PlayerMesh->GetRelativeRotation().Roll);

	// NewRotation = bIsADS? FRotator(0,0,0): NewRotation;
	PlayerMesh->SetRelativeRotation(FMath::RInterpTo(PlayerMesh->GetRelativeRotation(),
													 NewRotation,
													 UGameplayStatics::GetWorldDeltaSeconds(this),
													 LTurnInterpSpeed));
}

void APlayerCharacter::LookUp(float AxisValue)
{
	SmoothVerticalLook = UKismetMathLibrary::FInterpTo(SmoothVerticalLook, AxisValue, UGameplayStatics::GetWorldDeltaSeconds(this), 30.f);
	float FinalLook = bIsADS ? SmoothVerticalLook/2 : SmoothVerticalLook;
	AddControllerPitchInput(FinalLook);

	// // weapon sway for more visual smoothness
	float LLookupInterpSpeed;
	if (bIsADS)
	{
		LLookupInterpSpeed = LookInterpSpeedADS;
	}
	else
	{
		LLookupInterpSpeed = LookInterpSpeed;
	}
	
	if(bIsADS)
	{
		return;
	}
	PlayerMesh->SetRelativeRotation(FRotator(PlayerMesh->GetRelativeRotation().Pitch - AxisValue,
											 PlayerMesh->GetRelativeRotation().Yaw,
											 PlayerMesh->GetRelativeRotation().Roll));

	FRotator NewRotation = FRotator(Camera->GetRelativeRotation().Yaw,
									PlayerMesh->GetRelativeRotation().Yaw,
									PlayerMesh->GetRelativeRotation().Roll);

	PlayerMesh->SetRelativeRotation(FMath::RInterpTo(PlayerMesh->GetRelativeRotation(),
													 NewRotation,
													 UGameplayStatics::GetWorldDeltaSeconds(this),
													 LLookupInterpSpeed));
}

void APlayerCharacter::HandSway(float AxisValue)
{
	float AdsAlpha = bIsADS ? 0 : 1;
	float SwayValue = AxisValue * AdsAlpha * HandSwayRotationValue;
	FRotator SwayRotationTarget = FRotator(SwayValue,0,0);
	SwayRotation = FMath::RInterpTo(SwayRotation,SwayRotationTarget,UGameplayStatics::GetWorldDeltaSeconds(this),SwaySpeed);
}

//===========================================================Input Axis Section End==========================================//

//========================================================================Jumping==================================//

void APlayerCharacter::Jump()
{
	if (GetCurrentMovementType() == EMovementType::EMT_Crouching)//if the player is croucing it will uncrouch
	{
		Crouch();
	}
	else
	{
		SlideDirection = GetActorForwardVector();//cancle the slide if player is sliding
		if (SlideTimeline.IsPlaying())
		{
			StopSliding();
			if(CanUncrouch(CrouchCapsuleHeight))
			{
				SetMovementSpeed(EMovementType::EMT_Walking);
				Crouch();
			}
			else
			{
				SetMovementSpeed(EMovementType::EMT_Crouching);
				return;
			}
			
		}
		if(JumpsLeft == 1)
		{
			UGameplayStatics::PlaySoundAtLocation(this,JumpJetPackSound,GetActorLocation());
		}
		if(JumpsLeft == 2)
		{
			UGameplayStatics::PlaySoundAtLocation(this,JumpSound,GetActorLocation());
		}
		if (bIsWallRunning)
		{
			SetMovementSpeed(EMovementType::EMT_Walking);
			EndWallRun(EWallRunEndReason::EWRER_JumpOfWall);
			LaunchCharacter(FindLaunchVelocity(), false, true);
		}
		else if (CharacterMovement->IsFalling() && JumpsLeft > 0)
		{
			JumpsLeft -= 2;
			LaunchCharacter(FindLaunchVelocity(), false, true);
		}
		else if (JumpsLeft > 0)
		{
			JumpsLeft--;
			LaunchCharacter(FindLaunchVelocity(), false, true);
		}
	}
}

void APlayerCharacter::StopJump()
{
	StopJumping();
}

void APlayerCharacter::Landed(const FHitResult &Hit)
{
	Super::Landed(Hit);
	JumpsLeft = MaxJumps;
	if (bIsCrouched && UKismetMathLibrary::VSize(GetVelocity()) > 850 && GetVelocity().X != 0.f && GetVelocity().Y != 0.f)
	{
		Slide(SlideDirection, "Landed");
	}
}

FVector APlayerCharacter::FindLaunchVelocity() const
{
	FVector LaunchDirection = FVector::ZeroVector;

	if (bIsWallRunning)
	{
		//Just to offect the launch direction to the right vector depending upon the wallrun side 
		LaunchDirection = FVector::CrossProduct(WallRunDirection, FVector(0, WallRunSide == EWallRunDirection::EWRD_Right ? 5 : -5, 0));
	}

	FVector FinalLaunchVelocity = (LaunchDirection + FVector(0, 0, 1)) * CharacterMovement->JumpZVelocity;
	return FinalLaunchVelocity;
}

//=========================================================Jumping End===================================================//

//========================================================Crouch===================================//

void APlayerCharacter::Crouch()//right now its a toggle funciton,
{
	if (bIsCrouched && CanUncrouch(CrouchCapsuleHeight))
	{
		bIsCrouched = false;
		StopSliding();
		CrouchTimeline.Reverse();
		// UE_LOG(LogTemp, Warning, TEXT("Uncrouch"));
		if (bIsSprintKeyDown)
		{
			Sprint();
		}
		else
		{
			SetMovementSpeed(EMovementType::EMT_Walking);
		}
	}
	/*if sprinting or faling with a certain forward velocity the player will slide in the forward direction and 
	if falling then will slide in the direction stored just after leaving the ground*/
	else if (CanPerformCertainMovement(EMovementType::EMT_Sliding) && !CharacterMovement->IsFalling())
	{
		Slide(GetActorForwardVector(), "Crouch");
	}
	else if (CanPerformCertainMovement(EMovementType::EMT_Crouching))
	{
		bIsCrouched = true;
		CrouchTimeline.Play();
	}
}

void APlayerCharacter::ToggleCrouch()
{
	float Alpha = CrouchCurve->GetFloatValue(CrouchTimeline.GetPlaybackPosition());
	GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::Lerp(StandingCapsuleHeight, CrouchCapsuleHeight, Alpha));

	Camera->SetRelativeLocation(FVector(Camera->GetRelativeLocation().X,
										Camera->GetRelativeLocation().Y,
										FMath::Lerp(StandingCameraHeight, CrouchCameraHeight, Alpha)));
}

// doing a sphere trace slightly smaller that the capsule radius in the upward direcion to check if player can uncrouch or not
bool APlayerCharacter::CanUncrouch(float CapsuleHeightAlpha) const
{
	float TraceRadius = Capsule->GetUnscaledCapsuleRadius() - 1;
	FVector TraceEnd = FVector(0, 0, (StandingCapsuleHeight + (StandingCapsuleHeight - CapsuleHeightAlpha - TraceRadius))) *
						   GetActorUpVector() +
					   GetActorLocation();
	TArray<AActor *> ActorsToIgnore;
	FHitResult OutHit;
	bool IsHit = UKismetSystemLibrary::SphereTraceSingle(
		this,
		GetActorLocation(),
		TraceEnd,
		TraceRadius,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHit,
		true);
	return !IsHit;
}

//=========================================================================Slide===================================//

void APlayerCharacter::Slide(FVector LSlideDirection, FString Caller)
{
	// UE_LOG(LogTemp, Warning, TEXT("Caller = %s"), *Caller);//just for testing by whome this function is getting called
	if (LSlideDirection == FVector::ZeroVector)
	{
		return;
	}
	SetMovementSpeed(EMovementType::EMT_Sliding);
	SlideTimeline.SetPlayRate(1);
	bIsCrouched = true;
	CrouchTimeline.Play();
	SlideDirection = LSlideDirection;
	SlideTimeline.PlayFromStart();
	SlideSoundComp = UGameplayStatics::SpawnSoundAttached(SlideSound,Capsule);
}

void APlayerCharacter::PerformSlide()
{

	//Calculaing the angle between the player and the walking surface to increase/decrease the sliding timing based on the surface angle
	float ZPoint = GetActorLocation().Z - Capsule->GetUnscaledCapsuleHalfHeight();
	FVector StartTrace = FVector(GetActorLocation().X, GetActorLocation().Y , ZPoint + 20.f );
	FVector EndTrace = FVector(GetActorLocation().X, GetActorLocation().Y , ZPoint - 40.f );
	TArray<AActor*> ActorsToIgnore;
	FHitResult SlideHitResult;
	bool bSlideTraceHit = UKismetSystemLibrary::LineTraceSingle(this,
																StartTrace, 
																EndTrace, 
																UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
																false,
																ActorsToIgnore,
																EDrawDebugTrace::ForDuration,
																SlideHitResult,
																true);
	if(bSlideTraceHit)
	{
		float DotProduct = 1 + FVector::DotProduct(GetActorForwardVector(),SlideHitResult.ImpactNormal) * SlideTimelinePlaybackAlpha ;
		SlideTimeline.SetPlayRate(DotProduct);
	}

	float Alpha = SlideCurve->GetFloatValue(SlideTimeline.GetPlaybackPosition());
	if(SlideSoundComp)
	{
		SlideSoundComp->VolumeMultiplier = 1 - Alpha;
	}
	CharacterMovement->MaxWalkSpeed = FMath::Lerp(SlideSpeed, CrouchSpeed, Alpha);
	//UE_LOG(LogTemp, Warning, TEXT("Slide Direction = %s"), *SlideDirection.ToString());
	AddMovementInput(SlideDirection, 1);
	if (GetVelocity().Size() < 10)
	{
		StopSliding();
		SetMovementSpeed(EMovementType::EMT_Crouching);
		// UE_LOG(LogTemp, Warning, TEXT("Sliding stopped due to 0 velocity"));
	}
}

void APlayerCharacter::EndSlide()
{
	SetMovementSpeed(EMovementType::EMT_Crouching);//end result of the slide...
	if(SlideSoundComp)
	{
		SlideSoundComp->Stop();
	}
}

void APlayerCharacter::StopSliding()
{
	SlideTimeline.Stop();
	if(SlideSoundComp)
	{
		SlideSoundComp->Stop();
	}
}
//======================================================================Slide End================================================//

//======================================================================Sprint start===============================//

void APlayerCharacter::Sprint()
{
	bIsSprintKeyDown = true;
	if (GetCurrentMovementType() == EMovementType::EMT_Crouching )
	{
		bIsSprintKeyDown = false;
		Crouch();
	}
	bIsSprintKeyDown = true;
	CanPerformCertainMovement(EMovementType::EMT_Sprinting);
}

void APlayerCharacter::CancelSprint()
{
	bIsSprintKeyDown = false;
	if (GetCurrentMovementType() == EMovementType::EMT_Sprinting)
	{
		SetMovementSpeed(EMovementType::EMT_Walking);
	}
}

//==================================================================Sprint End===================================================//

//============================================================================Player Movement Helper Methods Start=================//

void APlayerCharacter::SetMovementSpeed(EMovementType MovementType)
{
	switch (MovementType)
	{
	case EMovementType::EMT_Walking:
		CharacterMovement->MaxWalkSpeed = bIsADSButtonDown ? WalkSpeed - 200 : WalkSpeed;
		CurrentMovementType = MovementType;
		break;

	case EMovementType::EMT_Sprinting:
		CharacterMovement->MaxWalkSpeed = bIsADSButtonDown ? WalkSpeed - 200 : SprintSpeed;
		CurrentMovementType = MovementType;
		break;

	case EMovementType::EMT_Crouching:
		CharacterMovement->MaxWalkSpeed = bIsADSButtonDown ? CrouchSpeed - 100 : CrouchSpeed;
		CurrentMovementType = MovementType;
		break;

	case EMovementType::EMT_Sliding:
		CharacterMovement->MaxWalkSpeed = SlideSpeed;
		CurrentMovementType = MovementType;
		break;

	case EMovementType::EMT_WallRun:
		CharacterMovement->MaxWalkSpeed = bIsADSButtonDown ? WallRunSpeed - 200 : WallRunSpeed;
		CurrentMovementType = MovementType;
		break;
	}
}

EMovementType APlayerCharacter::GetCurrentMovementType()
{
	return CurrentMovementType;
}

//this function return true/false if player can perform feeded movement and also sets the movement speed it the condition satifies
bool APlayerCharacter::CanPerformCertainMovement(EMovementType Movement)
{
	EMovementType LCurrentMovementType = GetCurrentMovementType();
	switch (Movement)
	{
	case EMovementType::EMT_Walking:
		if (ForwardAxis <= 0 && GetCurrentMovementType() == EMovementType::EMT_Sprinting)
		{
			CancelSprint();
		}
		if (ForwardAxis != 0)
		{
			if (GetCurrentMovementType() == EMovementType::EMT_Sliding && ForwardAxis < 0)
			{
				SlideTimeline.SetPlayRate(3);//increasing the play rate to make player stop sliding but with some smooth looks
			}
			if (GetCurrentMovementType() == EMovementType::EMT_Sliding)
			{
				if (FVector::DotProduct(GetActorForwardVector(), SlideDirection) < 0) // player will stop sliding if he turns back during sliding
				{
					SlideTimeline.SetPlayRate(3);
				}
			}

			return true;
		}
		else
		{
			return false;
		}
		break;

	case EMovementType::EMT_Sprinting:
		if (LCurrentMovementType != EMovementType::EMT_WallRun && ForwardAxis > 0 && LCurrentMovementType != EMovementType::EMT_Crouching)
		{
			if (SlideTimeline.IsPlaying())
			{
				if(!CanUncrouch(CrouchCapsuleHeight))
				{
					return false;
				}
				StopSliding();
				Crouch();
			}
			SetMovementSpeed(Movement);
			return true;
		}
		else
		{
			return false;
		}
		break;

	case EMovementType::EMT_Crouching:
		if (LCurrentMovementType != EMovementType::EMT_Sliding && LCurrentMovementType != EMovementType::EMT_WallRun)
		{
			SetMovementSpeed(Movement);
			return true;
		}
		else
		{
			return false;
		}
		break;

	case EMovementType::EMT_Sliding:
		if (LCurrentMovementType != EMovementType::EMT_Walking &&
			LCurrentMovementType != EMovementType::EMT_WallRun &&
			LCurrentMovementType != EMovementType::EMT_Crouching &&
			LCurrentMovementType != EMovementType::EMT_Sliding &&
			!CharacterMovement->IsFalling() &&
			!bIsADSButtonDown)
		{
			SetMovementSpeed(Movement);
			return true;
		}
		else
		{
			return false;
		}
		break;

	case EMovementType::EMT_WallRun:
		SetMovementSpeed(Movement);
		bIsWallRunning = true;
		return true;
	}
	return false;
}

bool APlayerCharacter::IsInAir()
{
	return CharacterMovement->IsFalling();
}

UCameraComponent *APlayerCharacter::GetCamera()
{
	return Camera;
}

USkeletalMeshComponent *APlayerCharacter::GetMesh()
{
	return PlayerMesh;
}

bool APlayerCharacter::IsADSButtonDown()
{
	return bIsADSButtonDown;
}
//==================================================================Player Movement Helper Methods End===============================//

//=====================================================================Weapons Section=============================//

void APlayerCharacter::Shoot()
{
	bIsShootButtonDown = true;
	if (CurrentWeapon == nullptr)
	{
		// UE_LOG(LogTemp, Warning, TEXT("NO weapon base reference"));
		return;
	}
	CurrentWeapon->Shoot();
}

void APlayerCharacter::StopShooting()
{
	bIsShootButtonDown = false;
	if (CurrentWeapon == nullptr)
	{
		// UE_LOG(LogTemp, Warning, TEXT("NO weapon base reference"));
		return;
	}
	CurrentWeapon->StopShooting();
}

void APlayerCharacter::ADSON()
{
	if(CurrentWeapon)
	{
		if(CurrentWeapon->bIsWeaponShootable)
		{
			bIsADSButtonDown = true;
			if (!bIsDoingMeleeAttack && !bIsReloading && !bIsThrowing)
			{
				SetMovementSpeed(GetCurrentMovementType());
				ADS_OnTimeline.PlayFromStart();
			}
		}	
	}
	
}

void APlayerCharacter::ADSOnInAction()
{
	bIsADS = true;
	float Alpha = CrouchCurve->GetFloatValue(ADS_OnTimeline.GetPlaybackPosition());
	ADS_Alpha = Alpha;
	PerformADS(ADSFOV, 1.3f, Alpha);
}

void APlayerCharacter::	ADSOFF()
{
	if(!bIsADSButtonDown)
	{
		return;
	}
	if(CurrentWeapon)
	{
		if(CurrentWeapon->bIsWeaponShootable)
		{
		bIsADSButtonDown = false;
		if(!bIsReloading && !bIsThrowing)
		{
			SetMovementSpeed(GetCurrentMovementType());
			ADS_OffTimeline.PlayFromStart();
		}
		}
	}
}

void APlayerCharacter::ADSOffInAction()
{
	bIsADS = false;
	float Alpha = CrouchCurve->GetFloatValue(ADS_OffTimeline.GetPlaybackPosition());
	ADS_Alpha = Alpha;
	PerformADS(DefaultFOV, 0.5f, Alpha);
}

void APlayerCharacter::PerformADS(float FinalAdsFovValue, float NewVignetteIntensity, float Alpha)
{
	FTransform NewTransform = UKismetMathLibrary::MakeRelativeTransform(CurrentWeapon->GunMesh->GetSocketTransform(FName("ADS")),PlayerMesh->GetComponentTransform());
	NewTransform = UKismetMathLibrary::InvertTransform(NewTransform);

	FTransform FinalTransform;
	if(bIsADS)
	{
		FinalTransform = UKismetMathLibrary::TLerp(PlayerMesh->GetRelativeTransform(),NewTransform,Alpha);
	}
	else
	{
		FinalTransform = UKismetMathLibrary::TLerp(NewTransform,PlayerMeshTransform,Alpha);
	}
	// FTransform tempPlayerTransform = bIsADS ? PlayerMesh->GetRelativeTransform() : PlayerMeshTransform;
	
	PlayerMesh->SetRelativeTransform(FinalTransform);

	float newFOV = FMath::Lerp(Camera->FieldOfView, FinalAdsFovValue, Alpha);
	Camera->SetFieldOfView(newFOV);
	Camera->PostProcessSettings.bOverride_VignetteIntensity = true;
	Camera->PostProcessSettings.VignetteIntensity = FMath::Lerp(Camera->PostProcessSettings.VignetteIntensity, NewVignetteIntensity, Alpha);
}
void APlayerCharacter::Reload()
{
	if(!CurrentWeapon)
	{
		return;
	}
	if(!(CurrentWeapon->CurrentMagAmmo < CurrentWeapon->MagSize) || bIsThrowing)
	{
		return;
	}
	bIsShooting = false;
	if (bIsADSButtonDown)
	{
		ADS_OffTimeline.PlayFromStart();
	}
	if (!bIsDoingMeleeAttack && CurrentWeapon)
	{
		CurrentWeapon->Reload();
	}
}

void APlayerCharacter::	PlayCameraShake(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale)
{
	APlayerCameraManager *PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	PlayerCameraManager->StartCameraShake(ShakeClass, Scale);
}

void APlayerCharacter::Pickup()
{
	bCanPickup = true;
}

void APlayerCharacter::NoPickup()
{
	bCanPickup = false;
}

void APlayerCharacter::PickupInAction()
{
	if(bIsThrowing)
	{
		bIsWeaponHit = false;
		return;
	}
	FVector EndTrace = GetActorLocation() + GetActorUpVector() * -100;
	FHitResult PickupHit;
	TArray<AActor *> ActorsToIgnore;

	//all the pickables are given pickup collisoin channel so that pickup sphere trace will hit only those its which have pickup collision channel
	TArray<TEnumAsByte<EObjectTypeQuery>> PickupObject;
	PickupObject.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel2));
	bool bIsPickupHit = UKismetSystemLibrary::SphereTraceSingleForObjects(this,
																		  GetActorLocation() + 20.f,
																		  EndTrace,
																		  45,
																		  PickupObject,
																		  false,
																		  ActorsToIgnore,
																		  EDrawDebugTrace::None,
																		  PickupHit,
																		  true);

	if (bIsPickupHit)
	{
		AActor *HitActor = PickupHit.GetActor();
		if (HitActor != nullptr)
		{
			PickupWeaponInterface = Cast<IPickupWeaponInterface>(HitActor);//The pickup item has an interface to avoid casting to each pickup item
			
			if (PickupWeaponInterface != nullptr)
			{
				PickupWeaponInterface->SetPickupWeaponName();
				bIsPickupGun = PickupWeaponInterface->IsPickupGun();//Currently there are two types of pickables (gun and throwables)

				if (bIsPickupGun) 
				{
					PickupGun(HitActor);
				}
				else
				{
					PickupThrowable(HitActor);
				}
			}

			else
			{
				// UE_LOG(LogTemp, Warning, TEXT("Pickup weapon has no interface"));
				PickupWeaponName = EWeaponName::EWN_None;
			}
		}
	}
	else
	{
		bIsWeaponHit = false;
		PickupWeaponName = EWeaponName::EWN_None;
	}
}

void APlayerCharacter::PickupGun(AActor* HitActor)
{
	//Checking if the player dosen't holds the pickup item
	if (UKismetMathLibrary::NotEqual_ClassClass(PrimaryWeapon.WeaponClass.Get(), HitActor->GetClass()) &&
		UKismetMathLibrary::NotEqual_ClassClass(SecondaryWeapon.WeaponClass.Get(), HitActor->GetClass()))
	{
		bIsWeaponHit = true;//for the Hud to display pickup text
		if (bCanPickup)
		{
			if(!bCanPickupWeapon)
			{
				return;
			}
				GetWorld()->GetTimerManager().SetTimer(WeaponPickupTimerHandle,[&](){bCanPickupWeapon = true;},.5,false);
			bCanPickupWeapon = false;
			bIsWeaponHit = false;
			PickupHitWeapon = HitActor;
			bCanPickup = false;
			PickupWeaponInterface->PickupWeapon();
		}
	}
	else // Weapon Already in the slot
	{
		if (UKismetMathLibrary::EqualEqual_ClassClass(PrimaryWeapon.WeaponClass.Get(), HitActor->GetClass()))
		{
			if (PrimaryWeapon.TotalAmmo < PrimaryWeapon.MaxAmmo)//will shift this function to the weaponBase to give different weapons different ammo
			{
				if(!EqPrimaryWeapon)
				{
					return;
				}
				PrimaryWeapon.TotalAmmo = FMath::Min(PrimaryWeapon.TotalAmmo + EqPrimaryWeapon->PickupAmmoCount, PrimaryWeapon.MaxAmmo);
				if(GunPickupSound)
				{
					UGameplayStatics::PlaySound2D(this,GunPickupSound);
				}
				EqPrimaryWeapon->TotalAmmo = PrimaryWeapon.TotalAmmo;
				HitActor->Destroy();
			}
		}
		else if (UKismetMathLibrary::EqualEqual_ClassClass(SecondaryWeapon.WeaponClass.Get(), HitActor->GetClass()))
		{
			if (SecondaryWeapon.TotalAmmo < SecondaryWeapon.MaxAmmo)
			{
				if(!EqSecondaryWeapon)
				{
					return;
				}
				SecondaryWeapon.TotalAmmo = FMath::Min((SecondaryWeapon.TotalAmmo + EqSecondaryWeapon->PickupAmmoCount), SecondaryWeapon.MaxAmmo);
				if(GunPickupSound)
				{
					UGameplayStatics::PlaySound2D(this,GunPickupSound);
				}
				EqSecondaryWeapon->TotalAmmo = SecondaryWeapon.TotalAmmo;
				HitActor->Destroy();
			}
		}
	}
}

void APlayerCharacter::PickupThrowable(AActor* HitActor)
{
	
	// item not in the slot
	if (UKismetMathLibrary::NotEqual_ClassClass(PrimaryThrowableData.BP_Throwable.Get(), HitActor->GetClass()) &&
		UKismetMathLibrary::NotEqual_ClassClass(SecondaryThrowableData.BP_Throwable.Get(), HitActor->GetClass()))
	{
		bIsWeaponHit = true;
		if (bCanPickup)
		{
			bCanPickup = false;
			bIsWeaponHit = false;
			PickupWeaponInterface->PickupWeapon();
			if(ThrowablePickupSound)
				{
					UGameplayStatics::PlaySound2D(this,ThrowablePickupSound);
				}
		}
	}
	else
	{
		// UE_LOG(LogTemp, Warning, TEXT("Throwable in slot"));
		if (UKismetMathLibrary::EqualEqual_ClassClass(PrimaryThrowableData.BP_Throwable.Get(), HitActor->GetClass()))
		{
			if (PrimaryThrowableData.Count < ThrowableMaxCount)
			{
				PrimaryThrowableData.Count++;
				if(ThrowablePickupSound)
				{
					UGameplayStatics::PlaySound2D(this,ThrowablePickupSound);
				}
				HitActor->Destroy();
			}
		}
		else if (UKismetMathLibrary::EqualEqual_ClassClass(SecondaryThrowableData.BP_Throwable.Get(), HitActor->GetClass()))
		{
			if (SecondaryThrowableData.Count < ThrowableMaxCount)
			{
				SecondaryThrowableData.Count++;
				if(ThrowablePickupSound)
				{
					UGameplayStatics::PlaySound2D(this,ThrowablePickupSound);
				}
			}
				HitActor->Destroy();
		}
	}
}


void APlayerCharacter::EquipPrimaryWeapon()
{
	if(!bCanSwitchWeapon)
	{
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(WeaponSwitchTimerHandle,[&](){bCanSwitchWeapon = true;},.1,false);
	bCanSwitchWeapon = false;

	if (WeaponEquippedSlot == 1)
	{
		if (!(PrimaryWeapon.WeaponClass == nullptr))//will not switch to this weapon if its empty
		{
			WeaponEquippedSlot = 0;
			SwitchWeapon(false);
		}
	}
}

void APlayerCharacter::EquipSecondaryWeapon()
{
	if(!bCanSwitchWeapon)
	{
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(WeaponSwitchTimerHandle,[&](){bCanSwitchWeapon = true;},.1,false);
	bCanSwitchWeapon = false;

	if (WeaponEquippedSlot == 0)
	{
		if (!(SecondaryWeapon.WeaponClass == nullptr))
		{
			WeaponEquippedSlot = 1;
			SwitchWeapon(false);
		}
	}
}

void APlayerCharacter::SwitchWeapon(bool bIsPickupWeapon)
{
	if(bIsADSButtonDown)
	{
		ADSOFF();
	}
	UE_LOG(LogTemp,Warning,TEXT(" weapon switch"));


	if (WeaponEquippedSlot == 0)
	{
		SetWeaponVars(PrimaryWeapon, true,bIsPickupWeapon);
	}
	else if (WeaponEquippedSlot == 1)
	{
		SetWeaponVars(SecondaryWeapon, false,bIsPickupWeapon);
	}
}

void APlayerCharacter::SetWeaponVars(FWeaponData NewWeaponData, bool bIsPrimaryWeapon,bool bIsPickupWeapon)//function called by the weapon base when picking the weapon 
{
	// UE_LOG(LogTemp,Warning,TEXT("Setting weapon vars for %i"),bIsPrimaryWeapon);
	if (NewWeaponData.WeaponClass != nullptr)
	{
		/*this check is here to check if we are manually switch the weapon(false) or 
		its getting switched while picking(true) it up as in manual switch we are not casting to that weapon*/
		if(bIsPickupWeapon)
		{
			/*we are saving the primary weapon and secondary weapon in thier own seprate variables 
			so that we don't to cast to the weapon everytime we switch the weapon, we just need to set the value of the CurrentWeapon
			depending on which weapon we wanna use and at this time we will make it visible and hide the other weapon*/
			if(bIsPrimaryWeapon)
			{
				//change the primary weapon
				// UE_LOG(LogTemp,Warning,TEXT("Equipping the primary Weapon"));
				EqPrimaryWeapon = Cast<AWeaponBase>(PickupHitWeapon);
				CurrentWeapon = EqPrimaryWeapon;
				if(EqSecondaryWeapon)
				{
					EqSecondaryWeapon->BackToHolster();
				}
			}
			else
			{
				//change the secondary weapon
				// UE_LOG(LogTemp,Warning,TEXT("Equipping the secondary Weapon"));
				EqSecondaryWeapon = Cast<AWeaponBase>(PickupHitWeapon);
				CurrentWeapon = EqSecondaryWeapon;
				if(EqPrimaryWeapon)
				{
					EqPrimaryWeapon->BackToHolster();
				}

			}
		}
		else
		{
			if(bIsPrimaryWeapon)
			{
				EqSecondaryWeapon->BackToHolster();
				EqPrimaryWeapon->SetActorHiddenInGame(false);
				CurrentWeapon = EqPrimaryWeapon;	
			}
			else
			{
				EqPrimaryWeapon->BackToHolster();
				EqSecondaryWeapon->SetActorHiddenInGame(false);
				CurrentWeapon = EqSecondaryWeapon;
			}
		}
		if (CurrentWeapon == nullptr)
		{
			// UE_LOG(LogTemp, Warning, TEXT("NO weapon reference"));
			return;
		}
		// UE_LOG(LogTemp,Warning,TEXT("Pickup Weapon Name = %s"),*PickupHitWeapon->GetName());
		//Also we are not spawning the weapon, as the pickup weapon and firing weapons are same, this way we can get away from spawning 
		if(bIsADSButtonDown)
		{
			ADSOFF();
		}
		PickupHitWeapon->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("WeaponSocket_r"));
		CurrentWeapon->bIsPrimaryWeapon = bIsPrimaryWeapon;
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->SetActorHiddenInGame(false);
		CurrentWeapon->DrawWeapon();
	}
	else
	{
		// UE_LOG(LogTemp, Warning, TEXT("New weapon data has a null weapon class"));
	}
}

void APlayerCharacter::DropWeapon(bool bIsPrimaryDrop)
{
	if(bIsPrimaryDrop)
	{
		if(EqPrimaryWeapon)
		{
			EqPrimaryWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			EqPrimaryWeapon->SetActorScale3D(EqPrimaryWeapon->GetActorScale3D()/SceneComp->GetComponentScale());
			if(PickupHitWeapon)
			{
				EqPrimaryWeapon->SetActorLocation(PickupHitWeapon->GetActorLocation());
				EqPrimaryWeapon->SetActorRotation(PickupHitWeapon->GetActorRotation());
			}
			EqPrimaryWeapon->DropGun();
		}
	}
	else
	{
		if(EqSecondaryWeapon)
		{
			EqSecondaryWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			EqSecondaryWeapon->SetActorScale3D(EqPrimaryWeapon->GetActorScale3D()/SceneComp->GetComponentScale());
			if(PickupHitWeapon)
			{
				EqSecondaryWeapon->SetActorLocation(PickupHitWeapon->GetActorLocation());
				EqSecondaryWeapon->SetActorRotation(PickupHitWeapon->GetActorRotation());
			}
			EqSecondaryWeapon->DropGun();
		}

	}
}


void APlayerCharacter::ReEquipAfterGrenade()
{
	bCanPredictPath = false;
	if(GetMesh()->GetAnimInstance()->Montage_IsPlaying(GrenadeHoldMontage))
	{
	GetMesh()->GetAnimInstance()->Montage_Stop(0,GrenadeHoldMontage);
	}	
	if(CurrentWeapon)
	{
		CurrentWeapon->DrawWeapon();
	}
}

void APlayerCharacter::MeleeAttack()
{
	if(!CurrentWeapon)
	{
		return;
	}
	if (!bIsDoingMeleeAttack && !bIsReloading && !bIsADS && !bIsThrowing)
	{
		bIsDoingMeleeAttack = true;
		float MeleeEndTime = CurrentWeapon->DoMeleeAttack();
		MeleeEndTime = MeleeEndTime == 0 ? 0.1 : MeleeEndTime;
		FTimerHandle MeleeTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			MeleeTimerHandle, [&]()
			{ bIsDoingMeleeAttack = false; },
			MeleeEndTime, false);
	}
}

void APlayerCharacter::MeleeAttackInAction()
{
	FVector EndTrace = Camera->GetComponentLocation() + Camera->GetForwardVector() * 100;
	TArray<AActor *> ActorsToIgnore;
	FHitResult MeleeOutHit;
	bool bIsMeleeHit = UKismetSystemLibrary::SphereTraceSingle(this,
															   Camera->GetComponentLocation(),
															   EndTrace,
															   10.f,
															   UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
															   false,
															   ActorsToIgnore,
															   EDrawDebugTrace::ForDuration,
															   MeleeOutHit,
															   true);
	if (bIsMeleeHit && MeleeOutHit.GetActor())
	{
		// apply damage and radial force
		UGameplayStatics::ApplyPointDamage(MeleeOutHit.GetActor(),20,MeleeOutHit.Location,MeleeOutHit,GetInstigatorController(),this,UDamageType::StaticClass());
	}
}
FWeaponData APlayerCharacter::GetWeaponData(bool bIsSecondaryWeapon)//function used by the save data
{
	return bIsSecondaryWeapon ? SecondaryWeapon : PrimaryWeapon;
}

//function used by the save data, here we will spawn the saved gun and then attach it to the player
void APlayerCharacter::SetWeaponData(bool bIsSecondaryWeapon,FWeaponData WeaponData)
{
	if(WeaponData.WeaponClass == nullptr)
	{
		// UE_LOG(LogTemp,Warning,TEXT("No Weapon Class"));
		return;
	}
	FActorSpawnParameters GunSpawnParams;
	GunSpawnParams.Owner = this;
	AWeaponBase* TempWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponData.WeaponClass,
																  GetActorLocation(),
																  GetActorRotation(),
																  GunSpawnParams);

	TempWeapon->GunMesh->SetSimulatePhysics(false);
	TempWeapon->GunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TempWeapon->CurrentMagAmmo = WeaponData.CurrentMagAmmo;
	TempWeapon->TotalAmmo = WeaponData.TotalAmmo;
	// UE_LOG(LogTemp,Error,TEXT("Max Ammo = %i"),WeaponData.MaxAmmo);
	PickupHitWeapon = TempWeapon;
	if(bIsSecondaryWeapon)
	{
		DropWeapon(false);
		SecondaryWeapon = WeaponData;
		WeaponEquippedSlot = 1;
	}
	else
	{
		DropWeapon(true);
		PrimaryWeapon = WeaponData;
		WeaponEquippedSlot = 0;
	}
	SwitchWeapon(true);
}
//================================================Throwable Stuff======================================================

void APlayerCharacter::SwitchThrowable()//flipflop funciton for switching the throwables
{
	if(bIsThrowing)
	{
		return;
	}
	if (ThrowableEquippedSlot == 0)
	{
		ThrowableEquippedSlot = 1;
	}
	else
	{
		ThrowableEquippedSlot = 0;
	}
}

FThrowableData APlayerCharacter::GetThrowableData(bool bIsSecondaryThrowable)
{
	return bIsSecondaryThrowable ? SecondaryThrowableData : PrimaryThrowableData ;
}

void APlayerCharacter::SetThrowableData(FThrowableData ThrowableData, bool bIsSecondaryThrowable)
{
	bIsSecondaryThrowable ? (SecondaryThrowableData = ThrowableData) : (PrimaryThrowableData = ThrowableData);
}

void APlayerCharacter::DropThrowable(bool IsPrimary)
{
	if(IsPrimary)
	{
		if(PrimaryThrowableData.Count > 0)
		{
			GetWorld()->SpawnActor<AThrowableBase>(PrimaryThrowableData.BP_Throwable,GetActorTransform());
		}
		PrimaryThrowableData.Count = 0;
		PrimaryThrowableData.BP_Throwable = nullptr;
	}
	else
	{
		if(SecondaryThrowableData.Count > 0)
		{
			GetWorld()->SpawnActor<AThrowableBase>(SecondaryThrowableData.BP_Throwable,GetActorTransform());
		}
		SecondaryThrowableData.Count = 0;
		SecondaryThrowableData.BP_Throwable = nullptr;
	}
}

void APlayerCharacter::PrimaryThrowStart()
{
	if(!bCanThrowAgain)
	{
		UE_LOG(LogTemp,Warning,TEXT("Cancel throw"));
		return;
	}
	bCanThrowAgain = false;
	bIsThrowableExploded = false;
	if (ThrowableEquippedSlot == 0 && PrimaryThrowableData.Count > 0)
	{
		bIsThrowing = true;
		PrimaryThrowable = StartThrow(PrimaryThrowableData.BP_Throwable);
		if(CurrentWeapon)
		{
			if(bIsADSButtonDown)
			{
				ADSOFF();
			}
			CurrentWeapon->BackToHolster();
			CurrentWeapon->SetCanShoot(false);
		}
		PlayerMesh->GetAnimInstance()->Montage_Play(GrenadeHoldMontage);
		// UE_LOG(LogTemp,Warning,TEXT("Throwing Primary"));
	}
	else if (ThrowableEquippedSlot == 1 && SecondaryThrowableData.Count > 0)
	{
		bIsThrowing = true;
		SecondaryThrowable = StartThrow(SecondaryThrowableData.BP_Throwable);
		if(CurrentWeapon)
		{
			if(bIsADSButtonDown)
			{
				ADSOFF();
			}
			CurrentWeapon->SetActorHiddenInGame(true);
			CurrentWeapon->SetCanShoot(false);
		}
		PlayerMesh->GetAnimInstance()->Montage_Play(GrenadeHoldMontage);
		// UE_LOG(LogTemp,Warning,TEXT("Throwing Primary"));

	}
}

void APlayerCharacter::PrimaryThrowEnd()
{
	if(!bIsThrowableSpawned)
	{
		UE_LOG(LogTemp,Warning,TEXT("Cancel throw"));
		return;
	}
	bIsThrowableSpawned = false;

	FTimerHandle ThrowTimer;
	if (PrimaryThrowable && ThrowableEquippedSlot == 0 && PrimaryThrowableData.Count > 0)
	{
		UE_LOG(LogTemp,Warning,TEXT("End Throw"));
		PrimaryThrowableData.Count--;
		// EndThrow(PrimaryThrowable);
		if(!PrimaryThrowable->bIsExploded)
		{
			float Time = PlayerMesh->GetAnimInstance()->Montage_Play(ThrowMontage);
			GetWorld()->GetTimerManager().SetTimer(ThrowTimer,this,&APlayerCharacter::ReEquipAfterGrenade,Time-0.2,false);
		}
	}
	else if (SecondaryThrowable && ThrowableEquippedSlot == 1 && SecondaryThrowableData.Count > 0)
	{
		SecondaryThrowableData.Count--;
		UE_LOG(LogTemp,Warning,TEXT("End Throw"));

		// EndThrow(SecondaryThrowable);
		if(!SecondaryThrowable->bIsExploded)
		{
			float Time = PlayerMesh->GetAnimInstance()->Montage_Play(ThrowMontage);
			GetWorld()->GetTimerManager().SetTimer(ThrowTimer,this,&APlayerCharacter::ReEquipAfterGrenade,Time-0.2,false);
		}
	}
}


void APlayerCharacter::InitiateEndThrow()
{
	if(ThrowableEquippedSlot == 0)
	{
		EndThrow(PrimaryThrowable);
	}
	else
	{
		EndThrow(SecondaryThrowable);
	}
}

AThrowableBase *APlayerCharacter::StartThrow(TSubclassOf<AThrowableBase> Throwable)
{
	AThrowableBase *SpawnedThrowable = nullptr;
	if (Throwable)
	{
		FActorSpawnParameters GrenadeSpawnParemeters;
		GrenadeSpawnParemeters.Owner = this;
		SpawnedThrowable = GetWorld()->SpawnActor<AThrowableBase>(Throwable, PlayerMesh->GetSocketLocation(FName("WeaponSocket_r")), FRotator::ZeroRotator,GrenadeSpawnParemeters);
		SpawnedThrowable->AttachToComponent(PlayerMesh,FAttachmentTransformRules::SnapToTargetIncludingScale,FName("WeaponSocket_r"));
		SpawnedThrowable->Initiate();
		bCanPredictPath = true;
		bIsThrowableSpawned = true;
	}
	return SpawnedThrowable;
}

void APlayerCharacter::EndThrow(AThrowableBase *CurrentThrowable)
{
	bCanPredictPath = false;
	if (CurrentThrowable)
	{
		CurrentThrowable->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentThrowable->Throw(ThrowVelocity);
		CurrentThrowable->SetActorScale3D(CurrentThrowable->GetActorScale3D()/SceneComp->GetComponentScale());
	}
	bIsThrowing = false;

}

// void APlayerCharacter::ThrowPredection()
// {
// 	FVector EndPoint = GetActorLocation() + ((GetActorForwardVector() + GetControlRotation().Vector()) * FMath::Pow(ThrowSpeed, 1.1));
// 	UGameplayStatics::SuggestProjectileVelocity_CustomArc(this,
// 														  ThrowVelocity,
// 														  PlayerMesh->GetSocketLocation(FName("WeaponSocket_r")),
// 														  EndPoint,
// 														  0.f,
// 														  0.9);
// 	// FPredictProjectilePathParams ProjectilePathParams;
// 	// ProjectilePathParams.StartLocation = PlayerMesh->GetSocketLocation(FName("WeaponSocket_r"));
// 	// ProjectilePathParams.LaunchVelocity = ThrowVelocity;
// 	// ProjectilePathParams.ProjectileRadius = 10.f;
// 	// ProjectilePathParams.bTraceWithChannel = true;
// 	// ProjectilePathParams.bTraceWithCollision = true;
// 	// ProjectilePathParams.TraceChannel = ECollisionChannel::ECC_WorldStatic;
// 	// ProjectilePathParams.ActorsToIgnore.Add(this);
// 	// ProjectilePathParams.ActorsToIgnore.Add(PrimaryThrowable);
// 	// ProjectilePathParams.ActorsToIgnore.Add(SecondaryThrowable);
// 	// ProjectilePathParams.DrawDebugType = EDrawDebugTrace::ForOneFrame;
// 	// ProjectilePathParams.SimFrequency = 30;
// 	// FPredictProjectilePathResult ProjectilePathResult;
// 	// bool bIsHit = UGameplayStatics::PredictProjectilePath(this,ProjectilePathParams,ProjectilePathResult);
	
// 	// for(int32 i = 0 ;i<ProjectilePathResult.PathData.Num();i++)
// 	// {
// 	// 	PredictionSpline->AddSplinePointAtIndex(ProjectilePathResult.PathData[i].Location,i,ESplineCoordinateSpace::World);
// 	// 	// PredectionSplineMesh.Add(NewObject<USplineMeshComponent>(this,USplineMeshComponent::StaticClass));
// 	// 	// PredectionSplineMesh[i]->M
// 	// }
// 	// // PredictionSpline->AddSplinePointAtIndex()
// 	// if(bIsHit)
// 	// {
// 	// 	//show the end point
// 	// }
// }

// void APlayerCharacter::OnConstruction(const FTransform &Transform)
// {
// 	Super::OnConstruction(Transform);
// 	UE_LOG(LogTemp,Warning,TEXT("OnConsctuction"));
// 	PredictionSpline = NewObject<USplineComponent>(this,USplineComponent::StaticClass());
// 	if(PredictionSpline)
// 	{
// 		UE_LOG(LogTemp,Warning,TEXT("Spline constructed"));
// 	}

// }


