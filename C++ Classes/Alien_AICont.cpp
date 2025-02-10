// Fill out your copyright notice in the Description page of Project Settings.


#include "Alien_AICont.h"

#include "Alien.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "MyHeroo.h"
#include "ObjectsGrab_Interface.h"
#include "Animation/AnimMontage.h"
#include "Player_GotDamageInterface.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Objects_TakingHits_interface.h"


void AAlien_AICont::BeginPlay() {

	Super::BeginPlay();

	if (GetOwner()) {
		//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("Alien AI Controller Owner Name : %s  "), *GetOwner()->GetName()));
	}

}

void AAlien_AICont::ToGetAlienMainRef(ACharacter* AlienRef) {


	AlienMainRef = Cast<AAlien>(AlienRef);

	if (AlienMainRef) {
		AlienMainRef->OnActivateShield_Del.AddDynamic(this, &AAlien_AICont::Activate_Shield);
	}

}

//--------------------------------------------------------------- Ground Attacks Setup ----------------------------------------------------------------------------

void AAlien_AICont::ToGetSomethingToGrab(bool ShouldGrabObject)
{


	//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Yellow, FString::Printf(TEXT("Called Function ToGetSomethingToGrab_______ ")));
	Object_1 = nullptr;
	Object_2 = nullptr;
	Object_3 = nullptr;

	TArray<FHitResult> SweepHitResults;
	FVector Start = AlienMainRef->GetActorLocation();
	FVector End = Start;
	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(SphereRedious);
	FCollisionQueryParams QueryParams;

	bool Hit = GetWorld()->SweepMultiByChannel(SweepHitResults, Start, End, FQuat::Identity, ECC_Destructible, CollisionShape, QueryParams);
	//DrawDebugSphere(GetWorld(), Start, SphereRedious, 3, FColor::Black, false, 4.0, -1, 2.0);
	if (Hit) {

		//--------------------------if Will Only Hold One Object---------------------

		if (AlienObjectHandleState == EAlienObjectHandleState::Handling_OneObject) {

			for (FHitResult SingleHitResult : SweepHitResults) {
				//IObjectGrab_Interface* GrabInterfaceRef = Cast<IObjectGrab_Interface>(SingleHitResult.GetActor());
				if (SingleHitResult.GetActor() != nullptr && SingleHitResult.GetActor()->ActorHasTag("Grab")) {

					Object_1 = SingleHitResult.GetActor();
					HitComponent_1 = SingleHitResult.GetComponent();
					GrabLocation = Object_1->GetActorLocation();
					GrabingFunction(Object_1, HitComponent_1, AlienMainRef->PhysicsHandleCompObject_1, 2.0, ShouldGrabObject);
					break;

				}

			}
			if (!Object_1) {

				EndLongRangeAttack(nullptr);

			}

		}

		//----------------------if Will Hold Two Objects---------------------

		if (AlienObjectHandleState == EAlienObjectHandleState::Handling_TwoObjects) {

			for (FHitResult SingleHitResult : SweepHitResults) {
				if (SingleHitResult.GetActor() != nullptr && SingleHitResult.GetActor()->ActorHasTag("Grab")) {
					if (!Object_1) {

						Object_1 = SingleHitResult.GetActor();
						HitComponent_1 = SingleHitResult.GetComponent();
						GrabingFunction(Object_1, HitComponent_1, AlienMainRef->PhysicsHandleCompObject_1, 2.5, ShouldGrabObject);
						continue;
					}
					else if (!Object_2) {

						Object_2 = SingleHitResult.GetActor();
						HitComponent_2 = SingleHitResult.GetComponent();
						GrabingFunction(Object_2, HitComponent_2, AlienMainRef->PhysicsHandleCompObject_2, 3.1, ShouldGrabObject);
						continue;

					}

				}

			}

			if (!Object_1 && !Object_2) {

				EndLongRangeAttack(nullptr);

			}

		}

		//----------------------if Will Hold Three Objects---------------------

		if (AlienObjectHandleState == EAlienObjectHandleState::Handling_ThreeObjects) {
			for (FHitResult SingleHitResult : SweepHitResults) {

				if (SingleHitResult.GetActor() != nullptr && SingleHitResult.GetActor()->ActorHasTag("Grab")) {

					// To Assign Object One 

					if (!Object_1) {

						Object_1 = SingleHitResult.GetActor();
						HitComponent_1 = SingleHitResult.GetComponent();
						GrabingFunction(Object_1, HitComponent_1, AlienMainRef->PhysicsHandleCompObject_1, 3.5, ShouldGrabObject);
						continue;
					}
					else if (!Object_2) {

						Object_2 = SingleHitResult.GetActor();
						HitComponent_2 = SingleHitResult.GetComponent();
						GrabingFunction(Object_2, HitComponent_2, AlienMainRef->PhysicsHandleCompObject_2, 5.0, ShouldGrabObject);
						continue;

					}
					else if (!Object_3) {
						Object_3 = SingleHitResult.GetActor();
						HitComponent_3 = SingleHitResult.GetComponent();
						GrabLocation = Object_3->GetActorLocation();
						GrabingFunction(Object_3, HitComponent_3, AlienMainRef->PhysicsHandleCompObject_3, 7.5, ShouldGrabObject);
						continue;

					}

				}

			}
			if (!Object_1 && !Object_2 && !Object_3) {

				EndLongRangeAttack(nullptr);

			}

		}

	}

	// After Finding Something To Grab He Will Call The Function To Grab It 

}


void AAlien_AICont::GrabingFunction(AActor* Object, UPrimitiveComponent* HitComponent, UPhysicsHandleComponent* PhysicsHandleCompObject, float TimeToRelease, bool ShouldGrabObject)
{

	// To Tell Him To Grab Component 
	if (Object) {

		if (HitComponent) {

			//To See if he should pick up the object or not 
			if (ShouldGrabObject) {

				if (Object == Object_1) {
					IsHoldingObject_1 = true;
				}
				else if (Object == Object_2) {
					IsHoldingObject_2 = true;
				}
				else if (Object == Object_3) {
					IsHoldingObject_3 = true;
				}
				PhysicsHandleCompObject->GrabComponentAtLocation(HitComponent, NAME_None, HitComponent->GetComponentLocation());
			}


			//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FString::Printf(TEXT("He Is Grabing Something  ")));

			//Changing State To Holding Object To Throw
			AlienMainRef->AlienAttackState = EAlienAttackState::Holding_Oject;

			//Checks And Adds On Actor Hit Delegete
			if (!Object->OnActorHit.IsBound()) {
				Object->OnActorHit.AddDynamic(this, &AAlien_AICont::OnGrabbedObjectHit);
			}

			FVector SmallImpulse = AlienMainRef->GetActorUpVector() * 10;
			HitComponent->AddImpulse(SmallImpulse);

			if (!ShouldGrabObject) {
				//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("Grab Function  ")));
				//Object->SetActorLocation(FVector(Object->GetActorLocation().X, Object->GetActorLocation().Y, Object->GetActorLocation().Z + 500));
				//if (RotationComponent) {

				FVector SmallImpulseInside = AlienMainRef->GetActorUpVector() * 10;
				HitComponent->AddImpulse(SmallImpulseInside);

				if (RotationComponent_AC) {

					FTransform RelativeTransformtion;
					Object->AddComponentByClass(RotationComponent_AC, true, RelativeTransformtion, false);

					URotatingMovementComponent* RotationComponentRef = Cast<URotatingMovementComponent>(Object->GetComponentByClass(RotationComponent_AC));
					//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("Casting The Rotation Component  ")));

					if (RotationComponentRef) {
						RotationComponentRef->RotationRate = FRotator(0.0, 1000, 0.0);
						//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("He is making the Object rotating ")));

					}


				}

				//}

			}

			//-----To Call The Throw Function After Some time 
			FTimerDelegate TimerDel;
			FTimerHandle TimerToPlayThrowMontage;
			TimerDel.BindUFunction(this, "PlayThorwMontage", Object, HitComponent, PhysicsHandleCompObject, ShouldGrabObject);
			GetWorldTimerManager().SetTimer(TimerToPlayThrowMontage, TimerDel, TimeToRelease, false);

		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("He Couldnt Find The Hit Component !!!!!!!!!!!! ")));
		}

	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("He Couldn't Find A GrabLoction or Main Object ?????????? ")));
	}

}


void AAlien_AICont::ReleaseComponentAndAddImpluse(AActor* Object, UPrimitiveComponent* HitComponent, UPhysicsHandleComponent* PhysicsHandleCompObject, bool ShouldGrabObject)
{

	//-----------To Release Component From the Physics Handle Component-------------
	if (AlienMainRef && AlienMainRef->PlayerPawn && !AlienMainRef->StopAttacking) {

		// if He Picked Up The Object He Will Release it And Add Impulse 
		if (ShouldGrabObject && PhysicsHandleCompObject && PhysicsHandleCompObject->GetGrabbedComponent()) {

			PhysicsHandleCompObject->ReleaseComponent();
			if (Object == Object_1) {
				IsHoldingObject_1 = false;
			}
			else if (Object == Object_2) {
				IsHoldingObject_2 = false;
			}
			else if (Object == Object_3) {
				IsHoldingObject_3 = false;
			}
		}

		// if He Didnt Pick Up The Object He Throw
		else if (!ShouldGrabObject) {

			//To Remove the Rotation Component After Throwing
			if (Object->GetComponentByClass(RotationComponent_AC)) {
				Object->GetComponentByClass(RotationComponent_AC)->DestroyComponent();
			}

			//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("No Component Was Grabbed !!!!!!!!!!!!! ")));
		}

		AlreadyHit = false;

		//-----------Adding Impulse To Hit The Player-----------
		if (HitComponent) {

			IsThrowing = true;

			//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT(" He Throwed Something ")));

			FVector PlayerDirection = HitComponent->GetComponentLocation() - AlienMainRef->PlayerPawn->GetActorLocation();
			PlayerDirection.Z += -60;
			int Random = FMath::RandRange(-60, 60);
			PlayerDirection.Y += Random;

			FVector Impluse = -PlayerDirection * (HitComponent->GetMass() + 60);
			HitComponent->AddImpulse(Impluse);
			//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("He Released Component And Added Impulse : %f "), HitComponent->GetMass()/4 ));
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT(" NO he Couldnt Find The Hit Component !!!!!!!!!!! ")));

		}


		/*	FTimerHandle RemoveDynamicTimer;
			GetWorldTimerManager().SetTimer(RemoveDynamicTimer, [&]() { }*/

			//-------------------To Remove Dynamicly the Damage Of The Grabbed Object------------------
		if (Object) {

			FTimerDelegate TimerDel;
			FTimerHandle TimerToEndLongRangeAttack;

			TimerDel.BindUFunction(this, FName("EndLongRangeAttack"), Object);
			GetWorldTimerManager().SetTimer(TimerToEndLongRangeAttack, TimerDel, 1.0, false);

		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("NO he Couldnt Find Main Object !!!!!!!!!!! ")));
		}


	}
	else if (AlienMainRef->StopAttacking || AlienMainRef->AlienDead) {

		if (ShouldGrabObject && PhysicsHandleCompObject && PhysicsHandleCompObject->GetGrabbedComponent()) {

			PhysicsHandleCompObject->ReleaseComponent();
			if (Object == Object_1) {
				IsHoldingObject_1 = false;
			}
			else if (Object == Object_2) {
				IsHoldingObject_2 = false;

				FTimerDelegate TimerDel;
				FTimerHandle TimerToEndLongRangeAttack;
				TimerDel.BindUFunction(this, FName("EndLongRangeAttack"), Object);
				GetWorldTimerManager().SetTimer(TimerToEndLongRangeAttack, TimerDel, 1.0, false);

			}
			else if (Object == Object_3) {
				IsHoldingObject_3 = false;

				FTimerDelegate TimerDel;
				FTimerHandle TimerToEndLongRangeAttack;
				TimerDel.BindUFunction(this, FName("EndLongRangeAttack"), Object);
				GetWorldTimerManager().SetTimer(TimerToEndLongRangeAttack, TimerDel, 1.0, false);
			}
		}

		// if He Didnt Pick Up The Object He Throw
		else if (!ShouldGrabObject) {

			//To Remove the Rotation Component After Throwing
			if (Object->GetComponentByClass(RotationComponent_AC)) {
				Object->GetComponentByClass(RotationComponent_AC)->DestroyComponent();
			}

			//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("No Component Was Grabbed !!!!!!!!!!!!! ")));
		}

		//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("Couldn't Find The Alien Refernce ???????????? ")));
	}

	GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Orange, FString::Printf(TEXT("He Ended The Release Function ")));


}


void AAlien_AICont::PlayThorwMontage(AActor* Object, UPrimitiveComponent* HitComponent, UPhysicsHandleComponent* PhysicsHandleCompObject, bool ShouldGrabObject)
{

	if (!AlienMainRef->StopAttacking) {
		AlienMainRef->ToCallPlayMontage();

	}

	FTimerDelegate TimerDel;
	FTimerHandle ReleaseTimer;
	TimerDel.BindUFunction(this, FName("ReleaseComponentAndAddImpluse"), Object, HitComponent, PhysicsHandleCompObject, ShouldGrabObject);
	GetWorldTimerManager().SetTimer(ReleaseTimer, TimerDel, 0.7, false);



}

void AAlien_AICont::EndLongRangeAttack(AActor* ObjectRef)
{
	if (ObjectRef) {
		ObjectRef->OnActorHit.RemoveDynamic(this, &AAlien_AICont::OnGrabbedObjectHit);
		//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Green, FString::Printf(TEXT(" He Removed Dynamicly Form Object Referance After Releasing it  ")));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT(" NO Couldn't Find the Object Referance After Releasing it  ")));
	}

	AlienMainRef->ToChangeStateAndSpeed(EAlienAttackState::NormalWalkingAround, AlienMainRef->WalkingSpeed);
	AlienMainRef->IsAttacking = false;

}


void AAlien_AICont::OnGrabbedObjectHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{

	if (OtherActor && ImpactSound1 && ImpactSound2 && IsThrowing) {

		if (!PlayedFirstSound) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound1, Hit.Location);
			PlayedFirstSound = true;
		}
		else {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound1, Hit.Location);
			PlayedFirstSound = false;
		}

		IsThrowing = false;
	}

	if (OtherActor && OtherActor->GetClass()->ImplementsInterface(UPlayer_GotDamageInterface::StaticClass()) && !AlreadyHit) {

		Cast<IPlayer_GotDamageInterface>(OtherActor)->Player_TakingDamage_Pure(20, AlienMainRef);
		IPlayer_GotDamageInterface::Execute_Player_TakingDamage(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

		AlreadyHit = true;
		//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT(" The Player Got Hit In C++ Class ")));

	}

	if (OtherActor && OtherActor->GetClass()->ImplementsInterface(UObjects_TakingHits_interface::StaticClass()) && !AlreadyHit) {
		IObjects_TakingHits_interface::Execute_Object_TakingDamage(OtherActor);
		AlreadyHit = true;
		//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT(" The Other Actor has The Interface of objects Being Hit ")));

	}

}


//----------------------------------------------------------------------- Ground End ----------------------------------------------------------------------------------

void AAlien_AICont::Activate_Shield(bool IsActivatingShield)
{

	if (IsActivatingShield) {
		AlienMainRef->CanBeDamaged = false;
		IsShieldActivated = true;
		ShieldHealth = initialHealth;
	}
	else {
		AlienMainRef->CanBeDamaged = true;
		IsShieldActivated = false;
		ShieldHealth = initialHealth;
	}

}

void AAlien_AICont::DeActivate_Shield()
{

	AlienMainRef->CanBeDamaged = true;
	IsShieldActivated = false;

	ShieldHealth = initialHealth;

}
