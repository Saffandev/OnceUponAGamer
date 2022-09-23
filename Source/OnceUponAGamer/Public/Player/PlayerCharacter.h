// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "Enum/EnumWeaponName.h"
#include "Enum/EnumThrowableNames.h"
#include "PlayerCharacter.generated.h"

class UCurveFloat;
class UCameraComponent;
class AWeaponBase;
class AThrowableBase;

UENUM(BlueprintType)
	enum class EMovementType : uint8
	{
		EMT_Walking UMETA(DisplayName = "Walking"),
		EMT_Sprinting UMETA(DisplayName = "Sprinting"),
		EMT_Crouching UMETA(DisplayName = "Crouching"),
		EMT_Sliding UMETA(DisplayName = "Sliding"),
		EMT_WallRun UMETA(DisplayName = "WallRun")
	};

	enum class EWallRunDirection : uint8
	{
		EWRD_Left UMETA(DisplayName = "LeftWall"),
		EWRD_Right UMETA(DisplayName = "RightWall")
	};

	enum class EWallRunEndReason : uint8
	{
		EWRER_FallOfWall,
		EWRER_JumpOfWall
	};

USTRUCT(BlueprintType)
	struct FWeaponData
	{
		GENERATED_BODY()
		UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		EWeaponName WeaponName;
		UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		TSubclassOf<AWeaponBase> WeaponClass;
		UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		int32 MaxAmmo;
		UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		int32 TotalAmmo;
		UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		int32 CurrentMagAmmo;
	};

USTRUCT(BlueprintType)
	struct FThrowableData
	{
		GENERATED_BODY()
		UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		TSubclassOf<AThrowableBase> BP_Throwable;
		UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		int32 Count;
		UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		EThrowableName ThrowableName;
	};

UCLASS()
class ONCEUPONAGAMER_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Landed(const FHitResult& Hit);
	EMovementType GetCurrentMovementType();
	UCameraComponent* GetCamera();
	void PlayCameraShake(TSubclassOf<UCameraShakeBase> ShakeClass,float Scale);
	USkeletalMeshComponent* GetMesh();
	bool IsInAir();
	bool GetWallRunning()
	{
		return bIsWallRunning;
	}
	
	//================================Weapons=============================//
	bool IsADSButtonDown();
	void ADSON();
	void DropWeapon(bool bIsPrimaryDrop);
	void SwitchWeapon(bool bIsPickupWeapon);
	void Reload();
	UFUNCTION(BlueprintCallable)
	FWeaponData GetWeaponData(bool bIsSecondaryWeapon);
	UFUNCTION(BlueprintCallable)
	void SetWeaponData(bool bIsSecondaryWeapon, FWeaponData WeaponData);//used by the save game to set the weapon equipped before quiting the game
	
	//===============================Throwables==========================//
	void ReEquipAfterGrenade();
	void DropThrowable(bool bIsPrimary);
	UFUNCTION(BlueprintCallable)
	FThrowableData GetThrowableData(bool bIsSecondaryThrowable);
	UFUNCTION(BlueprintCallable)
	void SetThrowableData(FThrowableData ThrowableData, bool bIsSecondaryThrowable);
	bool GetIsThrowingThrowable()
	{
		return bIsThrowing;
	}
	//==============================Health/Sheild=====================//
	UFUNCTION(BlueprintCallable)
	void Heal(float Health);
	UFUNCTION(BlueprintCallable)
	void SetHealthShield(float Health,float Shield);//used by the save game
	UFUNCTION(BlueprintCallable)
	void HealShield(float ShieldHeal);
	void RegainShield();
	
	UFUNCTION(BlueprintImplementableEvent)
	void HitMarker();
	UFUNCTION(BlueprintImplementableEvent)
	void DamageMarker(AActor* DamageCauser);
	UFUNCTION(BlueprintImplementableEvent)
	void Dead();
	
protected:
	virtual void BeginPlay() override;
	// void OnConstruction(const FTransform &Transform) override;
				 
private:

	//======================================== Movement=========================================//
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void Turn(float AxisValue);
	void LookUp(float AxisValue);
	void HandSway(float AxisValue);
	void Jump();
	void StopJump();
	void Sprint();
	void Slide(FVector LSlideDirection,FString Caller = "None");
	void CancelSprint();
	void SetMovementSpeed(enum EMovementType MovementType);
	void Crouch();
	bool CanUncrouch(float CapsuleHeightAlpha = 0.f) const;
	bool CanPerformCertainMovement(EMovementType MovementMode);
	FVector FindLaunchVelocity() const;	

	UFUNCTION()
	void ToggleCrouch();
	
	UFUNCTION()
	void PerformSlide();
	
	UFUNCTION()
	void EndSlide();
	
	UFUNCTION()
	void DisablePlayerInput();
	
	UFUNCTION(BlueprintCallable)
	void EnablePlayerInput();

	//======================================== Ledge Grab =========================================//
	void LedgeGrab(FVector ImpactPoint);
	bool CanLedgeGrab(FVector ImpactPoint);
	
	//======================================== Wall Run =========================================//
	void WallRun(AActor* HitActor, FVector HitImpactNormal);
	bool CanDoWallRun(AActor* HitActor, FVector HitImpactNormal);
	bool IsPlayerInStateOfWallRun(FVector HitImpactNormal);
	bool CanSurfaceBeWallRun(FVector HitImpactNormal);
	void BeginWallRun();
	void UpdateWallRun();
	UFUNCTION()
	void WallRunInAction();
	UFUNCTION()
	void WallRunOutOfAction();
	void EndWallRun(EWallRunEndReason WallRunEndReason);
	UFUNCTION()
	void CameraTiltAction();

	//======================================== Weapons =========================================//
	void Shoot();
	void StopShooting();
	void ADSOFF();
	void PerformADS(float FinalADSValue,float NewVignetteIntensity,float Alpha);
	void EquipPrimaryWeapon();
	void EquipSecondaryWeapon();
	void Pickup();
	void NoPickup();
	void PickupInAction();
	void PickupGun(AActor* HitActor);
	void PickupThrowable(AActor* HitActor);
	void MeleeAttack();

	void PrimaryThrowStart();
	void PrimaryThrowEnd();
	void SwitchThrowable();
	void EndThrow(AThrowableBase* CurrentThrowable);
	UFUNCTION(BlueprintCallable)
	void InitiateEndThrow();
	void ThrowPredection();
	void SetWeaponVars(FWeaponData NewWeaponData,bool bIsPrimaryWeapon,bool bIsPickupWeapon);
	AThrowableBase* StartThrow(TSubclassOf<AThrowableBase> Throwable);

	UFUNCTION()
	void ADSOnInAction();
	
	UFUNCTION(BlueprintCallable,meta = (AllowPrivateAccess = "true"))
	void MeleeAttackInAction();

	UFUNCTION()
	void ADSOffInAction();
	
	//======================================== Hit/Damage =========================================//
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* HitActor, UPrimitiveComponent* OtherComp,FVector NormalizeImpulse,const FHitResult& Hit);
	
	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor,float Damage,const UDamageType* DamageType,AController* InstigatedBy,AActor* DamageCauser);

	UFUNCTION()
	void TakePointDamage(AActor* DamagedActor,float Damage,AController* InstigatedBy, FVector HitLocation,UPrimitiveComponent* HitComp,FName BoneName,FVector ShotDirection,const UDamageType* DamageType,AActor* DamageCauser);

	UFUNCTION()
	void TakeRadialDamage(AActor* DamagedActor,float Damage,const UDamageType* DamageType,FVector Origin,FHitResult Hit,AController* InstigatedBy,AActor* DamageCauser);
	
	void TakeDamage(float Damage);

	

private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* PlayerMesh;

	UPROPERTY(VisibleAnywhere)
	class UAIPerceptionStimuliSourceComponent* AIPerceptionStimuliSource;

	UCharacterMovementComponent* CharacterMovement;
	AController* PlayerController;
	UCapsuleComponent* Capsule;
	FTransform PlayerMeshTransform;
	//======================================== Curves =========================================//
	UPROPERTY(EditAnywhere,Category = "Curves")
	UCurveFloat* CrouchCurve;

	UPROPERTY(EditAnywhere,Category = "Curves")
	UCurveFloat* SlideCurve;

	UPROPERTY(EditAnywhere,Category = "Curves")
	UCurveFloat* CameraTiltCurve;

	UPROPERTY(EditAnywhere,Category = "Curves")
	UCurveFloat* WallRunUpdateCurve;

	//======================================== Timelines =========================================//
	FTimeline CrouchTimeline;//timeline componet, you have to create different FTimeline for each new timeline
	FTimeline SlideTimeline;
	FTimeline CameraTiltTimeline;
	FTimeline WallRunUpdateTimeline;
	FTimeline ADS_OnTimeline;
	FTimeline ADS_OffTimeline;
	FTimerHandle WallRunTimerHandle;
	
	//======================================== Wall Run =========================================//
	bool bIsCrouched;
	bool bIsSprintKeyDown;
	bool bIsWallRunning;
	bool bCanEndWallRun;
	bool bCanDoWallRunAgain;
	FVector WallRunDirection;
	EWallRunDirection WallRunSide;

	//======================================== Weapons =========================================//
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Weapon",meta = (AllowPrivateAccess = "true"))
	bool bIsADS;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Weapon",meta = (AllowPrivateAccess = "true"))
	bool bIsWeaponHit;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Weapon",meta = (AllowPrivateAccess = "true"))
	bool bIsPickupGun;
	UPROPERTY(EditAnywhere,Category = "Weapon")
	float ADSFOV;
	UPROPERTY(EditAnywhere,Category = "Weapon")
	TSubclassOf<AWeaponBase> InitialWeapon;
	UPROPERTY(EditAnywhere,Category = "Weapon")
	USoundBase* GunPickupSound;

	bool bIsADSButtonDown;
	bool bCanPickup;
	AActor* PickupHitWeapon;
	class IPickupWeaponInterface *PickupWeaponInterface;
	//======================================== Throwable =========================================//
	UPROPERTY(EditAnywhere,Category = "Throwable")
	UAnimMontage* GrenadeHoldMontage;
	UPROPERTY(EditAnywhere,Category = "Throwable")
	UAnimMontage* ThrowMontage;
	UPROPERTY(EditAnywhere,Category = "Throwable")
	int32 ThrowableMaxCount;
	UPROPERTY(EditAnywhere,Category = "Throwable")
	TSubclassOf<AThrowableBase> BPThrowable;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Throwable",meta = (AllowPrivateAccess = "true"))
	float ThrowSpeed;
	UPROPERTY(VisibleAnywhere,BlueprintReadonly,meta = (AllowPrivateAccess = "true"))
	bool bIsThrowing;
	UPROPERTY(EditAnywhere,Category = "Throwable")
	USoundBase* ThrowablePickupSound;

	FVector ThrowVelocity;
	AThrowableBase* PrimaryThrowable;
	AThrowableBase* SecondaryThrowable;
	bool bCanPredictPath;
	
	//======================================== Movement =========================================//
	bool bIsDoingLedgeGrab;
	float CrouchCapsuleHeight;
	float StandingCapsuleHeight;
	float CrouchCameraHeight;
	float StandingCameraHeight;
	float ForwardAxis;
	float RightAxis;
	float DefaultFOV;
	float SmoothHorizontalLook;
	float SmoothVerticalLook;

	UPROPERTY(EditAnywhere,Category = "Movement")
	float CrouchScale;
	UPROPERTY(EditAnywhere,Category = "Movement")
	float WalkSpeed;
	UPROPERTY(EditAnywhere,Category = "Movement")
	float SprintSpeed;
	UPROPERTY(EditAnywhere,Category = "Movement")
	float CrouchSpeed;
	UPROPERTY(EditAnywhere,Category = "Movement")
	float SlideSpeed;
	UPROPERTY(EditAnywhere,Category = "Movement")
	float WallRunSpeed;
	UPROPERTY(EditAnywhere,Category = "Movement")
	int32 MaxJumps = 2;;
	UPROPERTY(EditAnywhere,Category = "Movement")
	float TurnInterpSpeed;
	UPROPERTY(EditAnywhere,Category = "Movement")
	float TurnInterpSpeedADS;
	UPROPERTY(EditAnywhere,Category = "Movement")
	float LookInterpSpeed;
	UPROPERTY(EditAnywhere,Category = "Movement")
	float LookInterpSpeedADS;
	UPROPERTY(EditAnywhere,Category = "Movement")
	float HandSwayRotationValue;
	UPROPERTY(EditAnywhere,Category = "Movement")
	float SwaySpeed;
	UPROPERTY(EditAnywhere,Category = "Movement")
	USoundBase* DeathSound;
	int32 JumpsLeft;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Movement",meta = (AllowPrivateAccess = "true"))
	EMovementType CurrentMovementType;//used for driving animation in anim bp
	FVector SlideDirection;
	
	
	// UPROPERTY(EditAnywhere)
	// class USplineComponent* PredictionSpline;
	// TArray<class USplineMeshComponent*> PredictionSplineMesh;
	// UPROPERTY(EditAnywhere)
	// UStaticMesh* SplineMesh;

	//======================================== Health/Shield =========================================//
	UPROPERTY(EditAnywhere)
	float MaxHealth;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	float CurrentHealth;
	UPROPERTY(EditAnywhere)
	float MaxShield;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	float CurrentShield;
	UPROPERTY(EditAnywhere)
	float TimeForShieldRecharge;
	UPROPERTY(EditAnywhere)
	float ShieldRechargeRate;
	FTimerHandle ShieldRechargeTimer;

public:

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Movement")
	FRotator SwayRotation;
	//======================================== Weapons =========================================//
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Weapon")
	FWeaponData PrimaryWeapon;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Weapon")
	FWeaponData SecondaryWeapon;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Weapon")
	int32 WeaponEquippedSlot;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	AWeaponBase* CurrentWeapon;
	AWeaponBase* EqPrimaryWeapon;
	AWeaponBase* EqSecondaryWeapon;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	EWeaponName PickupWeaponName;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	float ADS_Alpha;

	bool bIsShooting;
	//======================================== Throwable =========================================//
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Throwable")
	FThrowableData PrimaryThrowableData;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Throwable")
	FThrowableData SecondaryThrowableData;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Throwable")
	EThrowableName ThrowableName;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Throwable")
	int32 ThrowableEquippedSlot;

	bool bIsThrowableExploded;

	bool bIsDoingMeleeAttack;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool bIsReloading;
};
