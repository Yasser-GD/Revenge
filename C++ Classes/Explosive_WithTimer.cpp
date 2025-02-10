// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive_WithTimer.h"

#include "Damage_Systeem_InterFac.h"
#include "MyHeroo.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraComponent.h"

// Sets default values
AExplosive_WithTimer::AExplosive_WithTimer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AExplosive_WithTimer::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AExplosive_WithTimer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AExplosive_WithTimer::PlayerInteract()
{
}

void AExplosive_WithTimer::EquipItem()
{
}

void AExplosive_WithTimer::UnEquipItem()
{
}

void AExplosive_WithTimer::AmmoPickUp(float Amount, EAmmoType AmmoType)
{
}

void AExplosive_WithTimer::DeAttachItem()
{
}

void AExplosive_WithTimer::OnUse()
{
}

void AExplosive_WithTimer::OnReleaseItem()
{
}

