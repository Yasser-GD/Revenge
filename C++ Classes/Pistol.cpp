// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyHeroo.h"
#include "TheBullet.h"
#include "AmmoPickUp.h"
#include "NiagaraFunctionLibrary.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimMontage.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "Damage_Systeem_InterFac.h"


// Sets default values
APistol::APistol()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PistolMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PistolMesh"));
	BoxToPickUpPistol = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxToPickUpRifle"));
	BoxToPickUpPistol->SetupAttachment(PistolMesh);

	SphereToSpawnBullets = CreateDefaultSubobject <USphereComponent>(TEXT("Sphere To Spawn Bullets"));
	SphereToSpawnBullets->SetupAttachment(PistolMesh);
	
}

// Called when the game starts or when spawned
void APistol::BeginPlay()
{
	
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	Heroo = Cast<AMyHeroo>(PlayerPawn);
	if (Heroo) {
		//GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Cyan, FString::Printf(TEXT("The Pistol Has The Heroo Referance")));
	}
	
	if (Heroo) {
		CameraDefaultLocation = Heroo->CameraFollow->GetComponentLocation();
		CameraRelativeLocaiton = Heroo->CameraFollow->GetRelativeLocation();

	}

	OnShootingWithPistol.Broadcast(PistolBulletsNum, MaxPistolBullets);
	
}

// Called every frame
void APistol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Aiming 
	if (PressedToAim && TimeElasped < LerpDuration) {

		AimingSocketLocation = PistolMesh->GetSocketLocation(FName("Aiming Socket"));
		Heroo->CameraFollow->SetWorldLocation(FMath::Lerp(CameraDefaultLocation, AimingSocketLocation, TimeElasped / LerpDuration));

		TimeElasped += DeltaTime;
	}
	
}


void APistol::PlayerInteract()
{
	//GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Cyan, FString::Printf(TEXT("Interacted ")));
	//Heroo->IsPickingUpWeapens = true;

	FToAttachWeapen();

}

void APistol::EquipItem()
{
	this->AttachToComponent(Heroo->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Pistol_Socket"));
	IsEquiped = true;
	OnEquip_Pistol.Broadcast(true);

}


void APistol::UnEquipItem()
{

	this->AttachToComponent(Heroo->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("ToHoldRifle"));
	IsEquiped = false;
	OnEquip_Pistol.Broadcast(false);

}

void APistol::AmmoPickUp(float Amount, EAmmoType AmmoType)
{
	if (AmmoType == PistolAmmoType)
	{
		MaxPistolBullets += Amount;
		OnShootingWithPistol.Broadcast(PistolBulletsNum, MaxPistolBullets);
		GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Cyan, FString::Printf(TEXT("The Pistol Has The Heroo Referance")));
	}
}


void APistol::FToAttachWeapen()
{

	if (Heroo != nullptr && Heroo->WeaponType == EWeaponType::Pistol) {

		//GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Cyan, FString::Printf(TEXT("Called The Attach Fuunction")));
		PistolMesh->SetSimulatePhysics(false);
		this->AttachToComponent(Heroo->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Pistol_Socket"));
		BoxToPickUpPistol->SetVisibility(false);

		if (Heroo) {
			Heroo->LeftMouseClick.AddDynamic(this, &APistol::OnUse);
			Heroo->RightMouseClick.AddDynamic(this, &APistol::Aiming_Pressed);
			Heroo->RightMouseReleased.AddDynamic(this, &APistol::Aiming_Released);
			Heroo->On_R_Pressed_Del.AddDynamic(this, &APistol::Reload);

		}

	}
	else {

		PistolMesh->SetSimulatePhysics(false);
		this->AttachToComponent(Heroo->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("ToHoldRifle"));
		BoxToPickUpPistol->SetVisibility(false);

		if (Heroo) {
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, FString::Printf(TEXT("Added Dynamic To The Buttons ")));
			Heroo->LeftMouseClick.AddDynamic(this, &APistol::OnUse);
			Heroo->RightMouseClick.AddDynamic(this, &APistol::Aiming_Pressed);
			Heroo->RightMouseReleased.AddDynamic(this, &APistol::Aiming_Released);
			Heroo->On_R_Pressed_Del.AddDynamic(this, &APistol::Reload);
		}

	}

}


void APistol::DeAttachItem()
{
	FDetachmentTransformRules Rules = FDetachmentTransformRules::KeepWorldTransform;
	this->DetachAllSceneComponents(Heroo->GetMesh(), Rules);
	PistolMesh->SetSimulatePhysics(true);
	PistolMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	BoxToPickUpPistol->SetVisibility(true);

	if (Heroo) {
		Heroo->LeftMouseClick.RemoveDynamic(this, &APistol::OnUse);
		Heroo->RightMouseClick.RemoveDynamic(this, &APistol::Aiming_Pressed);
		Heroo->RightMouseReleased.RemoveDynamic(this, &APistol::Aiming_Released);
		Heroo->On_R_Pressed_Del.RemoveDynamic(this, &APistol::Reload);
	}

}


void APistol::OnUse()
{


	if (IsEquiped && !IsReloading) {

		if (Heroo) {
			//SpawnBulletsSocketLocation = KarMesh->GetSocketLocation("SocketLocationForBullets");
			FVector TraceStart = Heroo->CameraFollow->GetComponentLocation();
			FVector TraceEnd = TraceStart + Heroo->CameraFollow->GetForwardVector() * 300;

			SpawnBulletsSocketLocation = TraceStart + Heroo->CameraFollow->GetForwardVector() * 300;
			SpawnBulletRotation = Heroo->CameraFollow->GetComponentRotation();
			//SpawnBulletRotation.Pitch += 45.0;

		}

		//DrawDebugLine(GetWorld(), SpawnBulletsSocketLocation, SpawnBulletsSocketLocation + Heroo->CameraFollow->GetForwardVector() * 2000, FColor::Blue, false, 8.0, -1, 0.5);
		if (Heroo != nullptr) {

			if (PistolBulletsNum != 0) {
		
				if (ShootSoundCue) {
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShootSoundCue, PistolMesh->GetSocketLocation("SocketLocationForBullets"));
				}
				if (Shooting_Montage) {
					Heroo->PlayAnimMontage(Shooting_Montage);
				}

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Spawning Bullets  ")));

				//To Play Shooting Animation
				if (ShootingAnimAsset) {
					PistolMesh->PlayAnimation(ShootingAnimAsset, false);
				}
				GetWorld()->SpawnActor<AActor>(Bullets, SpawnBulletsSocketLocation, SpawnBulletRotation, SpawnParams);
				//GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Blue, FString::Printf(TEXT("Hit the Bosssss : %s"), *SpawnBulletsSocketLocation.ToString()));

				PistolBulletsNum--;

				OnShootingWithPistol.Broadcast(PistolBulletsNum, MaxPistolBullets);

			}

		}

	}


}

void APistol::OnReleaseItem()
{
	
}

void APistol::Aiming_Pressed()
{
	if (IsEquiped) {

		//GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Blue, FString::Printf(TEXT("Aiming pressed")));
		Heroo->CameraFollow->SetFieldOfView(70.0);
		CameraDefaultLocation = Heroo->CameraFollow->GetComponentLocation();
		PressedToAim = true;

	}

}

void APistol::Aiming_Released()
{
	if (IsEquiped) {

		Heroo->CameraFollow->SetFieldOfView(98.0);
		//GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Blue, FString::Printf(TEXT("Aiming Released")));
		AimingSocketLocation = FVector::ZeroVector;
		//PC->SetViewTargetWithBlend(PC->GetPawn(), 0.5);
		//CameraFollow->SetRelativeLocation(CameraRelativeLocaiton);
		FAttachmentTransformRules Rules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
		TimeElasped = 0;
		PressedToAim = false;
		FVector CameraLoc = Heroo->GetMesh()->GetSocketLocation(FName("CameraPlaceCrouching"));
		Heroo->CameraFollow->SetWorldLocation(CameraLoc);

	}


}



void APistol::Reload()
{

	if (PistolBulletsNum != 12 ) {

		IsReloading = true;

		int WhatNumToAdd;
		if (MaxPistolBullets > 0) {

			if (ReloadingSoundCue) {
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), ReloadingSoundCue, GetActorLocation());
			}

			WhatNumToAdd = NumBulletsCanBeLoaded - PistolBulletsNum;

			if (WhatNumToAdd > MaxPistolBullets) {
				WhatNumToAdd = MaxPistolBullets;
			}

			PistolBulletsNum += WhatNumToAdd;
			MaxPistolBullets -= WhatNumToAdd;

			OnShootingWithPistol.Broadcast(PistolBulletsNum, MaxPistolBullets);

			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, FString::Printf(TEXT("BulletsNumber : %i "), PistolBulletsNum));
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, FString::Printf(TEXT("Max Bullets Num : %i "), MaxPistolBullets));

			FTimerHandle TimerToEndReloading;
			GetWorldTimerManager().SetTimer(TimerToEndReloading, [&]() {IsReloading = false; }, 2.0, false);

		}

	}

}

