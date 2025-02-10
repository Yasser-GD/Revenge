// Fill out your copyright notice in the Description page of Project Settings.


#include "FlashLight.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyHeroo.h"
#include "Components/SpotLightComponent.h"

// Sets default values
AFlashLight::AFlashLight()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FlashLight_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlashLight_Mesh"));
	//RootComponent = FlashLight_Mesh;

	BoxToPickUpFlashLight = CreateDefaultSubobject<UBoxComponent>(TEXT("Box To Pick Up Better"));
	BoxToPickUpFlashLight->SetupAttachment(FlashLight_Mesh);
	FlashLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spot Light"));
	FlashLight->SetupAttachment(FlashLight_Mesh);

}

// Called when the game starts or when spawned
void AFlashLight::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	Heroo = Cast<AMyHeroo>(PlayerPawn);

}

// Called every frame
void AFlashLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFlashLight::PlayerInteract()
{
	
	GEngine->AddOnScreenDebugMessage(-1, 6.0, FColor::Cyan, FString::Printf(TEXT("he called the interact function in the flashlight ")));
		AttachToPlayer();
		AlreadyInteracted = true;

}

void AFlashLight::EquipItem()
{
	if (Heroo->WeaponType == EWeaponType::FlashLight) {
	this->AttachToComponent(Heroo->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("FlashLightShootingSocket"));

	}
	else if (Heroo->WeaponType == EWeaponType::Pistol) {

		this->AttachToComponent(Heroo->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("FlashLight_LeftHandSoc"));
	}

}

void AFlashLight::UnEquipItem()
{

	this->AttachToComponent(Heroo->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("FlashLight Holder"));

}

void AFlashLight::AmmoPickUp(float Amount, EAmmoType AmmoType)
{
}

void AFlashLight::DeAttachItem()
{

	FDetachmentTransformRules Rules = FDetachmentTransformRules::KeepWorldTransform;
	this->DetachAllSceneComponents(Heroo->GetMesh(), Rules);
	FlashLight_Mesh->SetSimulatePhysics(true);
	FlashLight_Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	BoxToPickUpFlashLight->SetVisibility(true);

}

void AFlashLight::AttachToPlayer()
{
	GEngine->AddOnScreenDebugMessage(-1, 6.0, FColor::Cyan, FString::Printf(TEXT("he is Soppused To Attach The Flashlight ")));
	
		GEngine->AddOnScreenDebugMessage(-1, 6.0, FColor::Cyan, FString::Printf(TEXT("he is Soppused To Attach The Flashlight ")));
		this->AttachToComponent(Heroo->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("FlashLight Holder"));
		BoxToPickUpFlashLight->SetVisibility(false);

	


}

