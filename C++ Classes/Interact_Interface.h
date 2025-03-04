// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interact_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI , BlueprintType)
class UInteract_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SHOOTINGGAME_API IInteract_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category = "Interact")
		void Interact();


	virtual void PlayerInteract() = 0;


};
