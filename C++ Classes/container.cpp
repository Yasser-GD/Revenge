// Fill out your copyright notice in the Description page of Project Settings.


#include "container.h"

// Sets default values
Acontainer::Acontainer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void Acontainer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void Acontainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void Acontainer::Object_TakingDamage_Implementation()
{
}

void Acontainer::Object_TakingDamage_Pure(float DamageTaken, ACharacter* HittingActor)
{

	Container_Health -= DamageTaken;

}

