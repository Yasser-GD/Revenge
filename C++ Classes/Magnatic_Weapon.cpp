// Fill out your copyright notice in the Description page of Project Settings.


#include "Magnatic_Weapon.h"

#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraComponent.h"
#include "Damage_Systeem_InterFac.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "MyHeroo.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Components/AudioComponent.h"

// Sets default values
AMagnatic_Weapon::AMagnatic_Weapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMagnatic_Weapon::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	Heroo = Cast<AMyHeroo>(PlayerPawn);

	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);

}

// Called every frame
void AMagnatic_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMagnatic_Weapon::PlayerInteract()
{
	this->AttachToComponent(Heroo->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Pistol_Socket"));
	//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("Magnatic Weapon Attached ")));
	PlayerInteracted.Broadcast();

	Heroo->RightMouseClick.AddDynamic(this, &AMagnatic_Weapon::OnGrab);
	Heroo->RightMouseReleased.AddDynamic(this, &AMagnatic_Weapon::OnGrabRelease);

	Heroo->LeftMouseClick.AddDynamic(this, &AMagnatic_Weapon::OnHeldToThrow);
	Heroo->LeftMouseReleased.AddDynamic(this, &AMagnatic_Weapon::OnThrow);

}

void AMagnatic_Weapon::EquipItem()
{
	AttachToComponent(Heroo->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Magnatic Weapon Equip"));
	IsEquiped = true;
}

void AMagnatic_Weapon::UnEquipItem()
{
	AttachToComponent(Heroo->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("ToHoldRifle"));
	IsEquiped = false;
}

void AMagnatic_Weapon::AmmoPickUp(float Amount, EAmmoType AmmoType)
{
}

void AMagnatic_Weapon::DeAttachItem()
{

	FDetachmentTransformRules Rules = FDetachmentTransformRules::KeepWorldTransform;
	this->DetachAllSceneComponents(Heroo->GetMesh(), Rules);

}


void AMagnatic_Weapon::OnUse()
{
}

void AMagnatic_Weapon::OnReleaseItem()
{
	
}



void AMagnatic_Weapon::AttachWeapon(AMyHeroo* TargetCharacter)
{

	HeroRef = TargetCharacter;
	if (HeroRef) {

		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		GetOwner()->AttachToComponent(HeroRef->GetMesh(), AttachmentRules, FName(TEXT("GripPoint")));

		/*Heroo->RightMouseClick.AddDynamic(this, &AMagnatic_Weapon::Aiming_Pressed);
		Heroo->RightMouseReleased.AddDynamic(this, &AMagnatic_Weapon::Aiming_Released);*/

	}

}

void AMagnatic_Weapon::DeAttachWeapon()
{

	if (HeroRef) {

		/*FDetachmentTransformRules *DetachRules;
		GetOwner()->DetachFromActor(*DetachRules);*/

		/*HeroRef->OnUseItem.RemoveDynamic(this, &UWeaponComponent_Grab::OnGrab);
		HeroRef->OnReleaseItem.RemoveDynamic(this, &UWeaponComponent_Grab::OnRelease);*/
	}

}



///----------------------------------------------------------------------- WEAPON FUNCTIONLITY ----------------------------------------------------------------


void AMagnatic_Weapon::OnGrab()
{

	if (IsEquiped) {

		if (!Delaying && !IsGrabbingObject) {

			if (Heroo == nullptr || Heroo->GetController() == nullptr)
			{
				return;
			}

			//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("Called On Use Item  ")));

			AlreadyHit = false;
			PlayerController = Cast<APlayerController>(Heroo->GetController());

			if (PlayerController) {

				const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();

				FHitResult LineHitResult;
				FVector Start = PlayerController->PlayerCameraManager->GetCameraLocation();
				FVector End = Start + (PlayerController->PlayerCameraManager->GetActorForwardVector() * 430);
				FCollisionQueryParams Params;

				bool Hit = GetWorld()->LineTraceSingleByChannel(LineHitResult, Start, End, ECC_Vehicle, Params);
				//DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, 5.0, -1, 3.0);
				if (Hit) {
					//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("Line Trace Hit ")));

					if (LineHitResult.GetActor()->ActorHasTag(FName("Grab"))) {

						HitComponent = LineHitResult.GetComponent();
						GrabbedActor = LineHitResult.GetActor();
						GrabLocation = GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
						if (HitComponent) {
							//PhysicsHandleCompObject_->GrabComponentAtLocation(HitComponent, NAME_None, HitComponent->GetComponentLocation());
						}

						FVector SmallImpulse = Heroo->GetActorUpVector() * 10;
						HitComponent->AddImpulse(SmallImpulse);

						//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("Grabbing Object ")));

						// To Say The He is Already Grabing Something
						IsGrabbingObject = true;

						OnGrabObject.Broadcast(HitComponent, HitComponent->GetComponentLocation());

						

						RotationRate = 45.0;

						if (RotationComponent) {
							FTransform RelativeTransformtion;
							GrabbedActor->AddComponentByClass(RotationComponent, true, RelativeTransformtion, false);
							//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("The Rotaiton Compoent Is Added  ")));
						}

					}

				}

			}
			else {
				GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("Player Controller is Null ")));
			}

		}

	}

}


void AMagnatic_Weapon::OnGrabRelease()
{
	if (IsGrabbingObject && IsEquiped && GrabbedActor) {

		//On Release Object Broadcast To Who Ever is Listening 
		OnReleaseObject.Broadcast();

		// TO Add Dynamic On Release To Remove The Hit From THe Object We Were Grabbing 
		if (!DoOnce_AddDynamic) {
			OnObjectDamage.AddDynamic(this, &AMagnatic_Weapon::OnReleaseGrabbed);
			DoOnce_AddDynamic = true;
		}

	}

}


void AMagnatic_Weapon::OnHeldToThrow()
{
	if (IsGrabbingObject && IsEquiped && GrabbedActor) {

		PressedToThrow = true;
		GrabbedActor->SetActorRotation(FQuat::Identity);

		if (!GrabbedActor->OnActorHit.IsBound()) {
			IsThrowing = false;
			GrabbedActor->OnActorHit.AddDynamic(this, &AMagnatic_Weapon::OnGrabbedHit);
		}

		GetWorldTimerManager().SetTimer(GetHarderTimer, this, &AMagnatic_Weapon::ObjectGetHarder, 0.6, true);

	}

}


void AMagnatic_Weapon::OnThrow()
{

	if (IsGrabbingObject && IsEquiped && GrabbedActor) {

		//To Remove the Rotation Component After Throwing
		if (GrabbedActor->GetComponentByClass(RotationComponent)) {
			GrabbedActor->GetComponentByClass(RotationComponent)->DestroyComponent();
		}

		GetWorldTimerManager().ClearTimer(GetHarderTimer);

		if (AlreadyPlayedSound) {

			GettingHarder_Del.Broadcast(true);
			AlreadyPlayedSound = false;
		}


		//To Give it A dalay of 1 one sec
		Delaying = true;

		PressedToThrow = false;
		IsThrowing = true;

		if (RelesingSound) {
			UGameplayStatics::PlaySoundAtLocation(this, RelesingSound, GetActorLocation());
		}

		// To Remove The Damage from the throwed thing 
		FTimerHandle TimeToRemoveDynamic;
		GetWorld()->GetTimerManager().SetTimer(TimeToRemoveDynamic, [&]() {OnObjectDamage.Broadcast();
		//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("Removed Dynamic ")));
		Delaying = false;
		ThrowDamage = DefaultThrowDamage;
		Impulsestrength = DefaultImpulse;  }, 1.0, false);

		OnThrowDelegate.Broadcast();

		FVector Start = GrabbedActor->GetActorLocation();

		FVector End = Start + FVector(300, 300, 0.0);

		//FVector LocationAwayFromCamera = CameraLoc + FVector(100, 100, 0.0);

		FVector TraceStart = Heroo->CameraFollow->GetComponentLocation();
		FVector TraceEnd = TraceStart + Heroo->CameraFollow->GetForwardVector() * 300;

		FVector ObjectNewLocation = TraceStart + Heroo->CameraFollow->GetForwardVector() * 200;
		GrabbedActor->SetActorLocation(TraceEnd);
		//DrawDebugLine(GetWorld(), GetActorLocation(), TraceEnd, FColor::Cyan, false, 6.0, -1, 2.0);

		FVector Diraction = TraceEnd - ObjectNewLocation;

		FVector Impulse = Diraction * Impulsestrength;
		HitComponent->SetPhysicsLinearVelocity(Impulse);

		IsGrabbingObject = false;

		// To Set The Damage And The Impulse Back to The Default 

	}

}


void AMagnatic_Weapon::ObjectGetHarder()
{

	if (ThrowDamage < 120) {
		//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT(" Object Got Harder : %f"), ThrowDamage));
		ThrowDamage += 24;

		//To Play Get Harder Sound

		if (!AlreadyPlayedSound) {
			//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT(" Deleget BroadCast For Playing Charging Sound ")));
			GettingHarder_Del.Broadcast(false);
			AlreadyPlayedSound = true;
		}

		URotatingMovementComponent* RotationComponentRef = Cast<URotatingMovementComponent>(GrabbedActor->GetComponentByClass(RotationComponent));

		if (RotationComponentRef) {
			RotationRate *= 3;
			RotationComponentRef->RotationRate = FRotator(0.0, RotationRate, 0.0);

		}

	}

	if (Impulsestrength < 65) {
		Impulsestrength += 10;
	}

}


void AMagnatic_Weapon::OnGrabbedHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{

	if (OtherActor) {

		IDamage_Systeem_InterFac* DamageInterface = Cast<IDamage_Systeem_InterFac>(OtherActor);
		if (DamageInterface && !AlreadyHit) {

			//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Red, FString::Printf(TEXT("Damaged the alien ")));
			DamageInterface->TakeDamage_AllEnemys(ThrowDamage, NAME_None);
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound_OfHittingPawn, Hit.Location);
			AlreadyHit = true;

		}

		if (OtherActor && !AlreadyHit && ImpactSound1 && ImpactSound2 && IsThrowing) {
			//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("He played The Rock Impact Sound ")));
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

	}

}

void AMagnatic_Weapon::OnReleaseGrabbed()
{
	if (GrabbedActor) {
		//GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Purple, FString::Printf(TEXT("On Release Grabbed Is Called !!!!!!!!!! ")));
		GrabbedActor->OnActorHit.RemoveDynamic(this, &AMagnatic_Weapon::OnGrabbedHit);
	}

}



//////////////////He Will Grab the Object With Right Click And Will Release with the left mouse button And if he held the the left button while releasing the throw will be stronger 