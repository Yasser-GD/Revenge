// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interact_Interface.h"
#include "EquipAndUn_Items_Interface.h"
#include "Player_OnUseInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Explosive_WithTimer.generated.h"

UCLASS()
class SHOOTINGGAME_API AExplosive_WithTimer : public AActor, public IInteract_Interface, public IEquipAndUn_Items_Interface, public IPlayer_OnUseInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AExplosive_WithTimer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Player Interact to pick Up 
	virtual void PlayerInteract() override;

	//To Equip And UnEquip Item 
	virtual void EquipItem() override;
	virtual void UnEquipItem() override;
	
	virtual void AmmoPickUp(float Amount , EAmmoType AmmoType) override;

	//To Drop Item
	virtual void DeAttachItem() override;

	//On Use Item And On Release 
	virtual void OnUse() override;
	virtual void OnReleaseItem() override;

public:

	UPROPERTY()
		int ExplosivesOwnedNum = 0;
	

};
