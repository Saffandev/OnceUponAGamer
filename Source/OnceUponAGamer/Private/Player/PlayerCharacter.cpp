// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
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

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
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
	AIPerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense_Touch>());
	AIPerceptionStimuliSource->RegisterWithPerceptionSystem();

	GetCapsuleComponent()->InitCapsuleSize(40.f, 96.f);
	bIsWallRunning = false;
	bCanDoWallRunAgain = true;
	bCanEndWallRun = false;

	PrimaryWeapon.WeaponName = EWeaponName::EWN_None;
	PrimaryWeapon.MaxAmmo = 0;
	PrimaryWeapon.TotalAmmo = 0;
	PrimaryWeapon.MagSize = 0;
	PrimaryWeapon.CurrentMagAmmo = 0;
	PrimaryWeapon.ReloadTime = 0.f;
	PrimaryWeapon.Accuracy = 0.f;
	PrimaryWeapon.FireRate = 0.f;

	SecondaryWeapon.WeaponName = EWeaponName::EWN_None;
	SecondaryWeapon.MaxAmmo = 0;
	SecondaryWeapon.TotalAmmo = 0;
	SecondaryWeapon.MagSize = 0;
	SecondaryWeapon.CurrentMagAmmo = 0;
	SecondaryWeapon.ReloadTime = 0.f;
	SecondaryWeapon.Accuracy = 0.f;
	SecondaryWeapon.Accuracy = 0.f;
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

	PlayerController = GetController();
	// Binding Section
	Capsule->OnComponentHit.AddDynamic(this, &APlayerCharacter::OnHit);

	CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(InitialWeapon);
	CurrentWeapon->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponSocket_r"));

	PrimaryWeapon.WeaponClass = InitialWeapon;
	PrimaryWeapon.PickupWeaponClass = InitialWeapon->GetDefaultObject<AWeaponBase>()->GetPickupWeapon();
	SecondaryWeapon.WeaponClass = InitialWeapon;
	SecondaryWeapon.PickupWeaponClass = InitialWeapon->GetDefaultObject<AWeaponBase>()->GetPickupWeapon();

	JumpsLeft = MaxJumps;
	WalkSpeed = CharacterMovement->MaxWalkSpeed;
	CurrentMovementType = EMovementType::EMT_Walking;

	DefaultFOV = Camera->FieldOfView;
	WeaponEquippedSlot = 0;

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
	PickupInAction();
	if (bCanPredictPath)
	{
		ThrowPredection();
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
	// PlayerInputComponent ->BindAction(TEXT("Secondary Throw"),			EInputEvent::IE_Released,this, &APlayerCharacter::SecondaryThrowEnd);
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
																	EDrawDebugTrace::ForDuration,
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
																   EDrawDebugTrace::ForDuration,
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
					UKismetSystemLibrary::MoveComponentTo(Capsule,
														  LandingLocation,
														  GetActorRotation(),
														  true,
														  true,
														  0.3,
														  true,
														  EMoveComponentAction::Move,
														  LatentInfo);
					// EnablePlayerInput();
				}
			}
		}
	}
}

bool APlayerCharacter::CanLedgeGrab(FVector ImpactPoint)
{

	return CanUncrouch(0.f) &&
		   CharacterMovement->IsFalling() &&
		   ForwardAxis > 0 &&
		   (GetActorLocation().Z + 90.f) >= ImpactPoint.Z &&
		   (GetActorLocation().Z - 20.f) <= ImpactPoint.Z;
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
//==========================================================================Wall run conditions===========================================//
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
//===================================================================================Wall run in action========================//

void APlayerCharacter::BeginWallRun()
{
	CanPerformCertainMovement(EMovementType::EMT_WallRun);
	CharacterMovement->GravityScale = 0.f;
	CharacterMovement->AirControl = 1.f;
	CharacterMovement->SetPlaneConstraintNormal(FVector(0, 0, 3));
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
		UE_LOG(LogTemp, Warning, TEXT("Delay timer runs out"));
		EndWallRun(EWallRunEndReason::EWRER_FallOfWall); // End wallrun(fall of wall)
	}
	UpdateWallRun();
}

void APlayerCharacter::WallRunOutOfAction()
{
	if (bIsWallRunning)
	{
		UE_LOG(LogTemp, Warning, TEXT("Timeline finished"));
		EndWallRun(EWallRunEndReason::EWRER_FallOfWall);
	}
}

void APlayerCharacter::UpdateWallRun()
{
	if (!(ForwardAxis > 0 && UKismetMathLibrary::VSize(GetVelocity()) > 100.f))
	{
		UE_LOG(LogTemp, Warning, TEXT("Required keys not down"));
		EndWallRun(EWallRunEndReason::EWRER_FallOfWall); // End wallrun Fall of from wall
		return;
	}

	FVector EndTrace = FVector::CrossProduct(WallRunDirection, FVector(0, 0, (WallRunSide == EWallRunDirection::EWRD_Right ? 1 : -1)));
	EndTrace = EndTrace * 200 + GetActorLocation();
	TArray<AActor *> ActorsToIgnore;
	FHitResult WallRunOutHit;
	bool bIsWallHit = UKismetSystemLibrary::LineTraceSingle(this,
															GetActorLocation(),
															EndTrace,
															UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
															false,
															ActorsToIgnore,
															EDrawDebugTrace::None,
															WallRunOutHit,
															true);
	if (!bIsWallHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("No wall hit"));
		EndWallRun(EWallRunEndReason::EWRER_FallOfWall); // end wall run fall of from wall
		return;
	}
	EWallRunDirection LWallRunSide = FVector2D::DotProduct(UKismetMathLibrary::Conv_VectorToVector2D(WallRunOutHit.ImpactNormal),
														   UKismetMathLibrary::Conv_VectorToVector2D(GetActorRightVector())) > 0.f
										 ? EWallRunDirection::EWRD_Right
										 : EWallRunDirection::EWRD_Left;
	if (LWallRunSide != WallRunSide)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wall run side not equal"));
		EndWallRun(EWallRunEndReason::EWRER_FallOfWall); // end wall run fall of from wall
		return;
	}

	WallRunDirection = FVector::CrossProduct(WallRunOutHit.ImpactNormal, FVector(0, 0, (WallRunSide == EWallRunDirection::EWRD_Left ? -1 : 1)));
	FVector WallRunVelocity = FVector(WallRunDirection * CharacterMovement->GetMaxSpeed());
	WallRunVelocity.Z = 0;
	CharacterMovement->Velocity = FVector(WallRunVelocity);
}

void APlayerCharacter::EndWallRun(EWallRunEndReason WallRunEndReason)
{
	UE_LOG(LogTemp, Warning, TEXT("End wall run"));
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
	GetWorld()->GetTimerManager().SetTimer(
		LTimerHandle, [&]()
		{ bCanDoWallRunAgain = true; },
		WallRunReEnableTime, false);
}
//===================================================================Wall run section end ======================================//

//==============================================================Enable and disable Player Input Section============//

void APlayerCharacter::EnablePlayerInput()
{
	EnableInput(GetWorld()->GetFirstPlayerController());
	bIsDoingLedgeGrab = false;
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
	AddMovementInput(GetActorRightVector(), AxisValue);
}

void APlayerCharacter::Turn(float AxisValue)
{
	SmoothHorizontalLook = UKismetMathLibrary::FInterpTo(SmoothHorizontalLook, AxisValue, UGameplayStatics::GetWorldDeltaSeconds(this), 30.f);
	AddControllerYawInput(SmoothHorizontalLook);

	// weapon sway for more visual smoothness
	float TurnInterpSpeed;
	if (bIsADS)
	{
		TurnInterpSpeed = 60;
	}
	else
	{
		TurnInterpSpeed = 50;
	}
	PlayerMesh->SetRelativeRotation(FRotator(PlayerMesh->GetRelativeRotation().Pitch,
											 PlayerMesh->GetRelativeRotation().Yaw - AxisValue,
											 PlayerMesh->GetRelativeRotation().Roll));

	FRotator NewRotation = FRotator(PlayerMesh->GetRelativeRotation().Pitch,
									Camera->GetRelativeRotation().Yaw - 90,
									PlayerMesh->GetRelativeRotation().Roll);

	PlayerMesh->SetRelativeRotation(FMath::RInterpTo(PlayerMesh->GetRelativeRotation(),
													 NewRotation,
													 UGameplayStatics::GetWorldDeltaSeconds(this),
													 TurnInterpSpeed));
}

void APlayerCharacter::LookUp(float AxisValue)
{
	SmoothVerticalLook = UKismetMathLibrary::FInterpTo(SmoothVerticalLook, AxisValue, UGameplayStatics::GetWorldDeltaSeconds(this), 30.f);
	AddControllerPitchInput(SmoothVerticalLook);

	// weapon sway for more visual smoothness
	float LookupInterpSpeed;
	if (bIsADS)
	{
		LookupInterpSpeed = 60;
	}
	else
	{
		LookupInterpSpeed = 50;
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
													 LookupInterpSpeed));
}
//===========================================================Input Axis Section End==========================================//

//========================================================================Jumping==================================//

void APlayerCharacter::Jump()
{
	if (GetCurrentMovementType() == EMovementType::EMT_Crouching)
	{
		Crouch();
	}
	else
	{
		SlideDirection = GetActorForwardVector();
		if (SlideTimeline.IsPlaying())
		{
			SlideTimeline.Stop();
			SetMovementSpeed(EMovementType::EMT_Walking);
			if (bIsCrouched)
			{
				Crouch();
			}
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
			// SetMovementSpeed(EMovementType::EMT_Walking);
			LaunchCharacter(FindLaunchVelocity(), false, true);
		}
		else if (JumpsLeft > 0)
		{
			JumpsLeft--;
			// SetMovementSpeed(EMovementType::EMT_Walking);
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
		LaunchDirection = FVector::CrossProduct(WallRunDirection, FVector(0, 0, WallRunSide == EWallRunDirection::EWRD_Right ? 1 : -1));
	}
	// else if(CharacterMovement->IsFalling())
	// {
	// 	LaunchDirection = (GetActorForwardVector() * ForwardAxis) + (GetActorRightVector() * RightAxis);
	// }
	FVector FinalLaunchVelocity = (LaunchDirection + FVector(0, 0, 1)) * CharacterMovement->JumpZVelocity;
	return FinalLaunchVelocity;
}

//=========================================================Jumping End===================================================//

//========================================================Crouch===================================//

void APlayerCharacter::Crouch()
{
	if (bIsCrouched && CanUncrouch(CrouchCapsuleHeight))
	{
		bIsCrouched = false;
		SlideTimeline.Stop();
		CrouchTimeline.Reverse();
		UE_LOG(LogTemp, Warning, TEXT("Uncrouch"));
		if (bIsSprintKeyDown)
		{
			Sprint();
		}
		else
		{
			SetMovementSpeed(EMovementType::EMT_Walking);
		}
	}
	else if (CanPerformCertainMovement(EMovementType::EMT_Sliding) && !CharacterMovement->IsFalling())
	{
		Slide(GetActorForwardVector(), "Crouch");
	}
	else if (CanPerformCertainMovement(EMovementType::EMT_Crouching))
	{
		bIsCrouched = true;
		CrouchTimeline.Play();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Else of the crouch"));
	}
}

void APlayerCharacter::ToggleCrouch()
{
	float Alpha = CrouchCurve->GetFloatValue(CrouchTimeline.GetPlaybackPosition());
	GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::Lerp(StandingCapsuleHeight, CrouchCapsuleHeight, Alpha));
	Camera->SetRelativeLocation(FVector(
		Camera->GetRelativeLocation().X,
		Camera->GetRelativeLocation().Y,
		FMath::Lerp(StandingCameraHeight, CrouchCameraHeight, Alpha)));
}

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
	UE_LOG(LogTemp, Warning, TEXT("Caller = %s"), *Caller);
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
}

void APlayerCharacter::PerformSlide()
{
	float Alpha = SlideCurve->GetFloatValue(SlideTimeline.GetPlaybackPosition());
	CharacterMovement->MaxWalkSpeed = FMath::Lerp(SlideSpeed, CrouchSpeed, Alpha);
	UE_LOG(LogTemp, Warning, TEXT("Slide Direction = %s"), *SlideDirection.ToString());
	AddMovementInput(SlideDirection, 1);
	if (GetVelocity().Size() < 10)
	{
		SlideTimeline.Stop();
		SetMovementSpeed(EMovementType::EMT_Crouching);
		UE_LOG(LogTemp, Warning, TEXT("Sliding stopped due to 0 velocity"));
	}
}

void APlayerCharacter::EndSlide()
{
	SetMovementSpeed(EMovementType::EMT_Crouching);
}
//======================================================================Slide End================================================//

//======================================================================Sprint start===============================//

void APlayerCharacter::Sprint()
{
	bIsSprintKeyDown = true;
	if (GetCurrentMovementType() == EMovementType::EMT_Crouching)
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
				SlideTimeline.SetPlayRate(3);
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
				SlideTimeline.Stop();
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
	if (CurrentWeapon == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO weapon base reference"));
		return;
	}
	bIsShooting = true;
	CurrentWeapon->Shoot();
}

void APlayerCharacter::StopShooting()
{
	if (CurrentWeapon == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO weapon base reference"));
		return;
	}
	bIsShooting = false;
	CurrentWeapon->StopShooting();
}

void APlayerCharacter::ADSON()
{
	bIsADSButtonDown = true;
	if (!bIsDoingMeleeAttack)
	{
		SetMovementSpeed(GetCurrentMovementType());
		ADS_OnTimeline.PlayFromStart();
	}
}

void APlayerCharacter::ADSOnInAction()
{
	bIsADS = true;
	float Alpha = CrouchCurve->GetFloatValue(ADS_OnTimeline.GetPlaybackPosition());
	PerformADS(ADSFOV, 1.3f, Alpha);
}

void APlayerCharacter::ADSOFF()
{
	bIsADSButtonDown = false;
	SetMovementSpeed(GetCurrentMovementType());
	ADS_OffTimeline.PlayFromStart();
}

void APlayerCharacter::ADSOffInAction()
{
	bIsADS = false;
	float Alpha = CrouchCurve->GetFloatValue(ADS_OffTimeline.GetPlaybackPosition());
	PerformADS(DefaultFOV, 0.5f, Alpha);
}

void APlayerCharacter::PerformADS(float FinalAdsFovValue, float NewVignetteIntensity, float Alpha)
{
	float newFOV = FMath::Lerp(Camera->FieldOfView, FinalAdsFovValue, Alpha);
	Camera->SetFieldOfView(newFOV);
	Camera->PostProcessSettings.bOverride_VignetteIntensity = true;
	Camera->PostProcessSettings.VignetteIntensity = FMath::Lerp(Camera->PostProcessSettings.VignetteIntensity, NewVignetteIntensity, Alpha);
}
void APlayerCharacter::Reload()
{
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

void APlayerCharacter::PlayCameraShake(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale)
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
	FVector EndTrace = GetActorLocation() + GetActorUpVector() * -100;
	FHitResult PickupHit;
	TArray<AActor *> ActorsToIgnore;
	TArray<TEnumAsByte<EObjectTypeQuery>> PickupObject;
	PickupObject.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel2));
	bool bIsPickupHit = UKismetSystemLibrary::SphereTraceSingleForObjects(this,
																		  GetActorLocation(),
																		  EndTrace,
																		  30,
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
			IPickupWeaponInterface *PickupWeaponInterface = Cast<IPickupWeaponInterface>(HitActor);
			if (PickupWeaponInterface != nullptr)
			{
				if (PickupWeaponInterface->IsPickupGun()) // gun pickup
				{
					if (UKismetMathLibrary::NotEqual_ClassClass(PrimaryWeapon.WeaponClass.Get(), HitActor->GetClass()) &&
						UKismetMathLibrary::NotEqual_ClassClass(SecondaryWeapon.WeaponClass.Get(), HitActor->GetClass()))
					{
						UE_LOG(LogTemp, Warning, TEXT("Pickup Weapon Ok"));
						if (bCanPickup)
						{
							PickupHitWeapon = HitActor;
							bCanPickup = false;
							PickupWeaponInterface->SetPickupWeaponName();
							PickupWeaponInterface->PickupWeapon();
						}
					}

					else // Weapon Already in the slot
					{
						UE_LOG(LogTemp, Warning, TEXT("Weapon Already in the slot"));
						if (UKismetMathLibrary::EqualEqual_ClassClass(PrimaryWeapon.WeaponClass.Get(), HitActor->GetClass()))
						{
							if (PrimaryWeapon.TotalAmmo < PrimaryWeapon.MaxAmmo)
							{
								UE_LOG(LogTemp, Error, TEXT("Primary Weapon Ammo pick"));
								PrimaryWeapon.TotalAmmo = FMath::Min(PrimaryWeapon.TotalAmmo + 30, PrimaryWeapon.MaxAmmo);
								CurrentWeapon->TotalAmmo = PrimaryWeapon.TotalAmmo;
								HitActor->Destroy();
							}
						}
						else if (UKismetMathLibrary::EqualEqual_ClassClass(SecondaryWeapon.WeaponClass.Get(), HitActor->GetClass()))
						{
							if (SecondaryWeapon.TotalAmmo < SecondaryWeapon.MaxAmmo)
							{
								UE_LOG(LogTemp, Error, TEXT("Secondary Weapon Ammo pick"));
								SecondaryWeapon.TotalAmmo = FMath::Min((SecondaryWeapon.TotalAmmo + 30), SecondaryWeapon.MaxAmmo);
								CurrentWeapon->TotalAmmo = SecondaryWeapon.TotalAmmo;
								HitActor->Destroy();
							}
						}
					}
				}

				else
				{
					// item not in the slot
					if (UKismetMathLibrary::NotEqual_ClassClass(PrimaryThrowableData.BP_Throwable.Get(), HitActor->GetClass()) &&
						UKismetMathLibrary::NotEqual_ClassClass(SecondaryThrowableData.BP_Throwable.Get(), HitActor->GetClass()))
					{
						if (bCanPickup)
						{
							bCanPickup = false;
							UE_LOG(LogTemp, Warning, TEXT("Throwable not in slot"));
							// PickupWeaponInterface->SetPickupWeaponName();
							PickupWeaponInterface->PickupWeapon();
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Throwable in slot"));

						if (UKismetMathLibrary::EqualEqual_ClassClass(PrimaryThrowableData.BP_Throwable.Get(), HitActor->GetClass()))
						{
							if (PrimaryThrowableData.Count < ThrowableMaxCount)
							{
								PrimaryThrowableData.Count++;
								HitActor->Destroy();
							}
						}
						else if (UKismetMathLibrary::EqualEqual_ClassClass(SecondaryThrowableData.BP_Throwable.Get(), HitActor->GetClass()))
						{
							if (SecondaryThrowableData.Count < ThrowableMaxCount)
							{
								SecondaryThrowableData.Count++;
								HitActor->Destroy();
							}
						}
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Pickup weapon has no interface"));
				PickupWeaponName = EWeaponName::EWN_None;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PickupWeaponNull"));
		}
	}
}

void APlayerCharacter::EquipPrimaryWeapon()
{
	if (WeaponEquippedSlot == 1)
	{
		if (!(PrimaryWeapon.WeaponClass->IsChildOf(AKnifeWeapon::StaticClass())) ||
			!(SecondaryWeapon.WeaponClass->IsChildOf(AKnifeWeapon::StaticClass())))
		{
			WeaponEquippedSlot = 0;
			SwitchWeapon();
		}
	}
}

void APlayerCharacter::EquipSecondaryWeapon()
{
	if (WeaponEquippedSlot == 0)
	{
		if (!(PrimaryWeapon.WeaponClass->IsChildOf(AKnifeWeapon::StaticClass())) ||
			!(SecondaryWeapon.WeaponClass->IsChildOf(AKnifeWeapon::StaticClass())))
		{
			WeaponEquippedSlot = 1;
			SwitchWeapon();
		}
	}
}

void APlayerCharacter::SwitchWeapon()
{
	if (WeaponEquippedSlot == 0)
	{
		SetWeaponVars(PrimaryWeapon, true);
	}

	else if (WeaponEquippedSlot == 1)
	{
		SetWeaponVars(SecondaryWeapon, false);
	}
}

void APlayerCharacter::SetWeaponVars(FWeaponData NewWeaponData, bool bIsPrimaryWeapon)
{
	if (NewWeaponData.WeaponClass != nullptr)
	{
		CurrentWeapon = Cast<AWeaponBase>(PickupHitWeapon);
		if (CurrentWeapon == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("NO weapon reference"));
			return;
		}
		PickupHitWeapon->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponSocket_r"));
		CurrentWeapon->bIsPrimaryWeapon = bIsPrimaryWeapon;
		CurrentWeapon->DrawWeapon();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("New weapon data has a null weapon class"));
	}
}

void APlayerCharacter::DropWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->bIsPlayerHoldingTheWeapon = false;
		CurrentWeapon->DropGun();
	}
}

void APlayerCharacter::MeleeAttack()
{
	if (!bIsDoingMeleeAttack && !bIsReloading && !bIsADS)
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
	if (bIsMeleeHit)
	{
		// apply damage and radial force
	}
}

//================================================Throwable Stuff======================================================

void APlayerCharacter::SwitchThrowable()
{
	if (ThrowableEquippedSlot == 0)
	{
		ThrowableEquippedSlot = 1;
	}
	else
	{
		ThrowableEquippedSlot = 0;
	}
}

void APlayerCharacter::PrimaryThrowStart()
{
	
	if (ThrowableEquippedSlot == 0 && PrimaryThrowableData.Count > 0)
	{
		PrimaryThrowableData.Count--;
		PrimaryThrowable = StartThrow(PrimaryThrowableData.BP_Throwable);
	}
	else if (ThrowableEquippedSlot == 1 && SecondaryThrowableData.Count > 0)
	{
		SecondaryThrowableData.Count--;
		SecondaryThrowable = StartThrow(SecondaryThrowableData.BP_Throwable);
	}
}

void APlayerCharacter::PrimaryThrowEnd()
{
	if (ThrowableEquippedSlot == 0)
	{
		EndThrow(PrimaryThrowable);
	}
	else if (ThrowableEquippedSlot == 1)
	{
		EndThrow(SecondaryThrowable);
	}
}

AThrowableBase *APlayerCharacter::StartThrow(TSubclassOf<AThrowableBase> Throwable)
{
	AThrowableBase *SpawnedThrowable = nullptr;
	if (Throwable)
	{
		SpawnedThrowable = GetWorld()->SpawnActor<AThrowableBase>(Throwable, PlayerMesh->GetSocketLocation(FName("WeaponSocket_r")), FRotator::ZeroRotator);
		SpawnedThrowable->AttachToComponent(PlayerMesh,FAttachmentTransformRules::SnapToTargetNotIncludingScale,FName("WeaponSocket_r"));
		SpawnedThrowable->Initiate();
		bCanPredictPath = true;
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
	}
}

void APlayerCharacter::ThrowPredection()
{
	FVector EndPoint = GetActorLocation() + ((GetActorForwardVector() + GetControlRotation().Vector()) * FMath::Pow(ThrowSpeed, 1.1));
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(this,
														  ThrowVelocity,
														  PlayerMesh->GetSocketLocation(FName("WeaponSocket_r")),
														  EndPoint,
														  0.f,
														  0.9);
	// FPredictProjectilePathParams ProjectilePathParams;
	// ProjectilePathParams.StartLocation = PlayerMesh->GetSocketLocation(FName("WeaponSocket_r"));
	// ProjectilePathParams.LaunchVelocity = ThrowVelocity;
	// ProjectilePathParams.ProjectileRadius = 10.f;
	// ProjectilePathParams.bTraceWithChannel = true;
	// ProjectilePathParams.bTraceWithCollision = true;
	// ProjectilePathParams.TraceChannel = ECollisionChannel::ECC_WorldStatic;
	// ProjectilePathParams.ActorsToIgnore.Add(this);
	// ProjectilePathParams.ActorsToIgnore.Add(PrimaryThrowable);
	// ProjectilePathParams.ActorsToIgnore.Add(SecondaryThrowable);
	// ProjectilePathParams.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	// ProjectilePathParams.SimFrequency = 30;
	// FPredictProjectilePathResult ProjectilePathResult;
	// bool bIsHit = UGameplayStatics::PredictProjectilePath(this,ProjectilePathParams,ProjectilePathResult);
	
	// for(int32 i = 0 ;i<ProjectilePathResult.PathData.Num();i++)
	// {
	// 	PredictionSpline->AddSplinePointAtIndex(ProjectilePathResult.PathData[i].Location,i,ESplineCoordinateSpace::World);
	// 	// PredectionSplineMesh.Add(NewObject<USplineMeshComponent>(this,USplineMeshComponent::StaticClass));
	// 	// PredectionSplineMesh[i]->M
	// }
	// // PredictionSpline->AddSplinePointAtIndex()
	// if(bIsHit)
	// {
	// 	//show the end point
	// }
}

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