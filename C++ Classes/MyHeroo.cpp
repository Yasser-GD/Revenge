// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHeroo.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "TheBullet.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "FireDamageBox.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/SpringArmComponent.h"
#include "Swordd.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "EnemyBoss.h"
#include "Components/BoxComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Kar98.h"
#include "KarAmmoPickUp.h"
#include "Sound/SoundCue.h"
#include "AmmoPickUp.h"
#include "Components/SpotLightComponent.h"
#include "FlashLight.h"

#include "Blueprint/UserWidget.h"

#include "Interact_Interface.h"
#include "EquipAndUn_Items_Interface.h"
#include "Damage_Systeem_InterFac.h"
#include "Player_OnUseInterface.h"

#include "Misc/MessageDialog.h"


#define PrintString(String) GEngine->AddOnScreenDebugMessage(-1 , 5.0f , FColor::Cyan ,String);



// Sets default values
AMyHeroo::AMyHeroo()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(40.0f, 95.0f);
	/*CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Arm"));
	CameraArm->TargetArmLength = 300.0f;
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->bUsePawnControlRotation = true;*/


	CameraFollow = CreateDefaultSubobject<UCameraComponent>(TEXT("The Camera"));
	CameraFollow->SetupAttachment(GetMesh(), FName("CameraPlaceCrouching"));
	CameraFollow->bUsePawnControlRotation = false;

	TheItame = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));

	InteractWithHandBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hand Interact"));
	InteractWithHandBox->SetupAttachment(GetMesh(), FName("LeftHand"));
	/*BoxToTestFocusing = CreateDefaultSubobject<UBoxComponent>(TEXT("FocusingBox"));
	BoxToTestFocusing->SetupAttachment(GetCapsuleComponent());*/

	Right_EnemyAttackReach = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh for Enemy Attack Reach"));
	Right_EnemyAttackReach->SetupAttachment(GetMesh());
	Left_EnemyAttackReach = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Left Attack Reach"));
	Left_EnemyAttackReach->SetupAttachment(GetMesh());
	Up_EnemyAttackReach = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Up Attack Reach "));
	Up_EnemyAttackReach->SetupAttachment(GetMesh());
}

// Called when the game starts or when spawned
void AMyHeroo::BeginPlay()
{

	Super::BeginPlay();

	//The Normal Speed of the player
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	//To Hide the head bone
	GetMesh()->HideBoneByName("head", EPhysBodyOp::PBO_None);

	WeaponType = EWeaponType::None;

	FVector CameraLoc = GetMesh()->GetSocketLocation(FName("CameraPlaceCrouching"));
	CameraFollow->SetWorldLocation(CameraLoc);


	/*for (TObjectIterator<UTexture> it; it; ++it)
	{
		if (!it->GetLightingGuid().IsValid())
		{
			continue;
		}

		UTexture** result = guids.Find(it->GetLightingGuid());
		if (result == nullptr)
		{
			guids.Add(it->GetLightingGuid(), *it);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s: Texture has had its Guid updated."), *it->GetName());
			it->SetLightingGuid();
			it->Modify();
		}
	}

	FText DialogText = FText::FromString("Done, save all to save all");
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);*/



}

// Called every frame
void AMyHeroo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bDead) {



		if (GetMesh()->GetComponentRotation().Roll > 0 || GetMesh()->GetComponentRotation().Pitch > 0) {
			//GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Cyan, FString::Printf(TEXT("The Hero Rotation Adjusted ")));
			FRotator RotationIfTheMeshIsWrong = FRotator(0.0, GetActorRotation().Yaw, 0.0);
			SetActorRotation(RotationIfTheMeshIsWrong);
		}


		// Deflection Enable And Dsable Time
		if (IsBlocking && WeaponType == EWeaponType::Axe && AbleToDeflectTime < EndDeflectTime) {
			AbleToDeflectTime += DeltaTime;
			AbleToDeflect = true;

		}
		else {
			AbleToDeflect = false;
		}

		//PrintString(FString::Printf(TEXT(" Current Acceleration : %s "), *GetCharacterMovement()->GetCurrentAcceleration().ToString()));s

		//Function To Say Interact When the player is looking 
		//LineTraceToSayInteractOnScreen();

		//GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Cyan, FString::Printf(TEXT(" Velocity : %f "), GetMesh()->GetAnimInstance()->CalculateDirection(GetVelocity(), GetActorRotation())));

		

	}

}

// Called to bind functionality to input
void AMyHeroo::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyHeroo::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyHeroo::MoveRight);

	PlayerInputComponent->BindAxis("LookRight", this, &ACharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ACharacter::AddControllerYawInput);

	// Jumping Action 
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Crouching State
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMyHeroo::Crouching);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMyHeroo::StopCrouching);

	//Use Weapons 
	PlayerInputComponent->BindAction("UseSlotOne", IE_Pressed, this, &AMyHeroo::UseSlotOne);
	PlayerInputComponent->BindAction("UseSlotTwo", IE_Pressed, this, &AMyHeroo::UseSlotTwo);

	PlayerInputComponent->BindAction("Dettach anything", IE_Pressed, this, &AMyHeroo::DettachAnything);

	//FlashLight Bindings 
	PlayerInputComponent->BindAction("FlashLightONAndOFF", IE_Pressed, this, &AMyHeroo::FlashLightOn_Off);

	//Sprinting Input Bindings 
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMyHeroo::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMyHeroo::Stop_Sprint);

	// Shooting With A Weapon
	PlayerInputComponent->BindAction("Left Mouse Button", IE_Pressed, this, &AMyHeroo::OnUseItem_LeftMouse);
	PlayerInputComponent->BindAction("Left Mouse Button", IE_Released, this, &AMyHeroo::OnReleaseItem_LeftMouse);

	//PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AMyHeroo::AttackWith_Axe);
	// Reloading 
	PlayerInputComponent->BindAction("Reload Weapon", IE_Pressed, this, &AMyHeroo::Reload);

	// Aiming 
	PlayerInputComponent->BindAction("Right Mouse Button", IE_Pressed, this, &AMyHeroo::OnRightMouseButton_Pressed);
	PlayerInputComponent->BindAction("Right Mouse Button", IE_Released, this, &AMyHeroo::OnRightMouseButton_Released);

	//-- Player Interaction With Anything 
	PlayerInputComponent->BindAction("Player Interact", IE_Pressed, this, &AMyHeroo::InteractEverything);

	// Player Blocking 
	PlayerInputComponent->BindAction("Right Mouse Button", IE_Pressed, this, &AMyHeroo::Start_Blocking);
	PlayerInputComponent->BindAction("Right Mouse Button", IE_Released, this, &AMyHeroo::End_Blocking);

	PlayerInputComponent->BindAction("Secondary Item Slot One", IE_Pressed, this, &AMyHeroo::Use_Secondary_SlotOne);
	PlayerInputComponent->BindAction("Secondary Item Slot Two", IE_Pressed, this, &AMyHeroo::Use_Secondary_SlotTwo);

	PlayerInputComponent->BindAction("Mouse Wheel Up", IE_Pressed, this, &AMyHeroo::OnMouseWheelUp);
	PlayerInputComponent->BindAction("Mouse Wheel Down", IE_Pressed, this, &AMyHeroo::OnMouseWheelDown);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AMyHeroo::Dash);


}

void AMyHeroo::Player_TakingDamage_Implementation()
{
}

void AMyHeroo::Player_TakingDamage_Pure(float DamageTaken, ACharacter* HittingActor)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Cyan, FString::Printf(TEXT(" Called The Player Take Damage in c++ Class")));
	if (!bDead) {

		if (!IsBlocking && Hero_CanBeDamaged) {

			HeroHealth -= DamageTaken;
			HeroGotHit = true;

		}

		if (AbleToDeflect) {
			IDamage_Systeem_InterFac* WhosAttacking = Cast<IDamage_Systeem_InterFac>(HittingActor);
			if (WhosAttacking) {
				WhosAttacking->Deflection_impect();
			}

		}

		//To Check If The Player Health Is 0 
		if (HeroHealth <= 0) {
			/*GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetMesh()->SetCollisionProfileName("Ragdoll");
			GetMesh()->SetSimulatePhysics(true);*/
			bDead = true;
			PlayerDeath.Broadcast();

			DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));

		}

	}

}


// ----- Movement Functions ------ 
void AMyHeroo::MoveForward(float Value)
{
	if (!bDead) {
		FVector TDiraction = FRotationMatrix(GetController()->GetControlRotation()).GetScaledAxis(EAxis::X);
		FVector Diraction = GetActorForwardVector();
		AddMovementInput(TDiraction, Value);

	}

}

void AMyHeroo::MoveRight(float Value)
{

	if (!bDead) {
		FVector TDiraction = FRotationMatrix(GetController()->GetControlRotation()).GetScaledAxis(EAxis::Y);
		FVector Diraction = GetActorRightVector();
		AddMovementInput(TDiraction, Value);
		//GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Cyan, FString::Printf(TEXT(" Walking Value :%f "),Value ));
	}

}



void AMyHeroo::Crouching()
{
	if (!bDead) {

		ACharacter::Crouch();
		FAttachmentTransformRules Rules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
		CameraFollow->AttachToComponent(GetMesh(), Rules, FName("CameraLocCroucing"));
		//GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Cyan, FString::Printf(TEXT(" Crouch ")));
		IsCrouching = true;
		//GetCapsuleComponent()->InitCapsuleSize(35.0, 45.0);


	}
}

void AMyHeroo::StopCrouching()
{

	ACharacter::UnCrouch();
	FAttachmentTransformRules Rules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
	//GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Cyan, FString::Printf(TEXT(" Stop Crouching ")));
	IsCrouching = false;

	CameraFollow->AttachToComponent(GetMesh(), Rules, FName("CameraPlaceCrouching"));
	//GetCapsuleComponent()->InitCapsuleSize(51.0, 95.0);
	//CameraFollow->SetRelativeLocation(CameraRelativeLocaiton);

}
//--------------------------


void AMyHeroo::Start_Blocking()
{
	if (WeaponType == EWeaponType::None || WeaponType == EWeaponType::FlashLight || WeaponType == EWeaponType::Axe) {
		//GEngine->AddOnScreenDebugMessage(-1, 6.0, FColor::Blue, FString::Printf(TEXT("Hero Is Blocking Now")));
		IsBlocking = true;

	}
}

void AMyHeroo::End_Blocking()
{
	if (WeaponType == EWeaponType::None || WeaponType == EWeaponType::FlashLight || WeaponType == EWeaponType::Axe) {
		//GEngine->AddOnScreenDebugMessage(-1, 6.0, FColor::Blue, FString::Printf(TEXT("Hero Is not Blocking Now")));
		IsBlocking = false;
		AbleToDeflectTime = 0.0;
	}

}

//-------------- Sprinting----------
void AMyHeroo::Sprint()
{

	if (!bDead) {
		//GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Blue, FString::Printf(TEXT("Sprinting Pressed")));
		IsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

	}

}

void AMyHeroo::Stop_Sprint()
{

	if (!bDead) {
		//GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Blue, FString::Printf(TEXT("Sprinting Released")));
		IsSprinting = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	}

}

//----------Hero Taking Damage-----------------
void AMyHeroo::Hero_TakeDamage(float DamageTaken, ACharacter* HittingActor)
{
	if (!IsBlocking && Hero_CanBeDamaged) {

		HeroHealth -= DamageTaken;
		HeroGotHit = true;

	}

	if (AbleToDeflect) {
		IDamage_Systeem_InterFac* WhosAttacking = Cast<IDamage_Systeem_InterFac>(HittingActor);
		if (WhosAttacking) {
			WhosAttacking->Deflection_impect();
		}

	}

}


void AMyHeroo::OnUseItem_LeftMouse()
{

	//GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, FString::Printf(TEXT(" Left Mouse Button Pressed")));
	LeftMouseClick.Broadcast();

}

void AMyHeroo::OnReleaseItem_LeftMouse()
{

	LeftMouseReleased.Broadcast();

}


void AMyHeroo::Reload()
{

	On_R_Pressed_Del.Broadcast();

}


void AMyHeroo::OnRightMouseButton_Pressed()
{
	RightMouseClick.Broadcast();

}

void AMyHeroo::OnRightMouseButton_Released()
{

	RightMouseReleased.Broadcast();

}

void AMyHeroo::OnMouseWheelUp()
{
	MouseWheelUpCast_Del.Broadcast();
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, FString::Printf(TEXT(" Mouse Wheel Cast  ")));
}

void AMyHeroo::OnMouseWheelDown()
{
	MouseWheelDownCast_Del.Broadcast();
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, FString::Printf(TEXT(" Mouse Wheel Cast  ")));
}



// Sprinting Functions 

//---------------------


void AMyHeroo::ToSwitchOn_WeaponTypes_SlotOne()
{

	if (AslotForItems_One->ActorHasTag("Pistol")) {

		WeaponType = EWeaponType::Pistol;
		//GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, FString::Printf(TEXT(" He Set The State To Pistol ")));
	}
	else if (AslotForItems_One->ActorHasTag("Axe")) {
		WeaponType = EWeaponType::Axe;
	}
	else if (AslotForItems_One->ActorHasTag("Magnatic Weapon")) {
		WeaponType = EWeaponType::Magnatic_Weapon;
	}

}

void AMyHeroo::ToSwitchOn_WeaponTypes_SlotTwo()
{
	if (AslotForItems_Two->ActorHasTag("Pistol")) {

		WeaponType = EWeaponType::Pistol;
	}
	else if (AslotForItems_Two->ActorHasTag("Axe")) {
		WeaponType = EWeaponType::Axe;
	}
	else if (AslotForItems_Two->ActorHasTag("Magnatic Weapon")) {
		WeaponType = EWeaponType::Magnatic_Weapon;
	}

}


//Function to Equip The Rifle  
void AMyHeroo::UseSlotTwo()
{

	//GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Cyan, FString::Printf(TEXT(" Number 2 Is Pressed And Found Slot 22222 ")));
	if (!bDead) {

		if (AslotForItems_Two) {

			if (!Slot_2_AlreadyEquiped) {

				// To UnEquip Whatever is on hand 
				ToUnEquipOnSwichItems();

				ToSwitchOn_WeaponTypes_SlotTwo();

				ToEquipSlotTwo();
				Slot_2_AlreadyEquiped = true;

			}
			else {
				UnEquipSlotTwo();
				WeaponType = EWeaponType::None;
				Slot_2_AlreadyEquiped = false;

				//if You Try To UnEquip The Slot And The FlashLight is ON He Will Switch To FlashLight State 
				if (IsFlashLightIsON) {
					WeaponType = EWeaponType::FlashLight;

				}

			}

			//To ReEquip or UnEquip The FlashLigth
			if (SlotFor_FlashLight) {

				IEquipAndUn_Items_Interface* EquipAble_Interface = Cast<IEquipAndUn_Items_Interface>(SlotFor_FlashLight);
				if (EquipAble_Interface) {
					if (IsFlashLightIsON) {

						EquipAble_Interface->EquipItem();

					}
					else if (!IsFlashLightIsON) {
						EquipAble_Interface->UnEquipItem();
					}

				}

			}

		}

	}

}

void AMyHeroo::UseSlotOne()
{


	if (!bDead) {
		if (AslotForItems_One) {

			if (!Slot_1_AlreadyEquiped) {

				// To UnEquip Whatever is on hand 
				ToUnEquipOnSwichItems();

				ToSwitchOn_WeaponTypes_SlotOne();

				//GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Cyan, FString::Printf(TEXT(" He Got into the slot one")));
				ToEquip_SlotOne();
				Slot_1_AlreadyEquiped = true;
			}
			else {
				UnEquip_SlotOne();
				WeaponType = EWeaponType::None;
				Slot_1_AlreadyEquiped = false;

				//if You Try To UnEquip The Slot And The FlashLight is ON He Will Switch To FlashLight State 
				if (IsFlashLightIsON) {
					WeaponType = EWeaponType::FlashLight;

				}

			}

			//To ReEquip or UnEquip The FlashLigth
			if (SlotFor_FlashLight) {

				IEquipAndUn_Items_Interface* EquipAble_Interface = Cast<IEquipAndUn_Items_Interface>(SlotFor_FlashLight);
				if (EquipAble_Interface) {
					if (IsFlashLightIsON) {

						EquipAble_Interface->EquipItem();

					}
					else if (!IsFlashLightIsON) {
						EquipAble_Interface->UnEquipItem();
					}

				}

			}


		}

	}

}

void AMyHeroo::ToEquip_SlotOne()
{
	IEquipAndUn_Items_Interface* EquipAble_Interface = Cast<IEquipAndUn_Items_Interface>(AslotForItems_One);
	if (EquipAble_Interface) {
		EquipAble_Interface->EquipItem();
	}

}

void AMyHeroo::ToEquipSlotTwo()
{

	IEquipAndUn_Items_Interface* EquipAble_Interface = Cast<IEquipAndUn_Items_Interface>(AslotForItems_Two);
	if (EquipAble_Interface) {
		EquipAble_Interface->EquipItem();
	}

}

void AMyHeroo::UnEquip_SlotOne()
{

	IEquipAndUn_Items_Interface* EquipAble_Interface = Cast<IEquipAndUn_Items_Interface>(AslotForItems_One);
	if (EquipAble_Interface) {
		EquipAble_Interface->UnEquipItem();
	}


}

void AMyHeroo::UnEquipSlotTwo()
{

	IEquipAndUn_Items_Interface* EquipAble_Interface = Cast<IEquipAndUn_Items_Interface>(AslotForItems_Two);
	if (EquipAble_Interface) {
		EquipAble_Interface->UnEquipItem();
	}

}

void AMyHeroo::DettachAnything()
{

	if (Slot_1_AlreadyEquiped) {

		IEquipAndUn_Items_Interface* EquipAble_Interface = Cast<IEquipAndUn_Items_Interface>(AslotForItems_One);
		if (EquipAble_Interface) {
			EquipAble_Interface->DeAttachItem();

			AslotForItems_One = nullptr;
			WeaponType = EWeaponType::None;
		}
	}
	else if (Slot_2_AlreadyEquiped) {

		IEquipAndUn_Items_Interface* EquipAble_Interface = Cast<IEquipAndUn_Items_Interface>(AslotForItems_Two);
		if (EquipAble_Interface) {
			EquipAble_Interface->DeAttachItem();
			
			AslotForItems_Two = nullptr;
			WeaponType = EWeaponType::None;
		}
	}


}




void AMyHeroo::FlashLightOn_Off()
{

	//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Cyan, FString::Printf(TEXT("FlashLight ON")));
	if (!IsFlashLightIsON && SlotFor_FlashLight) {

		//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Cyan, FString::Printf(TEXT("FlashLight ON")));

		IEquipAndUn_Items_Interface* EquipAble_Interface = Cast<IEquipAndUn_Items_Interface>(SlotFor_FlashLight);
		if (EquipAble_Interface) {
			if (WeaponType == EWeaponType::None) {
				WeaponType = EWeaponType::FlashLight;
			}

			EquipAble_Interface->EquipItem();

			//To Turn ON The Light
			FlashLight_MainRef = Cast<AFlashLight>(SlotFor_FlashLight);

			if (FlashLight_MainRef) {
				FlashLight_MainRef->FlashLight->SetVisibility(true);
			}

			IsFlashLightIsON = true;
		}


	}
	else if (IsFlashLightIsON && SlotFor_FlashLight) {

		IEquipAndUn_Items_Interface* EquipAble_Interface = Cast<IEquipAndUn_Items_Interface>(SlotFor_FlashLight);
		if (EquipAble_Interface) {

			if (WeaponType == EWeaponType::FlashLight) {
				WeaponType = EWeaponType::None;

			}

			EquipAble_Interface->UnEquipItem();

			//To Turn OFF The Light
			FlashLight_MainRef = Cast<AFlashLight>(SlotFor_FlashLight);

			if (FlashLight_MainRef) {
				FlashLight_MainRef->FlashLight->SetVisibility(false);

			}

			IsFlashLightIsON = false;
		}

		//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Cyan, FString::Printf(TEXT("FlashLight OFF")));
	}

}


void AMyHeroo::Use_Secondary_SlotOne()
{

	if (SecondaryItem_Slot_One) {

		if (!SecondaryOne_IsEquiped) {
			//GEngine->AddOnScreenDebugMessage(-1, 6.0, FColor::Cyan, FString::Printf(TEXT("He Equiped Slot One Secondary")));

			// To UnEquip Whatever is on hand 
			ToUnEquipOnSwichItems();
			WeaponType = EWeaponType::CarryingSecondaryItem;
			EquipSecondary_SlotOne();
			SecondaryOne_IsEquiped = true;
		}
		else {
			UnEquipSecondary_SlotOne();
			SecondaryOne_IsEquiped = false;
			WeaponType = EWeaponType::None;
		}
	}
}

void AMyHeroo::Use_Secondary_SlotTwo()
{

	if (SecondaryItem_Slot_Two) {

		if (SecondaryTwo_IsEquiped) {

			// To UnEquip Whatever is on hand 
			ToUnEquipOnSwichItems();

			EquipSecondary_SlotTwo();
			SecondaryTwo_IsEquiped = true;
			WeaponType = EWeaponType::CarryingSecondaryItem;
		}
		else {
			UnEquipSecondary_SlotTwo();
			SecondaryTwo_IsEquiped = false;
			WeaponType = EWeaponType::None;
		}

	}
}


void AMyHeroo::EquipSecondary_SlotOne()
{
	IEquipAndUn_Items_Interface* EquipSecondary_One_Interface = Cast<IEquipAndUn_Items_Interface>(SecondaryItem_Slot_One);

	if (EquipSecondary_One_Interface) {
		EquipSecondary_One_Interface->EquipItem();
	}

}

void AMyHeroo::UnEquipSecondary_SlotOne()
{
	IEquipAndUn_Items_Interface* EquipSecondary_One_Interface = Cast<IEquipAndUn_Items_Interface>(SecondaryItem_Slot_One);

	if (EquipSecondary_One_Interface) {
		EquipSecondary_One_Interface->UnEquipItem();
	}

}

void AMyHeroo::EquipSecondary_SlotTwo()
{

	IEquipAndUn_Items_Interface* EquipSecondary_One_Interface = Cast<IEquipAndUn_Items_Interface>(SecondaryItem_Slot_Two);

	if (EquipSecondary_One_Interface) {
		EquipSecondary_One_Interface->EquipItem();
	}

}

void AMyHeroo::UnEquipSecondary_SlotTwo()
{

	IEquipAndUn_Items_Interface* EquipSecondary_One_Interface = Cast<IEquipAndUn_Items_Interface>(SecondaryItem_Slot_Two);

	if (EquipSecondary_One_Interface) {
		EquipSecondary_One_Interface->UnEquipItem();
	}

}

void AMyHeroo::ToUnEquipOnSwichItems()
{

	//if Something on slot one is Equiped
	if (Slot_1_AlreadyEquiped) {
		Slot_1_AlreadyEquiped = false;
		UnEquip_SlotOne();

	}
	//if Something on slot Two is Equiped
	else if (Slot_2_AlreadyEquiped) {
		UnEquipSlotTwo();
		Slot_2_AlreadyEquiped = false;

	}
	//if Something on Secondary Slot One is Equiped
	else if (SecondaryOne_IsEquiped) {
		UnEquipSecondary_SlotOne();
		SecondaryOne_IsEquiped = false;
	}
	//if Something on Secondary Slot Two is Equiped
	else if (SecondaryTwo_IsEquiped) {
		UnEquipSecondary_SlotTwo();
		SecondaryTwo_IsEquiped = false;

	}

}

void AMyHeroo::Dash()
{

	if (!bDead) {

		//GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Cyan, FString::Printf(TEXT(" Velocity : %f "), GetMesh()->GetAnimInstance()->CalculateDirection(GetVelocity(), GetActorRotation())));
		float Direction = GetMesh()->GetAnimInstance()->CalculateDirection(GetVelocity(), GetActorRotation());
		

		//Dodging Forward
		if (Direction >= 0 && Direction <= 10 || Direction <= -0 && Direction >= -10 ) {
			PlayAnimMontage(DodgeForward);
		}
		//Dodging Backward
		else if (Direction >= 170 && Direction <= 180 || Direction <= -170 && Direction >= -180) {
			PlayAnimMontage(DodgeBackward);
		}
		//Dodging Right 
		else if (Direction >= 78 && Direction <= 100 ) {
			PlayAnimMontage(DodgeRight);
		}
		//Dodging Left
		else if (Direction >= -100 && Direction <= -78) {
			PlayAnimMontage(DodgeLeft);
		}


		//Dodging Forward Right 
		else if ( Direction <= 55 && Direction >= 35) {
			PlayAnimMontage(DodgeForward_Right);
		}
		//Dodging Forward Left 
		else if (Direction <= -35 && Direction >= -57) {
			PlayAnimMontage(DodgeForward_Left);
		}
		//Dodging Backward Right 
		else if (Direction <= 145 && Direction >= 124) {
			PlayAnimMontage(DodgeBackward_Right);
		}
		//Dodging Backward Left 
		else if (Direction >= -145 && Direction <= -124) {
			PlayAnimMontage(DodgeBackward_Left);
		}

	}

}


//-- Picking UP Anything
void AMyHeroo::InteractEverything() {

	//GEngine->AddOnScreenDebugMessage(-1, 6.0, FColor::Cyan, FString::Printf(TEXT("He Pressed The Interact Function ")));

	TArray<FHitResult> Hits;
	FCollisionQueryParams Params;
	FVector TraceStart = CameraFollow->GetComponentLocation();
	FVector TraceEnd = TraceStart + CameraFollow->GetForwardVector() * 250;


	bool FHit = GetWorld()->LineTraceMultiByChannel(Hits, TraceStart, TraceEnd, ECC_Camera, Params);
	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Purple, false, 2.0, -1, 3.0);

	if (FHit) {

		for (FHitResult HitActor : Hits) {

			//--if We Made A Inventory Slots (AActors) We Can Assign The Equipable Actor To One Of Them
			IInteract_Interface* Interact = Cast<IInteract_Interface>(HitActor.GetActor());
			if (Interact) {

				if (HitActor.GetActor()->ActorHasTag("Equipable")) {

					if (AslotForItems_One == nullptr) {
						GEngine->AddOnScreenDebugMessage(-1, 6.0, FColor::Cyan, FString::Printf(TEXT("ASlot For Item Equiped Something ")));
						AslotForItems_One = HitActor.GetActor();

					}
					else if (AslotForItems_Two == nullptr) {
						AslotForItems_Two = HitActor.GetActor();

					}

				}
				else if (HitActor.GetActor()->ActorHasTag("FlashLight")) {

					SlotFor_FlashLight = HitActor.GetActor();
					//GEngine->AddOnScreenDebugMessage(-1, 6.0, FColor::Cyan, FString::Printf(TEXT("Got The Flash Light Slot Referance")));

				}


				///----------------- To Assign A Secondary Item To The Secondary Slots -----------------
				if (HitActor.GetActor()->ActorHasTag("Secondary Item")) {

					if (SecondaryItem_Slot_One == nullptr) {

						if (SecondaryItem_Slot_Two && HitActor.GetActor()->GetClass() != SecondaryItem_Slot_Two->GetClass()) {

							SecondaryItem_Slot_One = HitActor.GetActor();
							GEngine->AddOnScreenDebugMessage(-1, 6.0, FColor::Cyan, FString::Printf(TEXT("Assigned Something Two Secondary Slot One  ")));
						}
						else if (SecondaryItem_Slot_Two == nullptr) {
							SecondaryItem_Slot_One = HitActor.GetActor();
						}

					}
					else if (SecondaryItem_Slot_Two == nullptr) {

						if (SecondaryItem_Slot_One && HitActor.GetActor()->GetClass() != SecondaryItem_Slot_One->GetClass()) {

							SecondaryItem_Slot_Two = HitActor.GetActor();
							GEngine->AddOnScreenDebugMessage(-1, 6.0, FColor::Cyan, FString::Printf(TEXT("Assigned Something Two Secondary Slot One  ")));
						}

					}

				}

				//PrintString(FString::Printf(TEXT("The Item Has The Interact Interface : %s"), *HitActor.GetActor()->GetName()));
				Interact->Execute_Interact(HitActor.GetActor());
				Interact->PlayerInteract();


			}
		}

	}

}

void AMyHeroo::LineTraceToSayInteractOnScreen()
{

	//TArray<FHitResult> Hits;
	FHitResult Hit;
	FCollisionQueryParams Params;
	FVector TraceStart = CameraFollow->GetComponentLocation();
	FVector TraceEnd = TraceStart + CameraFollow->GetForwardVector() * 150;


	bool FHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);

	if (FHit) {

		if (Hit.GetActor()->GetClass()->ImplementsInterface(UInteract_Interface::StaticClass()) && AddWidgetOnScreen) {

			GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Blue, FString::Printf(TEXT("He Should Add The Widget On Screen ")));
			if (SayInteractWidget) {
				AddWidgetOnScreen = false;
				SayInteractWidget->AddToViewport();
			}

		}
		else if (!AddWidgetOnScreen) {
			AddWidgetOnScreen = true;
			if (SayInteractWidget) {

				SayInteractWidget->RemoveFromParent();
			}

		}
	}
	else {

		if (SayInteractWidget && !AddWidgetOnScreen) {
			SayInteractWidget->RemoveFromParent();
		}

	}

}

