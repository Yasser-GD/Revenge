// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmmoPickUp.generated.h"

UENUM()
enum class EAmmoType : uint8
{
	PistolAmmo = 0,
	RifleAmmo = 1,
};

UCLASS()
class SHOOTINGGAME_API AAmmoPickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmoPickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* AmmoBox;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	int32 Ammo_Amount = 10;

	
};


