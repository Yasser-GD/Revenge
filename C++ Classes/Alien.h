// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Damage_Systeem_InterFac.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Alien.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBoxComponent;
class USoundCue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttacking, int, AttackNumber, FVector, AttackLocation, bool, IsUsingRightLeg);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDodge);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShortRangeAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGettingHitReactions, bool, IsSmall, bool, IsMedium, bool, Islarge);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActivateShieldSkill, bool, IsActivatingShield);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTakingHeavyDamage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAlienDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTakingAnyDamage, float, Health);



UENUM(BlueprintType)
enum class EAlienMainState : uint8 {

	GroundAttacksState = 0,
	FlyingAttacksState = 1,
	FallingAfterHit = 2,

};

UENUM(BlueprintType)
enum class EAlienAttackState : uint8 {

	NormalWalkingAround = 0,
	LongRangeAttackState = 1,
	RunAndAttackState = 2,
	Holding_Oject = 3,

};

UENUM(BlueprintType)
enum class EAlien_FlyingAttackState : uint8 {

	PickingUpAndThrowingObjects = 0,
	ThrowingObjectWithoutPickingUp = 1,

};


UCLASS()
class SHOOTINGGAME_API AAlien : public ACharacter, public IDamage_Systeem_InterFac
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAlien();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//To Take Damage
	virtual void TakeDamage_AllEnemys(float DamageAmount, FName HitBoneName) override;

	TArray<float> ArrayOfTakingHits;

	//If The Player Deflect An Attakck
	virtual void Deflection_impect() override;


	UPROPERTY(BlueprintAssignable)
		FOnShortRangeAttack OnShortAttackDelegte;

	UPROPERTY(BlueprintAssignable)
		FOnGettingHitReactions OnGetSmallReaction;

	UPROPERTY(BlueprintAssignable)
		FOnTakingHeavyDamage TakingHeavyDamage_Del;

	UPROPERTY(BlueprintAssignable)
		FOnAlienDeath AlienDeath_Del;

	UPROPERTY(BlueprintAssignable)
		FOnTakingAnyDamage TakingAnyDamage_Del;

public:

	//-----------------Basic Properties----------------------

	UPROPERTY()
		class AMyHeroo* Hero;

	UPROPERTY()
		class AMyHeroo* MainHeroRef;

	UPROPERTY()
		class APawn* PlayerPawn;
	UPROPERTY()
		class AAlien_AICont* Alien_AIConRef;

	bool CanBeDamaged = true;

public:

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		EAlienAttackState AlienAttackState;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		EAlienMainState AlienMainState;

public:

	//-----------------Basic Properties----------------------

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "The basics")
		bool AlienDead = false;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "The basics")
		bool IsAlreadyDead = false;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "The basics")
		float Alien_Health = 500;
	UPROPERTY(BlueprintReadwrite, Category = "The basics")
		float initial_Health;

	//------------------------------

public:

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UAIPerceptionComponent* AIPercCom;
	UPROPERTY()
		UAISenseConfig_Sight* SightSense;

	UFUNCTION()
		void OnSensed(const TArray<AActor*>& UpdatedActors);

	UPROPERTY()
		bool PlayerSensed = false;

	bool IsDoingAnEvent = false;


public:

	//------------------Walking Around Functionality-----------------------

	// Walking Amount 
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Dodge Stuff")
		float Right_WalkingAmount = 600;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Dodge Stuff")
		float Left_WalkingAmount = -600;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Dodge Stuff")
		float Back_WalkingAmount = 400;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Dodge Stuff")
		float Forward_WalkingAmount = -400;

	UPROPERTY(BlueprintReadWrite)
		bool IsRight_BoxOverlaping = false;
	UPROPERTY(BlueprintReadWrite)
		bool IsLeft_BoxOverlaping = false;
	UPROPERTY(BlueprintReadWrite)
		bool IsBack_BoxOverlaping = false;


	UFUNCTION()
		void MovingAroundAndToggleAttacks();
	bool MoveTwice = false;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Developer Stuff")
		bool AbleToMoveAround = true;

	UFUNCTION()
		void ToMoveBack();
	bool IsMovingBack = false;

	UFUNCTION()
		void ToMoveForward();
	bool IsMovingForward = false;

	UFUNCTION()
		void NewRotationToTarget(FVector ActorLocation, FVector TargetLocation);

	// To Assign New Speed And State 
	UFUNCTION()
		void ToChangeStateAndSpeed(EAlienAttackState NewState, float Speed);

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Speed Changing")
		float WalkingSpeed = 170;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Speed Changing")
		float RunSpeed = 500;

	//-------------------------------------------


public:
	//------------------ Dodgeing Functionality-----------------------

	UPROPERTY(BlueprintAssignable)
		FOnDodge OnDodge;

	UPROPERTY(BlueprintReadWrite)
		UBoxComponent* CheckWallBox;

	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Dodgeing Amount 
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Dodge Stuff")
		float Right_DodgeAmount = 400;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Dodge Stuff")
		float Left_DodgeAmount = -400;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Dodge Stuff")
		float Back_DodgeAmount = 400;

	// Location To Move To 
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Dodge Stuff")
		FVector MainDodgeLocation;

	UFUNCTION()
		void Dodge();

	int afterTwoHitsDodgeNum = 0;

	UFUNCTION()
		bool CheckIfSafeToDodge(FVector DodgeLocation);

	bool StartLerpingDodge = false;

	UPROPERTY()
		float TimeElasped = 0.0;
	UPROPERTY(EditAnywhere)
		float LerpDuration = 1.0;

	UPROPERTY(BlueprintReadWrite)
		bool DodgeToPlayer = false;


public:

	//-------------------Attacking Functionailty---------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* DamageBoxRightLeg;
	UFUNCTION()
		void LegDamageBox(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(BlueprintAssignable)
		FOnAttacking OnAttackingCast;
	UPROPERTY(EditAnywhere, Category = "Attack Montages")
		class UAnimMontage* Attack_One;
	UPROPERTY(EditAnywhere, Category = "Attack Montages")
		class UAnimMontage* Attack_Two;
	UPROPERTY(EditAnywhere, Category = "Attack Montages")
		class UAnimMontage* Attack_Three;

	UFUNCTION()
		void ToggleAttacks();

	UFUNCTION()
		void LongRange_PickUpAndThrowOROnlyThrowAttack(bool ShouldPickUpObject);

	UFUNCTION()
		void ShortRangeAttack();

	UPROPERTY(EditAnywhere)
		int NumToAttackAfter = 0;

	//----------------------------

public:

	bool IsAttacking = false;

	UPROPERTY(BlueprintReadWrite)
		bool StopAttacking = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UPhysicsHandleComponent* PhysicsHandleCompObject_1;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UPhysicsHandleComponent* PhysicsHandleCompObject_2;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UPhysicsHandleComponent* PhysicsHandleCompObject_3;

private:

	//-------------------------Short Range Attack-------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* AttackBox;

	UFUNCTION()
		void OnOverlapAttack(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* FlyUpMontage;
	bool MontageEnded = false;


public:

	UPROPERTY(EditAnyWhere)
		UClass* ObjectGrab_InterfaceRef;

	UFUNCTION()
		void ToCallPlayMontage();

	UPROPERTY(EditAnyWhere)
		UAnimMontage* ThrowMontage;


private:

	//------------------------------------- Hitting in the short Range attack ------------------------------------



	FTimerHandle MeleeAttackDamage_TimerHandle;

	UFUNCTION()
		void MeleeAttacksDamaging(int AttackNumber, FVector AttackLocation, bool IsUsingRightLeg);

	float SphereRedius = 70;
	FVector SphereMainLocation;


public:


	UFUNCTION(BlueprintCallable)
		void CallingMulitiMeleeAttacksDamaging(int AttackNumber, FVector AttackLocation, bool IsUsingRightLeg);

	UPROPERTY(BlueprintReadWrite)
		bool IsDamagingON = false;

	UPROPERTY(BlueprintReadWrite)
		bool DamagedSomeone = false;

	UFUNCTION(BlueprintCallable)
		void StoppingSphere();


	//---------------------------------------------------


	//----------------------------------Alien Shield Properties-----------------------------

	UPROPERTY(BlueprintAssignable)
		FActivateShieldSkill OnActivateShield_Del;
private:

	bool AlreadyUsedShieldIn_HalfHealth = false;
	bool AlreadyUsed_Shield_SecondTime = false;

	//------------------Alien Shield END----------------


public:

	//-------------------Getting Hit Reactions Montages--------------------------

	UPROPERTY()
		bool CantBeBreak = false;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Hit Reactions")
		UAnimMontage* Getting_Small_Reaction_1;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Hit Reactions")
		UAnimMontage* Getting_Medium_Reaction_1;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Hit Reactions")
		UAnimMontage* Getting_Medium_Reaction_2;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Hit Reactions")
		UAnimMontage* Getting_Medium_Reaction_3;


	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Hit Reactions")
		UAnimMontage* Getting_Large_Reaction_1;

	UFUNCTION()
		void FallingToGround();

	UPROPERTY(BlueprintReadWrite)
		bool IsFallingToGround = false;

};
