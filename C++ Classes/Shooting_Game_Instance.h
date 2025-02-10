// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Shooting_Game_Instance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGGAME_API UShooting_Game_Instance : public UGameInstance
{
	GENERATED_BODY()
	

public:


	UPROPERTY()
		int CorrentFloor = 2;

	UPROPERTY(BlueprintReadwrite)
		bool bShowCursor = false;

public:

	UPROPERTY(BlueprintReadwrite)
		bool HasKey_SecurityRoom = false;

	UPROPERTY(BlueprintReadwrite)
		bool HasKey_LittleGirlRoom = false;

	// ------- Choice Making -------
	UPROPERTY(BlueprintReadwrite)
		bool Choice_ToTakeHer = false;
	UPROPERTY(BlueprintReadwrite)
		bool Choice_ToGiveHerInjection = false;

	UPROPERTY(BlueprintReadwrite)
		bool PlayerHasSyringe = false;

public:

	//-------------------------- Boss Fight Timer ----------------------
	
	UPROPERTY(BlueprintReadWrite)
		float Seconds = 0.0;

	UPROPERTY(BlueprintReadWrite)
		float Minutes = 0.0;

	UPROPERTY(BlueprintReadWrite)
		float Hours = 0.0;

	UPROPERTY(BlueprintReadWrite)
		bool DidHeSaveBefore = false;

};
