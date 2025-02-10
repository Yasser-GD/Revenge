// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damage_Systeem_InterFac.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamage_Systeem_InterFac : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SHOOTINGGAME_API IDamage_Systeem_InterFac
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	
	virtual	void TakeDamage_AllEnemys(float DamageAmount , FName HitBoneName) = 0;

	virtual void Deflection_impect() = 0;

};
