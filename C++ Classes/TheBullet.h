// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TheBullet.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class SHOOTINGGAME_API ATheBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATheBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectille")
		UProjectileMovementComponent* BulletProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectille")
		USphereComponent* ColitionSphere;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectille")
	class UStaticMeshComponent* BulletMesh;

	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
		float BulletDamage = 50.0;
	UPROPERTY()
	 class AMyHeroo* Herooo;

private:

	class APawn* PlayerPawn;
	class AMyHeroo* HerooRef;


};
