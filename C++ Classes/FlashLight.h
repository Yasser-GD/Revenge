// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interact_Interface.h"
#include "EquipAndUn_Items_Interface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlashLight.generated.h"

UCLASS()
class SHOOTINGGAME_API AFlashLight : public AActor, public IInteract_Interface, public IEquipAndUn_Items_Interface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlashLight();

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
	
	virtual void DeAttachItem() override;

	

	UPROPERTY(EditAnywhere)
		class USpotLightComponent* FlashLight;

private:


	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* FlashLight_Mesh;

	UPROPERTY(EditAnywhere)
		class UBoxComponent* BoxToPickUpFlashLight;
	UPROPERTY()
	APawn* PlayerPawn;

	//Hero Referance
	UPROPERTY()
	class AMyHeroo* Heroo;


	//Function To Attach The FlashLight To The player when Interact 
	UFUNCTION()
		void AttachToPlayer();
	bool AlreadyInteracted = false;

};
