// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Objects_TakingHits_interface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "container.generated.h"

UCLASS()
class SHOOTINGGAME_API Acontainer : public AActor , public IObjects_TakingHits_interface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Acontainer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	virtual void Object_TakingDamage_Implementation() override;

	virtual void Object_TakingDamage_Pure(float DamageTaken, ACharacter* HittingActor) override;

	UPROPERTY(EditAnyWhere)
		float Container_Health = 100;

};
