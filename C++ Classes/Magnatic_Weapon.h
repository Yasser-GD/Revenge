// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interact_Interface.h"
#include "EquipAndUn_Items_Interface.h"
#include "Player_OnUseInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Magnatic_Weapon.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGrabObject, UPrimitiveComponent*, HitComponent, FVector, GrabLoc);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReleaseObject);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjectDamage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnThrowObject);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerInteracted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGettingHarder , bool , IsStopping);


class USoundCue;


UCLASS()
class SHOOTINGGAME_API AMagnatic_Weapon : public AActor, public IInteract_Interface, public IEquipAndUn_Items_Interface, public IPlayer_OnUseInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMagnatic_Weapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Player Interact to pick Up 
	virtual void PlayerInteract() override;

	virtual void EquipItem() override;
	virtual void UnEquipItem() override;
	virtual void AmmoPickUp(float Amount , EAmmoType AmmoType) override;

	UPROPERTY()
		bool IsEquiped = false;

	virtual void DeAttachItem() override;

	virtual void OnUse() override;
	virtual void OnReleaseItem() override;


public:

	AActor* Owner;

	APawn* PlayerPawn;
	AMyHeroo* Heroo;


	UPROPERTY(BlueprintAssignable)
		FOnGrabObject OnGrabObject;
	UPROPERTY(BlueprintAssignable)
		FOnReleaseObject OnReleaseObject;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FOnObjectDamage OnObjectDamage;
	UPROPERTY(BlueprintAssignable)
		FOnThrowObject OnThrowDelegate;
	UPROPERTY(BlueprintAssignable)
		FPlayerInteracted PlayerInteracted;
	UPROPERTY(BlueprintAssignable)
		FOnGettingHarder GettingHarder_Del;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsGrabbingObject = false;

	//-----Line Trace Results
	UPROPERTY(BlueprintReadWrite)
		AActor* GrabbedActor;
	UPROPERTY(BlueprintReadWrite)
		UPrimitiveComponent* HitComponent;

	UPROPERTY(BlueprintReadWrite)
		FVector GrabLocation;

	// Direction To Shot Toward 
	UPROPERTY(BlueprintReadWrite)
		FVector DirectionToShotToward;

	// Location To Give TO The Blueprint To Try To Perdict THe Direction of The Projcetile
	UPROPERTY(BlueprintReadWrite)
		FVector PerdictedShotLocation;

	UPROPERTY(BlueprintReadWrite)
		FVector StartofPerdicting;

	// Attach And Dettach Weapon Functions
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void AttachWeapon(AMyHeroo* TargetCharacter);
	UFUNCTION()
		void DeAttachWeapon();

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
		FVector MuzzleOffset;



private:

	//-------------------------- Weapon Functionallty ----------------------

	UFUNCTION()
		void OnGrab();
	UFUNCTION()
		void OnGrabRelease();

	UFUNCTION()
		void OnHeldToThrow();
	UFUNCTION()
		void OnThrow();

	UPROPERTY(EditAnywhere, Category = "Garb And Throw Functionlty")
		float Impulsestrength = 20;
	UPROPERTY(EditAnywhere, Category = "Throw Damage")
		float ThrowDamage = 30.0;

	UPROPERTY(EditAnywhere, Category = "Garb And Throw Functionlty")
		float DefaultImpulse = 20;
	UPROPERTY(EditAnywhere, Category = "Throw Damage")
		float DefaultThrowDamage = 30.0;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool PressedToThrow = false;

	UFUNCTION()
		void ObjectGetHarder();
	FTimerHandle GetHarderTimer;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UActorComponent> RotationComponent;

	class URotatingMovementComponent* RotatingMovementCastedRef;

	float RotationRate = 45.0;


private:

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UPhysicsHandleComponent* PhysicsHandleCompObject_;

	/** The Character holding this weapon*/
	class AMyHeroo* HeroRef;

	APlayerController* PlayerController;

	UFUNCTION()
		void OnGrabbedHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		void OnReleaseGrabbed();

	// Delay Before Grabbing Again
	UPROPERTY()
		bool Delaying = false;

	UPROPERTY()
		bool AlreadyHit = false;

	UPROPERTY()
		bool DoOnce_AddDynamic = false;


public:

	// --------------------Sounds----------------------------

	UPROPERTY()
		USoundCue* GrabSound;

	UPROPERTY(EditAnyWhere, Category = "Sounds")
		USoundCue* RelesingSound;

	UPROPERTY(EditAnyWhere, Category = "Sounds")
		USoundCue* GetHarderSound;

	UPROPERTY()
		UAudioComponent* GetHarderAudioComponent;

	UPROPERTY()
		bool AlreadyCreatedSound = false;
	UPROPERTY()
		bool AlreadyPlayedSound = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USoundCue* ImpactSound1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USoundCue* ImpactSound2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USoundCue* ImpactSound_OfHittingPawn;

	bool PlayedFirstSound = false;
	bool IsThrowing = false;


};
