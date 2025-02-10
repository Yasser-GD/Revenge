// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AmmoPickUp.h"

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EquipAndUn_Items_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEquipAndUn_Items_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SHOOTINGGAME_API IEquipAndUn_Items_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:

	UFUNCTION()
	virtual void UnEquipItem() = 0;
	UFUNCTION()
	virtual void EquipItem() = 0;
	UFUNCTION()
	virtual void DeAttachItem() = 0;

	UFUNCTION()
	virtual void AmmoPickUp(float Amount , EAmmoType AmmoType) = 0;

};
