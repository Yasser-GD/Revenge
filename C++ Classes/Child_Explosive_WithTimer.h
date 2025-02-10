// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interact_Interface.h"
#include "EquipAndUn_Items_Interface.h"
#include "Player_OnUseInterface.h"

#include "CoreMinimal.h"
#include "Explosive_WithTimer.h"
#include "Child_Explosive_WithTimer.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExplosion);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerInteract);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimerChange, bool, IsIncreasingTimer, int32, Seconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExplosiveEquiped, bool, IsEquiped);

class USoundCue;

UCLASS()
class SHOOTINGGAME_API AChild_Explosive_WithTimer : public AExplosive_WithTimer
{

	GENERATED_BODY()


protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Player Interact to pick Up 
	virtual void PlayerInteract() override;

	//To Equip And UnEquip Item 
	virtual void EquipItem() override;
	virtual void UnEquipItem() override;
	
	virtual void AmmoPickUp(float Amount , EAmmoType AmmoType) override;

	////To Drop Item
	virtual void DeAttachItem() override;

	////On Use Item And On Release 
	virtual void OnUse() override;
	virtual void OnReleaseItem() override;

private:

	// References 

	APawn* PlayerPawn;
	class AMyHeroo* HeroRef;

	UPROPERTY(BlueprintAssignable)
		FOnPlayerInteract OnPlayerInteract;

	UPROPERTY(BlueprintAssignable)
		FOnTimerChange OnTimerChange;

	UPROPERTY(BlueprintAssignable)
		FOnExplosiveEquiped onExplosiveEquiped;


private:


	UFUNCTION()
		void ToIncreaseC4_OwnedNumber();

	UPROPERTY()
		bool AlreadyAttachedToObject = false;


//------------ Count Down Sound Properties ---------------

	UFUNCTION()
		void C4TimerSound(FVector SpawnedActorLocation);

	FTimerHandle C4SoundTimer;
	int TimerNum = 0;
	bool FinishedCountDown = false;

	UPROPERTY(EditAnyWhere)
		USoundCue* OneSecondTimerSound;
	UPROPERTY(EditAnyWhere)
		USoundCue* TwoSecondsBeforeExplodeTimerSound;
	UPROPERTY(EditAnyWhere)
		USoundCue* OneSecondBeforeExplodeTimerSound;

//---------------------------------------------------


	UFUNCTION()
		void Explode();

	UPROPERTY(BlueprintAssignable)
		FOnExplosion OnExplostion;

	UPROPERTY(EditAnywhere, Category = "Explode Varibles")
		float ExplosiveDamage = 200;


	UPROPERTY(EditAnywhere, Category = "Explode Varibles")
		float SphereRadius = 500;

public:

	UPROPERTY(BlueprintReadWrite)
		int32 ExplodeTime = 5;

	UPROPERTY(BlueprintReadWrite)
		bool IsEquiped = false;

private:

	bool DoOnce = false;

	class AChild_Explosive_WithTimer* NewC4Ref;


	AExplosive_WithTimer* ExplosiveInHandRef_One;
	AExplosive_WithTimer* ExplosiveInHandRef_Two;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> PreviewActor;

	AActor* SpawnedPreviewActor;

public:

	UFUNCTION()
		void IncreaseTimeOfExplosive();

	UFUNCTION()
		void DeIncreaseTimeOfExplosive();

};
