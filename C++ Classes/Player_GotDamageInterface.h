// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Player_GotDamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayer_GotDamageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SHOOTINGGAME_API IPlayer_GotDamageInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent)
		 void Player_TakingDamage();

	// you have to call both the blueprint type and the c++ type
	UFUNCTION()
		virtual void Player_TakingDamage_Pure(float DamageTaken, ACharacter* HittingActor) = 0;

};
