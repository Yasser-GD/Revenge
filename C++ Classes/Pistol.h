// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interact_Interface.h"
#include "EquipAndUn_Items_Interface.h"
#include "Player_OnUseInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pistol.generated.h"

class UBoxComponent;
class USphereComponent;
class ATheBullet;
class UNiagaraSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShooting, int, OwnedBulletsNum, int, MaxBulletsNum);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquip_Pistol, bool , IsPistol_Equiped );

UCLASS()
class SHOOTINGGAME_API APistol : public AActor, public IInteract_Interface, public IEquipAndUn_Items_Interface, public IPlayer_OnUseInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APistol();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	
//Player Interact to pick Up 
	virtual void PlayerInteract() override;

	virtual void EquipItem() override;
	virtual void UnEquipItem() override;
	virtual void AmmoPickUp(float Amount , EAmmoType AmmoType) override;

	bool IsEquiped = false;
	
	UFUNCTION()
	void FToAttachWeapen();
	
	virtual void DeAttachItem() override;

	virtual void OnUse() override;
	virtual void OnReleaseItem() override;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		USkeletalMeshComponent* PistolMesh;
	
	UPROPERTY(EditAnywhere)
		UBoxComponent* BoxToPickUpPistol;
	
	UPROPERTY(EditAnywhere , Category = "Anim Montages")
		UAnimationAsset* ShootingAnimAsset;
	
	UPROPERTY()
		APawn* PlayerPawn;
	UPROPERTY()
		class AMyHeroo* Heroo;
	
private:

	UPROPERTY(BlueprintAssignable)
		FOnShooting OnShootingWithPistol;

	UPROPERTY(BlueprintAssignable)
		FOnEquip_Pistol OnEquip_Pistol;

public:

	//-------------------- Aiming Properties -----------------------
	
	UFUNCTION()
		void Aiming_Pressed();
	UFUNCTION()
		void Aiming_Released();
	
	UPROPERTY(EditAnywhere, BlueprintReadwrite)
		bool PressedToAim = false;

	float LerpDuration = 3;
	float TimeElasped = 0;

	FVector AimingSocketLocation;

	FVector CameraDefaultLocation = FVector::ZeroVector;
	FVector CameraRelativeLocaiton = FVector::ZeroVector;

	//-------- END --------- Aiming Properties -----------------------------

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo" )
		int PistolBulletsNum = 12;
	UPROPERTY(EditAnywhere, Category = "Ammo")
		int NumBulletsCanBeLoaded = 12;

	//will Be zero 0
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Ammo")
		int MaxPistolBullets = 50;

	UFUNCTION()
		void Reload();

	bool IsReloading = false;

	UPROPERTY(EditAnywhere , Category = " Anim Montages")
		UAnimMontage* Shooting_Montage;

	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	    EAmmoType PistolAmmoType;
	
private:
	
	bool BoxAlreadyUnVisible = false;

	UPROPERTY(EditAnywhere, Category = "Projectille")
		USphereComponent* SphereToSpawnBullets;
	
	UPROPERTY(EditAnywhere, Category = "Projectille")
		TSubclassOf<AActor> Bullets;

public:

	UPROPERTY()
		bool PlayerIsOverlapping = false;

	FVector SpawnBulletsSocketLocation;
	FRotator SpawnBulletRotation;

	FVector SpawnLoc;

private:


	UPROPERTY(EditAnywhere , Category = "Projectille")
		UNiagaraSystem* Shoot_NiagaraEffect;

	//----- Sounds 

	UPROPERTY(EditAnywhere, Category = "Sounds")
		class USoundCue* ReloadingSoundCue;

	UPROPERTY(EditAnywhere, Category = "Sounds")
		USoundWave* ShootSoundCue;
	


	
};
