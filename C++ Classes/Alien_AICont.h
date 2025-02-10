// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Alien_AICont.generated.h"


class UPhysicsHandleComponent;



UENUM(BlueprintType)
enum class EAlienObjectHandleState : uint8 {

	Handling_OneObject = 0,
	Handling_TwoObjects = 1,
	Handling_ThreeObjects = 2,

};


UCLASS()
class SHOOTINGGAME_API AAlien_AICont : public AAIController
{

	GENERATED_BODY()


protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UFUNCTION()
		void ToGetAlienMainRef(ACharacter* AlienRef);

	class AAlien* AlienMainRef;

public:

	//---------------------------Grab System For Long Range Attack --------------------------

	UPROPERTY()
		EAlienObjectHandleState AlienObjectHandleState;

	UPrimitiveComponent* HitComponent_1;
	UPrimitiveComponent* HitComponent_2;
	UPrimitiveComponent* HitComponent_3;

	UFUNCTION()
		void ToGetSomethingToGrab(bool ShouldGrabObject);
	UPROPERTY(EditAnyWhere)
		float SphereRedious = 2000;

	// Grabing Function And Release And Add Impulse To Hit Player
	UFUNCTION()
		void GrabingFunction(AActor* Object, UPrimitiveComponent* HitComponent, UPhysicsHandleComponent* PhysicsHandleCompObject, float TimeToRelease, bool ShouldGrabObject);
	UFUNCTION(BlueprintCallable)
		void ReleaseComponentAndAddImpluse(AActor* Object, UPrimitiveComponent* HitComponent, UPhysicsHandleComponent* PhysicsHandleCompObject, bool ShouldGrabObject);

public:

	UPROPERTY(EditAnywhere)
		TSubclassOf<UActorComponent> RotationComponent_AC;

	UPROPERTY(EditAnywhere)
		class UActorComponent* RotationComponentReff;

public:

	UFUNCTION()
		void PlayThorwMontage(AActor* Object, UPrimitiveComponent* HitComponent, UPhysicsHandleComponent* PhysicsHandleCompObject, bool ShouldGrabObject);

	UFUNCTION()
		void EndLongRangeAttack(AActor* ObjectRef);


	UFUNCTION()
		void OnGrabbedObjectHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	bool AlreadyHit = false;

	FVector GrabLocation;

	//---------Objects Referance To Grab-------
	AActor* Object_1;
	AActor* Object_2;
	AActor* Object_3;

	UPROPERTY(EditAnyWhere)
		bool ThrowedSecondOne = false;

	class IObjectGrab_Interface* ObjectGrabInterfaceRef;

public:
	UPROPERTY()
		bool IsHoldingObject_1 = false;
	UPROPERTY()
		bool IsHoldingObject_2 = false;
	UPROPERTY()
		bool IsHoldingObject_3 = false;




	//---------------------------------------------------


	// -----------------------------------------Shield Skill-----------------------------------------


	UFUNCTION()
		void Activate_Shield(bool IsActivatingShield);

	UFUNCTION()
		void DeActivate_Shield();

	UPROPERTY(BlueprintReadWrite)
		bool IsShieldActivated = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ShieldHealth = 1500.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float initialHealth = 1500.0;


	// Im Thing About When The alien throw things while the sield is activated the object will take some of the shield with it 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UParticleSystemComponent* effectPSComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UParticleSystem* effectPS;


private:

	//-----------------------Sounds--------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite , meta = (AllowPrivateAccess = "true"))
		USoundCue* ImpactSound1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USoundCue* ImpactSound2;

	bool PlayedFirstSound = false;
	bool IsThrowing = false;

};
