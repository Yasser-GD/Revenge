// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "Kar98.h"

#include "Player_GotDamageInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyHeroo.generated.h"

class USphereComponent;
class UCameraComponent;
class ATheBullet;
class UPhysicsHandleComponent;
class UBoxComponent;
class USoundCue;
//class AKar98;

UENUM(BlueprintType)
enum class EWeaponType : uint8 {
	None = 0,
	FlashLight = 1,
	Pistol = 2,
	Magnatic_Weapon = 3,
	Axe = 4,
	CarryingSecondaryItem = 5,

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeath);

//Left Mouse Button Pressed And Released 
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftMouseButtonClick);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftMouseButtonReleased);
//Right Mouse Button Pressed And Released 
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRightMouseButtonClick);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRightMouseButtonReleased);
//Mouse Wheel 
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMouseWheelGoingUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMouseWheelGoingDown);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOn_R_Pressed);





UCLASS()
class SHOOTINGGAME_API AMyHeroo : public ACharacter, public IPlayer_GotDamageInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyHeroo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Player_TakingDamage_Implementation() override;

	virtual void Player_TakingDamage_Pure(float DamageTaken, ACharacter* HittingActor) override;

	UPROPERTY(BlueprintReadWrite)
		bool IsMeleeDamagingON = false;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EWeaponType WeaponType;
	UPROPERTY(BlueprintAssignable)
		FOnPlayerDeath PlayerDeath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UCameraComponent* CameraFollow;
	UPROPERTY(EditAnywhere)
		class USpringArmComponent* CameraArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* InteractWithHandBox;


	//-- Movement Functions --
	UFUNCTION()
		void MoveForward(float Value);
	UFUNCTION()
		void MoveRight(float Value);

	UPROPERTY(EditAnywhere, Category = "Movement Speed")
		float WalkSpeed = 300.0f;
	UPROPERTY(EditAnywhere, Category = "Movement Speed")
		float SprintSpeed = 490.0f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hero States")
		float HeroHealth = 200.0f;
	UPROPERTY(EditAnywhere, Category = "Hero States")
		float HeroDamage = 10.0f;
	UPROPERTY(BlueprintReadOnly)
		bool bDead = false;

	UFUNCTION(BlueprintCallable)
		void Hero_TakeDamage(float DamageTaken, ACharacter* HittingActor);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Good Mode For The Developer..")
		bool Hero_CanBeDamaged = true;

	UPROPERTY(BlueprintReadwrite)
		bool HeroGotHit = false;

public:

	UPROPERTY()
		FOnLeftMouseButtonClick LeftMouseClick;
	UPROPERTY()
		FOnLeftMouseButtonReleased LeftMouseReleased;
	UPROPERTY()
		FOnRightMouseButtonClick RightMouseClick;
	UPROPERTY()
		FOnRightMouseButtonReleased RightMouseReleased;

	UPROPERTY()
		FMouseWheelGoingUp MouseWheelUpCast_Del;
	UPROPERTY()
		FMouseWheelGoingDown MouseWheelDownCast_Del;
	UPROPERTY()
		FOn_R_Pressed On_R_Pressed_Del;


public:

	//------------- Player Slots For Holding Things ------------------

	UPROPERTY(BlueprintReadWrite, Category = "Items Slots", meta = (AllowPrivateAccess = "true"))
		class AActor* AslotForItems_One;
	UPROPERTY(BlueprintReadWrite, Category = "Items Slots", meta = (AllowPrivateAccess = "true"))
		class  AActor* AslotForItems_Two;


	UPROPERTY(BlueprintReadWrite, Category = "Items Slots", meta = (AllowPrivateAccess = "true"))
		class  AActor* SlotFor_FlashLight;

private:

	UFUNCTION()
		void ToSwitchOn_WeaponTypes_SlotOne();
	UFUNCTION()
		void ToSwitchOn_WeaponTypes_SlotTwo();

public:

	UFUNCTION()
		void UseSlotTwo();
	UFUNCTION()
		void UseSlotOne();


	UFUNCTION()
		void ToEquip_SlotOne();

	UFUNCTION()
		void ToEquipSlotTwo();

	UFUNCTION()
		void UnEquip_SlotOne();
	UFUNCTION()
		void UnEquipSlotTwo();

	UFUNCTION()
		void DettachAnything();


	bool Slot_1_AlreadyEquiped = false;
	bool Slot_2_AlreadyEquiped = false;


	UFUNCTION()
		void FlashLightOn_Off();

	UPROPERTY(BlueprintReadOnly)
		bool IsFlashLightIsON = false;


public:

	UPROPERTY(BlueprintReadWrite, Category = "Items Slots", meta = (AllowPrivateAccess = "true"))
		class AActor* SecondaryItem_Slot_One;
	UPROPERTY(BlueprintReadWrite, Category = "Items Slots", meta = (AllowPrivateAccess = "true"))
		class AActor* SecondaryItem_Slot_Two;

	UFUNCTION()
		void Use_Secondary_SlotOne();
	UFUNCTION()
		void Use_Secondary_SlotTwo();

	UFUNCTION()
		void EquipSecondary_SlotOne();
	UFUNCTION()
		void UnEquipSecondary_SlotOne();
	UFUNCTION()
		void EquipSecondary_SlotTwo();
	UFUNCTION()
		void UnEquipSecondary_SlotTwo();

	bool SecondaryOne_IsEquiped = false;
	bool SecondaryTwo_IsEquiped = false;


	UFUNCTION()
		void ToUnEquipOnSwichItems();


	// --------------- Dashing Functionality ---------------------

	UFUNCTION()
		void Dash();

	FVector DashTarget;


	//----------------------------------------------------------------

private:
	//------------ Objects or Items Referances----------------

	UPROPERTY(EditAnywhere)
		class AFlashLight* FlashLight_MainRef;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool IsHoldingPistol = false;
	bool ToEndTick = false;

public:
	// ---Crouching Functions 

	void Crouching();
	void StopCrouching();

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
		bool IsCrouching = false;


	//Spawn Location And Rotation of The Bullets 
	FVector SpawnLocation;
	FRotator SpawnRotation;


	//-----


	// I Guess i Was Testing Grabing Something Using the Physics Handle Comp
	UPROPERTY()
		UPhysicsHandleComponent* TheItame = nullptr;

	UPROPERTY()
		class UPrimitiveComponent* Compo;

	bool IsGrabbingg;

	// -- i was testing The pickup Function 
	FVector End;

	// ---------------------


private:

	//--------------Blocking And Deflect Vars And Functions---------------- 

	//Attack Montages To Select in Blueprints And Attack Stuff

	UPROPERTY(EditAnywhere, Category = "AnimMontages")
		UAnimMontage* BlockAnim;
	UPROPERTY(BlueprintReadwrite, meta = (AllowPrivateAccess = "true"), Category = "AnimMontages")
		bool IsBlocking = false;

	UFUNCTION()
		void Start_Blocking();
	UFUNCTION()
		void End_Blocking();

	//Deflation Time And Enable And Desable
	UPROPERTY(EditAnyWhere, BlueprintReadwrite, meta = (AllowPrivateAccess = "true"), Category = "Blocking")
		float AbleToDeflectTime = 0.0;
	UPROPERTY(EditAnyWhere, BlueprintReadwrite, meta = (AllowPrivateAccess = "true"), Category = "Blocking")
		float EndDeflectTime = 2.0;

	UPROPERTY(BlueprintReadwrite, meta = (AllowPrivateAccess = "true"))
		bool AbleToDeflect = false;




	//------------ Blocking End ---------------


	UPROPERTY()
		class AFireDamageBox* PickUUUP;

	UFUNCTION()
		void Sprint();
	UFUNCTION()
		void Stop_Sprint();
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool IsSprinting = false;

	UPROPERTY()
		int32 NumToActivateeFlash = 1;


	// Dodging 

	UPROPERTY(EditAnywhere, Category = "Dodging Anim Montages")
		UAnimMontage* DodgeRight;
	UPROPERTY(EditAnywhere, Category = "Dodging Anim Montages")
		UAnimMontage* DodgeLeft;
	UPROPERTY(EditAnywhere, Category = "Dodging Anim Montages")
		UAnimMontage* DodgeForward;
	UPROPERTY(EditAnywhere, Category = "Dodging Anim Montages")
		UAnimMontage* DodgeBackward;


	UPROPERTY(EditAnywhere, Category = "Dodging Anim Montages")
		UAnimMontage* DodgeForward_Right;
	UPROPERTY(EditAnywhere, Category = "Dodging Anim Montages")
		UAnimMontage* DodgeForward_Left;
	UPROPERTY(EditAnywhere, Category = "Dodging Anim Montages")
		UAnimMontage* DodgeBackward_Right;
	UPROPERTY(EditAnywhere, Category = "Dodging Anim Montages")
		UAnimMontage* DodgeBackward_Left;


	//-----------

	UPROPERTY(EditAnywhere, Category = "Weapons AnimMontages")
		UAnimMontage* ShootingAnim;


public:

	// For The Sliding Animation -------


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speeding")
		bool IsSliding = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speeding")
		int LaunchNuminAir = 0;

	//---------------

	//To Booleans To Check Which weapens is Equiped 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsTheFirstMontageEnded = false;

	// ---- for focusing ---- 
	FVector MonsterLocation;
	FVector DistenceBetween;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsFoucsing = false;

	//UPROPERTY(EditAnywhere , BlueprintReadWrite)
	class AEnemyBoss* TheBlueMonster;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool UnEquipedRifle = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsPickingUpWeapens = false;

	//----Weapons References----

	UPROPERTY(BlueprintReadOnly)
		class AKar98* Kar98;
	UPROPERTY(BlueprintReadOnly)
		class ASwordd* AxeRef;

private:

	// On Using Any Item in the slot one or two
	UFUNCTION()
		void OnUseItem_LeftMouse();

	// On Released Any Item in the slot one or two
	UFUNCTION()
		void OnReleaseItem_LeftMouse();


	UFUNCTION()
		void Reload();


	// On Right Mouse Button Pressed And Released Functions 
	UFUNCTION()
		void OnRightMouseButton_Pressed();
	UFUNCTION()
		void OnRightMouseButton_Released();

	UFUNCTION()
		void OnMouseWheelUp();
	UFUNCTION()
		void OnMouseWheelDown();



	//------------------------


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool CanAttack_WithAxe = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool HitEndReset = false;

public:

	UPROPERTY(EditAnywhere)
		AActor* ActorThatAttachedToAimingSoc;

	//-------------- Sounds ---------------


public:

	// Static Mesh To Give To The Enemy To Reach Me While Attacking 
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Right_EnemyAttackReach;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Left_EnemyAttackReach;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Up_EnemyAttackReach;

private:

	//-- Player Interaction Function
	UFUNCTION()
		void InteractEverything();

	//-------------To Add On Screen Widget That says interact if the player is looking 
	UFUNCTION()
		void LineTraceToSayInteractOnScreen();

public:

	UPROPERTY(EditAnywhere)
		class UUserWidget* SayInteractWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UUserWidget> InteractWidget;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool AddWidgetOnScreen = false;

	//---------------------------------


	TMap<FGuid, UTexture*> guids;

};

