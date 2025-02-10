// Fill out your copyright notice in the Description page of Project Settings.


#include "Child_Explosive_WithTimer.h"

#include "Damage_Systeem_InterFac.h"
#include "MyHeroo.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player_GotDamageInterface.h"


void AChild_Explosive_WithTimer::BeginPlay()
{

	Super::BeginPlay();

	PrimaryActorTick.bCanEverTick = true;

	PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	if (PlayerPawn) {
		HeroRef = Cast<AMyHeroo>(PlayerPawn);
	}

}


void AChild_Explosive_WithTimer::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

	if (IsEquiped) {

		FHitResult LineHit;
		FVector TraceStart = HeroRef->CameraFollow->GetComponentLocation();
		FVector TraceEnd = TraceStart + HeroRef->CameraFollow->GetForwardVector() * 250;

		bool Hit = GetWorld()->LineTraceSingleByChannel(LineHit, TraceStart, TraceEnd, ECC_Vehicle);

		if (Hit) {

			if (PreviewActor) {
				SpawnedPreviewActor->SetActorHiddenInGame(false);
				FAttachmentTransformRules PreviewActorAttachRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
				SpawnedPreviewActor->AttachToActor(LineHit.GetActor(), PreviewActorAttachRules, NAME_None);
				SpawnedPreviewActor->SetActorLocation(LineHit.ImpactPoint);
				//SpawnedPreviewActor->SetActorRotation(UKismetMathLibrary::MakeRotFromXY(LineHit.ImpactNormal, LineHit.ImpactNormal));

			}

		}
		else {
			SpawnedPreviewActor->SetActorHiddenInGame(true);
		}

	}

}


void AChild_Explosive_WithTimer::PlayerInteract()
{

	ToIncreaseC4_OwnedNumber();

	OnPlayerInteract.Broadcast();

	if (ExplosiveInHandRef_One && ExplosiveInHandRef_One->ExplosivesOwnedNum == 1) {

		FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
		AttachToComponent(HeroRef->GetMesh(), AttachRules, FName("SecondaryPocket"));
		//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, FString::Printf(TEXT("He Picked Up An Explosive One")));
		if (!DoOnce) {
			HeroRef->LeftMouseClick.AddDynamic(this, &AExplosive_WithTimer::OnUse);
			HeroRef->MouseWheelUpCast_Del.AddDynamic(this, &AChild_Explosive_WithTimer::IncreaseTimeOfExplosive);
			HeroRef->MouseWheelDownCast_Del.AddDynamic(this, &AChild_Explosive_WithTimer::DeIncreaseTimeOfExplosive);
		}

	}

	if (ExplosiveInHandRef_Two && ExplosiveInHandRef_Two->ExplosivesOwnedNum == 1) {

		FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
		AttachToComponent(HeroRef->GetMesh(), AttachRules, FName("SecondaryPocket"));
		//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, FString::Printf(TEXT("He Picked Up An Explosive Two ")));
		if (!DoOnce) {
			HeroRef->LeftMouseClick.AddDynamic(this, &AExplosive_WithTimer::OnUse);
			HeroRef->LeftMouseClick.AddDynamic(this, &AExplosive_WithTimer::OnUse);
			HeroRef->MouseWheelUpCast_Del.AddDynamic(this, &AChild_Explosive_WithTimer::IncreaseTimeOfExplosive);
			HeroRef->MouseWheelDownCast_Del.AddDynamic(this, &AChild_Explosive_WithTimer::DeIncreaseTimeOfExplosive);
		}

	}

}

void AChild_Explosive_WithTimer::EquipItem()
{

	if (PreviewActor) {

		FHitResult LineHit;
		FVector TraceStart = HeroRef->CameraFollow->GetComponentLocation();
		FVector TraceEnd = TraceStart + HeroRef->CameraFollow->GetForwardVector() * 150;

		//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, FString::Printf(TEXT("Spawned A New One ")));
		FVector SpawnLocation = HeroRef->GetMesh()->GetSocketLocation(FName("Secondary Item"));
		FRotator SpawnRotation = HeroRef->GetMesh()->GetSocketRotation(FName("Secondary Item"));
		FActorSpawnParameters SpawnParams;

		//To Spawn New Explosive In Hand
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnedPreviewActor = GetWorld()->SpawnActor<AActor>(PreviewActor, TraceEnd, SpawnRotation, SpawnParams);

	}


	if (!AlreadyAttachedToObject) {

		IsEquiped = true;
		onExplosiveEquiped.Broadcast(true);
		//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, FString::Printf(TEXT("Equiped The Explosives ")));
		FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
		AttachToComponent(HeroRef->GetMesh(), AttachRules, FName("Secondary Item"));

	}


}


void AChild_Explosive_WithTimer::UnEquipItem()
{
	if (!AlreadyAttachedToObject) {

		IsEquiped = false;
		onExplosiveEquiped.Broadcast(false);
		FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
		AttachToComponent(HeroRef->GetMesh(), AttachRules, FName("SecondaryPocket"));

		SpawnedPreviewActor->Destroy();

	}

}

void AChild_Explosive_WithTimer::AmmoPickUp(float Amount, EAmmoType AmmoType)
{
	Super::AmmoPickUp(Amount, AmmoType);
}

void AChild_Explosive_WithTimer::DeAttachItem()
{
}


void AChild_Explosive_WithTimer::OnUse()
{

	if (IsEquiped) {

		//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, FString::Printf(TEXT("On Use Called In the Explosive ")));

		if (ExplodeTime > 0) {

			FTimerDelegate TimerDelegete;
			TimerDelegete.BindUFunction(this, "Explode");


			FHitResult LineHit;
			FVector TraceStart = HeroRef->CameraFollow->GetComponentLocation();
			FVector TraceEnd = TraceStart + HeroRef->CameraFollow->GetForwardVector() * 250;

			bool Hit = GetWorld()->LineTraceSingleByChannel(LineHit, TraceStart, TraceEnd, ECC_Vehicle);

			if (Hit) {

				// if He Has More Than One Explosive He Will Spawn One in The Place He Want To Put it on
				if (ExplosivesOwnedNum > 1) {

					//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, FString::Printf(TEXT("Spawned A New One ")));
					FVector SpawnLocation = HeroRef->GetMesh()->GetSocketLocation(FName("Secondary Item"));
					FRotator SpawnRotation = HeroRef->GetMesh()->GetSocketRotation(FName("Secondary Item"));
					FActorSpawnParameters SpawnParams;

					//To Spawn New Explosive to The Hit Actor 
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
					AActor* SpawnedC4 = GetWorld()->SpawnActor<AActor>(GetClass(), LineHit.Location, SpawnRotation, SpawnParams);

					//To Attach The New Explosive To The Location Of the Actor That Got Hit 
					FAttachmentTransformRules NewC4_AttachRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
					SpawnedC4->AttachToActor(LineHit.GetActor(), NewC4_AttachRules, LineHit.BoneName);
					FVector NewLocation = LineHit.ImpactPoint + (UKismetMathLibrary::MakeRotFromX(LineHit.ImpactNormal).Vector().X * 10);
					SpawnedC4->SetActorLocation(LineHit.ImpactPoint);

					NewC4Ref = Cast<AChild_Explosive_WithTimer>(SpawnedC4);

					if (NewC4Ref) {

						TimerNum = ExplodeTime;
						//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, FString::Printf(TEXT("Set A Timer To Explode  ")));
						FTimerHandle NewC4Timer;
						GetWorldTimerManager().SetTimer(NewC4Timer, NewC4Ref, &AChild_Explosive_WithTimer::Explode, ExplodeTime, false);

						//To Play A Count Down Sound 
						FTimerDelegate TimeDel;
						TimeDel.BindUFunction(this, "C4TimerSound", NewC4Ref->GetActorLocation());
						GetWorldTimerManager().SetTimer(C4SoundTimer, TimeDel , 1.0 , true);

						// To Reduse The Owned C4s Number 
						ExplosivesOwnedNum--;

					}

				}

				//if He Has jUst One Explosive Left
				if (ExplosivesOwnedNum == 1) {

					FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
					AttachToActor(LineHit.GetActor(), AttachRules, LineHit.BoneName);
					SetActorLocation(LineHit.Location);

					if (IsAttachedTo(LineHit.GetActor())) {

						//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, FString::Printf(TEXT("The Explosive Is Attached To The Actor : %s "), *LineHit.GetActor()->GetName()));
						//Timer To Explode After
						FTimerHandle ExplodeAtTime;
						GetWorldTimerManager().SetTimer(ExplodeAtTime, this, &AChild_Explosive_WithTimer::Explode, ExplodeTime, false);
						ExplosivesOwnedNum--;


					}

				}

			}

		}
		else {

			FTimerHandle ExplodeAtTime;
			GetWorldTimerManager().SetTimer(ExplodeAtTime, this, &AChild_Explosive_WithTimer::Explode, 0.2, false);
			ExplodeTime--;
		}


		if (ExplosivesOwnedNum <= 0) {
			if (HeroRef->SecondaryItem_Slot_One == this) {
				HeroRef->SecondaryItem_Slot_One = nullptr;
				IsEquiped = false;
				HeroRef->WeaponType = EWeaponType::None;

			}
			else if (HeroRef->SecondaryItem_Slot_Two == this) {
				HeroRef->SecondaryItem_Slot_Two = nullptr;
				HeroRef->WeaponType = EWeaponType::None;
			}
		}


	}



}


void AChild_Explosive_WithTimer::OnReleaseItem()
{
}


void AChild_Explosive_WithTimer::ToIncreaseC4_OwnedNumber()
{

	ExplosiveInHandRef_One = Cast<AExplosive_WithTimer>(HeroRef->SecondaryItem_Slot_One);

	if (ExplosiveInHandRef_One) {

		ExplosiveInHandRef_One->ExplosivesOwnedNum++;
		//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, FString::Printf(TEXT(" Explosive Owned Number : %i "), ExplosiveInHandRef_One->ExplosivesOwnedNum));
		if (ExplosiveInHandRef_One->ExplosivesOwnedNum > 1) {
			Destroy();
		}

	}

	ExplosiveInHandRef_Two = Cast<AExplosive_WithTimer>(HeroRef->SecondaryItem_Slot_Two);
	if (ExplosiveInHandRef_Two) {
		ExplosiveInHandRef_Two->ExplosivesOwnedNum++;
		//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, FString::Printf(TEXT(" The Casting of The Second Ref Is Vaild ")));
	}

}


void AChild_Explosive_WithTimer::C4TimerSound(FVector SpawnedActorLocation)
{

	TimerNum -= 1;
	 
	if (TimerNum > 2) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), OneSecondTimerSound, SpawnedActorLocation);
		//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, FString::Printf(TEXT(" Second  ")));
	}
	else if (TimerNum == 2) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), TwoSecondsBeforeExplodeTimerSound , SpawnedActorLocation);
		//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, FString::Printf(TEXT(" Two Seconds Away  ")));
	}
	else if (TimerNum == 1) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), OneSecondBeforeExplodeTimerSound, SpawnedActorLocation);
		//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, FString::Printf(TEXT(" One Second Away  ")));

		GetWorldTimerManager().ClearTimer(C4SoundTimer);
	}
	
}


void AChild_Explosive_WithTimer::Explode()
{

	FVector Start;
	TArray<FHitResult> SweepHitResults;

	if (NewC4Ref) {
		Start = NewC4Ref->GetActorLocation();
	}
	else {
		Start = GetActorLocation();
	}

	FVector End = Start;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereRadius);

	bool Hit = GetWorld()->SweepMultiByChannel(SweepHitResults, Start, End, FQuat::Identity, ECC_Visibility, SphereShape);
	//DrawDebugSphere(GetWorld(), Start, SphereRadius, 5.0, FColor::Purple, false, 7.0);
	OnExplostion.Broadcast();
	TArray<AActor*> ActorsThatGotHit;

	if (Hit) {

		for (FHitResult HitResult : SweepHitResults) {

			if (!ActorsThatGotHit.Contains(HitResult.GetActor())) {

				if (HitResult.GetActor()->GetClass()->ImplementsInterface(UDamage_Systeem_InterFac::StaticClass())) {
					ActorsThatGotHit.Add(HitResult.GetActor());
					IDamage_Systeem_InterFac* DamageInterface = Cast<IDamage_Systeem_InterFac>(HitResult.GetActor());
					if (DamageInterface) {
						DamageInterface->TakeDamage_AllEnemys(ExplosiveDamage, NAME_None);
					}

				}
				else if (HitResult.GetActor()->GetClass()->ImplementsInterface(UPlayer_GotDamageInterface::StaticClass())) {
					ActorsThatGotHit.Add(HitResult.GetActor());
					Cast<IPlayer_GotDamageInterface>(HitResult.GetActor())->Player_TakingDamage_Pure(100, nullptr);
					IPlayer_GotDamageInterface::Execute_Player_TakingDamage(PlayerPawn);
				}

			}


		}

	}

	Destroy();

}

void AChild_Explosive_WithTimer::IncreaseTimeOfExplosive()
{
	if (IsEquiped) {

		if (ExplodeTime < 10) {
			ExplodeTime++;
			OnTimerChange.Broadcast(true , ExplodeTime);
		}

	    //GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, FString::Printf(TEXT(" Wheel UP : %i "), ExplodeTime));
	}


}

void AChild_Explosive_WithTimer::DeIncreaseTimeOfExplosive()
{

	if (IsEquiped) {

		if (ExplodeTime > 0) {
			ExplodeTime--;
			OnTimerChange.Broadcast(false, ExplodeTime);
		}

		//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, FString::Printf(TEXT(" Wheel Down  : %i "), ExplodeTime));

	}

}


