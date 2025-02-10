// Fill out your copyright notice in the Description page of Project Settings.


#include "Alien.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Components/BoxComponent.h"
#include "MyHeroo.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Sound\SoundCue.h"
#include "Alien_AICont.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Player_GotDamageInterface.h"



// Sets default values
AAlien::AAlien()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIPercCom = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Percption Comp"));
	SightSense = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Sense"));

	SightSense->PeripheralVisionAngleDegrees = 70.0;
	SightSense->SightRadius = 1400.0;
	SightSense->LoseSightRadius = 1500.0;
	SightSense->DetectionByAffiliation.bDetectEnemies = true;
	SightSense->DetectionByAffiliation.bDetectFriendlies = true;
	SightSense->DetectionByAffiliation.bDetectNeutrals = true;

	AIPercCom->SetDominantSense(SightSense->GetSenseImplementation());
	AIPercCom->ConfigureSense(*SightSense);

	CheckWallBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Check Wall Box "));
	CheckWallBox->SetupAttachment(GetCapsuleComponent());

	DamageBoxRightLeg = CreateDefaultSubobject<UBoxComponent>(TEXT("Damge Box "));
	DamageBoxRightLeg->SetupAttachment(GetMesh(), FName("RightFoot"));

	PhysicsHandleCompObject_1 = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("Physics Handle 1"));
	PhysicsHandleCompObject_2 = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("Physics Handle 2"));
	PhysicsHandleCompObject_3 = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("Physics Handle 3"));

	//MotionWarpComp = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warping"));

	// To Attack Player On Overlap Box 
	AttackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Attack Box "));
	AttackBox->SetupAttachment(GetMesh());

}


// Called when the game starts or when spawned
void AAlien::BeginPlay()
{

	Super::BeginPlay();

	SpawnDefaultController();

	AIPercCom->OnPerceptionUpdated.AddDynamic(this, &AAlien::OnSensed);

	PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	Hero = Cast<AMyHeroo>(PlayerPawn);

	initial_Health = Alien_Health;

	CheckWallBox->OnComponentBeginOverlap.AddDynamic(this, &AAlien::OnBeginOverlap);

	//To Get The Alien AI Controller

	Alien_AIConRef = Cast<AAlien_AICont>(GetController());

	if (Alien_AIConRef) {
		GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("AIController Isnt Valid ")));
		Alien_AIConRef->ToGetAlienMainRef(this);
	}

	FTimerHandle MoveAroundTimer;
	GetWorldTimerManager().SetTimer(MoveAroundTimer, this, &AAlien::MovingAroundAndToggleAttacks, 2.0, true);

	// To Damage The player 
	DamageBoxRightLeg->OnComponentBeginOverlap.AddDynamic(this, &AAlien::LegDamageBox);

	//To Attack Player On Overlap Box 
	AttackBox->OnComponentBeginOverlap.AddDynamic(this, &AAlien::OnOverlapAttack);

	/*FTimerHandle UnUsedTimer;
	GetWorldTimerManager().SetTimer(UnUsedTimer, this, &AAlien::LongRangeAttack, 3.0, false);*/

	AlienAttackState = EAlienAttackState::NormalWalkingAround;
	AlienMainState = EAlienMainState::GroundAttacksState;

	if (Alien_AIConRef) {
		Alien_AIConRef->AlienObjectHandleState = EAlienObjectHandleState::Handling_OneObject;
	}

	//OnAttackingCast.AddDynamic(this, &AAlien::CallingMulitiMeleeAttacksDamaging);

}

// Called every frame
void AAlien::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!AlienDead) {

		/*if (!StartLerpingDodge && MoveTwice) {
			Dodge();
			MoveTwice = false;
		}*/


		/*if (!GetMesh()->GetAnimInstance()->Montage_IsPlaying(FlyUpMontage) && MontageEnded) {


		}*/

		if (StartLerpingDodge) {
			if (TimeElasped <= LerpDuration) {

				SetActorLocation(FMath::Lerp(GetActorLocation(), MainDodgeLocation, TimeElasped / LerpDuration));
				TimeElasped += DeltaTime;
				//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FString::Printf(TEXT("Alpha : %f"), TimeElasped / LerpDuration));
				if (TimeElasped > LerpDuration) {
					StartLerpingDodge = false;
					//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FString::Printf(TEXT("Time Elasped Is false")));
				}

			}

		}

		if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) {

			NewRotationToTarget(GetActorLocation(), PlayerPawn->GetActorLocation());

		}


		if (AlienMainState == EAlienMainState::GroundAttacksState && AbleToMoveAround) {

			double DistanceFromPlayer = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
			//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Distance From player :%f "), DistanceFromPlayer));
			if (DistanceFromPlayer < 300 && !IsMovingBack && !IsAttacking) {
				ToMoveBack();
				IsMovingBack = true;

			}
			if (DistanceFromPlayer > 1000 && !IsMovingForward && !IsAttacking) {
				//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Blue, FString::Printf(TEXT("Moving Forward Cuz The Player Is Far Away ")));
				ToMoveForward();
				IsMovingForward = true;

			}

		}


		if (Alien_AIConRef) {

			if (PhysicsHandleCompObject_1 && Alien_AIConRef->IsHoldingObject_1 && PhysicsHandleCompObject_1->GetGrabbedComponent()) {
				PhysicsHandleCompObject_1->SetTargetLocation(GetMesh()->GetSocketLocation("Grab Socket"));
			}

			if (PhysicsHandleCompObject_2 && Alien_AIConRef->IsHoldingObject_2 && PhysicsHandleCompObject_2->GetGrabbedComponent()) {
				PhysicsHandleCompObject_2->SetTargetLocation(GetMesh()->GetSocketLocation("Head Grab Socket"));
			}

			if (PhysicsHandleCompObject_3 && Alien_AIConRef->IsHoldingObject_2 && PhysicsHandleCompObject_3->GetGrabbedComponent()) {
				PhysicsHandleCompObject_3->SetTargetLocation(GetMesh()->GetSocketLocation("Left Hand Grab Socket"));
			}

		}


	}

}

// Called to bind functionality to input
void AAlien::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAlien::TakeDamage_AllEnemys(float DamageAmount, FName HitBoneName)
{

	if (!AlienDead) {

		if (CanBeDamaged) {


			FVector MoveTest = Hero->GetActorLocation() - GetActorLocation();
			FVector Direction = MoveTest * 20;
			GetCharacterMovement()->AddImpulse(-Direction);
			//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Alien Got Hit :%s "), *MoveTest.ToString()));

			//-----Damaging The Alien
			Alien_Health -= DamageAmount;

			TakingAnyDamage_Del.Broadcast(Alien_Health);


			if (AlienMainState == EAlienMainState::GroundAttacksState) {

				ArrayOfTakingHits.Add(DamageAmount);
				//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Taking Damage Number : %i "), ArrayOfTakingHits.Num()));

				if (ArrayOfTakingHits.Num() == 1) {

					FTimerHandle TakingHitsTimer;
					GetWorldTimerManager().SetTimer(TakingHitsTimer, [&]() {
						if (ArrayOfTakingHits.Num() >= 2) {
							CantBeBreak = true;
							ShortRangeAttack();
							GetWorldTimerManager().ClearTimer(TakingHitsTimer);
							ArrayOfTakingHits.Empty();
							CantBeBreak = false;
						}
						else {
							GetWorldTimerManager().ClearTimer(TakingHitsTimer);
							ArrayOfTakingHits.Empty();
						}

						}, 1.0, false);

				}

				if (Alien_Health < initial_Health / 1.3 && Alien_Health >= initial_Health / 1.4) {

					Alien_AIConRef->AlienObjectHandleState = EAlienObjectHandleState::Handling_TwoObjects;
				}

				if (DamageAmount >= 150) {
					TakingHeavyDamage_Del.Broadcast();
				}

			}
			else if (AlienMainState == EAlienMainState::FlyingAttacksState) {

				if (ArrayOfTakingHits.Num() == 1) {

					FTimerHandle TakingHitsTimer;
					GetWorldTimerManager().SetTimer(TakingHitsTimer, [&]() {
						if (ArrayOfTakingHits.Num() >= 2) {
							Dodge();
							GetWorldTimerManager().ClearTimer(TakingHitsTimer);
							ArrayOfTakingHits.Empty();
						}
						else {
							GetWorldTimerManager().ClearTimer(TakingHitsTimer);
							ArrayOfTakingHits.Empty();
						}

						}, 1.0, false);

				}

				//Taking A Heavy Damage On Flying Mode 
				if (DamageAmount >= 150 && !IsFallingToGround) {
					
					FallingToGround();

				}

			}

			//Taking Hit Reactions Based on the damage takin 
			if (!CantBeBreak) {

				if (DamageAmount < 50) {
					if (Getting_Small_Reaction_1) {
						OnGetSmallReaction.Broadcast(true, false, false);
					}

				}

				if (DamageAmount >= 50 && DamageAmount < 94) {
					if (Getting_Medium_Reaction_1 && Getting_Medium_Reaction_2 && Getting_Medium_Reaction_3) {
						OnGetSmallReaction.Broadcast(false, true, false);
						GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Medium Animation Played  ")));
					}

				}
				if (DamageAmount > 94) {
					if (Getting_Large_Reaction_1) {
						OnGetSmallReaction.Broadcast(false, false, true);
					}
				}

			}



			//
			GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FString::Printf(TEXT("Alien Got Hit : %f"), Alien_Health));

			//------ To Change To Flying Mode When The Alien Have Half of His Health 
			if (AlienMainState == EAlienMainState::GroundAttacksState && Alien_Health < initial_Health / 1.5) {

				Alien_AIConRef->StopMovement();
				AlienMainState = EAlienMainState::FlyingAttacksState;
				Alien_AIConRef->AlienObjectHandleState = EAlienObjectHandleState::Handling_ThreeObjects;
				NumToAttackAfter = 0;
				OnActivateShield_Del.Broadcast(true);
				FTimerHandle RemoveDynamicTimer;
				MontageEnded = true;
				AbleToMoveAround = false;
				CanBeDamaged = false;
				Alien_AIConRef->StopMovement();
				GetCharacterMovement()->SetMovementMode(MOVE_Flying);
				GetWorldTimerManager().SetTimer(RemoveDynamicTimer, [&]() {PlayAnimMontage(FlyUpMontage); }, 1.0, false);
				GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FString::Printf(TEXT("Set The Flying Mode ON ")));
				FTimerHandle ToEndMontageStateTimer;
				GetWorldTimerManager().SetTimer(ToEndMontageStateTimer, [&]() { OnActivateShield_Del.Broadcast(false);  AbleToMoveAround = true;  MontageEnded = false;  CanBeDamaged = true; }, 9.0, false);

			}

			
		}



		//---------------------------- Shield Skill Section ------------------------------------

		if (Alien_Health <= initial_Health / 2 && !AlreadyUsedShieldIn_HalfHealth) {
			OnActivateShield_Del.Broadcast(true);
			Alien_AIConRef->ShieldHealth = Alien_AIConRef->initialHealth;
			AlreadyUsedShieldIn_HalfHealth = true;
		}
		else if (Alien_Health < initial_Health / 3.6 && !AlreadyUsed_Shield_SecondTime) {
			OnActivateShield_Del.Broadcast(true);
			Alien_AIConRef->ShieldHealth = Alien_AIConRef->initialHealth;
			AlreadyUsed_Shield_SecondTime = true;
		}

		if (Alien_AIConRef->IsShieldActivated) {

			Alien_AIConRef->ShieldHealth -= DamageAmount;
			GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Purple, FString::Printf(TEXT("Damaged The Shield : %f"), Alien_AIConRef->ShieldHealth));

			// Desable Shield on taking Heavy Damage
			if (DamageAmount >= 150) {

				OnActivateShield_Del.Broadcast(false);
				FallingToGround();

			}
			else if (Alien_AIConRef->ShieldHealth <= 0) {
				OnActivateShield_Del.Broadcast(false);
			}

		}
		//-------------------------Shield End----------------------



		// To Set The Alien To Be Killed If his health is ZERO
		if (Alien_Health <= 0) {
			AlienDead = true;
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			GetMesh()->SetSimulatePhysics(true);
			this->Tags.Add("Grab");
			AlienDeath_Del.Broadcast();

		}


	}

}

void AAlien::Deflection_impect()
{

}


//To Change Speed And Attacking State 
void AAlien::ToChangeStateAndSpeed(EAlienAttackState NewState, float Speed)
{

	AlienAttackState = NewState;
	GetCharacterMovement()->MaxWalkSpeed = Speed;

}


void AAlien::OnSensed(const TArray<AActor*>& UpdatedActors)
{

	if (!AlienDead) {

		for (AActor* SensedActor : UpdatedActors) {

			MainHeroRef = Cast<AMyHeroo>(SensedActor);

		}

		if (MainHeroRef) {

			for (int i = 0; i < UpdatedActors.Num(); i++) {

				FActorPerceptionBlueprintInfo ActorInfo;
				AIPercCom->GetActorsPerception(UpdatedActors[i], ActorInfo);

				if (ActorInfo.LastSensedStimuli[0].WasSuccessfullySensed()) {

					PlayerSensed = true;
					if (Alien_AIConRef) {

						//Alien_AIConRef->MoveToActor(Hero);
					}
					//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FString::Printf(TEXT("Alien Sensed the player")));

				}
				else {

					PlayerSensed = false;
					//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Alien Cant See the player")));

				}

			}

		}

	}

}

void AAlien::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	auto Walls = Cast<UStaticMesh>(OtherActor);
	if (Walls) {

	}

}

// ---------------------------Air Dodge----------------------- 
void AAlien::Dodge()
{

	if (!AlienDead && !StartLerpingDodge && !GetMesh()->GetAnimInstance()->Montage_IsPlaying(FlyUpMontage)) {

		FVector LastLocation = GetActorLocation();

		double DistanceFromPlayer = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());


		//The rest of the function in the blueprint 
		if (DistanceFromPlayer < 1300) {
			DodgeToPlayer = false;
		}
		else if (DistanceFromPlayer > 1300) {
			DodgeToPlayer = true;
		}

		OnDodge.Broadcast();

		//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Red, FString::Printf(TEXT("Dodging ")));
		if (MainDodgeLocation.Z < Hero->GetActorLocation().Z + 550) {
			MainDodgeLocation.Z = Hero->GetActorLocation().Z + 550;
			//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Red, FString::Printf(TEXT("Dodged And Adjusted the Height Of The Alien ")));
		}

		if (MainDodgeLocation != FVector::ZeroVector && CheckIfSafeToDodge(MainDodgeLocation)) {

			//MainDodgeLocation = NewLocationToRight;
			StartLerpingDodge = true;
			//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Red, FString::Printf(TEXT("Dodging ")));
			TimeElasped = 0.0;

		}

	}

}

bool AAlien::CheckIfSafeToDodge(FVector DodgeLocation)
{

	TArray<FHitResult> SweepHitResults;
	FVector Start = DodgeLocation;
	FVector End = Start;
	FVector BoxExtent = FVector(200, 200.0, 50.0);
	FQuat QuatRotation = FQuat(0.0, 0.0, 0.0, 0.0);
	FCollisionShape CollisionShape = FCollisionShape::MakeBox(BoxExtent);
	FCollisionQueryParams QueryParams;

	bool Hit = GetWorld()->SweepMultiByChannel(SweepHitResults, Start, End, QuatRotation, ECC_Visibility, CollisionShape, QueryParams);
	//DrawDebugBox(GetWorld(), Start, BoxExtent, QuatRotation, FColor::Black, false, 4.0, -1, 2.0);
	if (Hit) {
		//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Blue, FString::Printf(TEXT("Hit Something ")));
		for (FHitResult SingleHitResult : SweepHitResults) {
			AStaticMeshActor* Wall = Cast<AStaticMeshActor>(SingleHitResult.GetActor());
			if (Wall) {
				//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Red, FString::Printf(TEXT("He Found A Wall He Can't Move Here ")));
				return false;
				break;
			}
		}
		return true;

	}
	else {
		return true;
	}

	return false;
}


void AAlien::MovingAroundAndToggleAttacks()
{

	if (Alien_AIConRef && !AlienDead && !StartLerpingDodge && AbleToMoveAround) {

		// ------------------------ GROUND State Moving And Attacks ---------------------------

		if (GetVelocity().Length() == 0.0 && AlienMainState == EAlienMainState::GroundAttacksState && AlienAttackState == EAlienAttackState::NormalWalkingAround) {

			double DistanceFromPlayer = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
			FVector LastLocation = GetActorLocation();

			int RandNum = FMath::RandRange(1, 2);

			switch (RandNum)
			{
				//Right Move
			case(1):
				if (!IsRight_BoxOverlaping) {
					FVector NewLocationToRight = FVector(GetActorLocation().X + Right_WalkingAmount, GetActorLocation().Y, GetActorLocation().Z);
					Alien_AIConRef->MoveToLocation(NewLocationToRight);

					break;

				}
				//Left Move
			case(2):
				if (!IsLeft_BoxOverlaping) {
					FVector NewLocationToLeft = FVector(GetActorLocation().X + Left_WalkingAmount, GetActorLocation().Y, GetActorLocation().Z);
					Alien_AIConRef->MoveToLocation(NewLocationToLeft);
					//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Number To Attack After ")));
					break;

				}

			default:
				break;
			}


			// Calling Function To See if its Time To Attack
			ToggleAttacks();

		}

		if (AlienMainState == EAlienMainState::FlyingAttacksState && AlienAttackState == EAlienAttackState::NormalWalkingAround) {

			// Calling Function To See if its Time To Attack
			ToggleAttacks();

		}

		//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Number To Attack After : %i "), NumToAttackAfter));

	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("AI Controller isnt Valid ")));
	}

}

void AAlien::ToMoveBack()
{
	if (Alien_AIConRef) {

		//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Back Movement ")));
		FVector NewLocationToBack = FVector(GetActorLocation().X, GetActorLocation().Y + Back_DodgeAmount, GetActorLocation().Z);
		Alien_AIConRef->MoveToLocation(NewLocationToBack);
		/*if (NumToAttackAfter == 2) {
			LongRangeAttack();
			NumToAttackAfter = 0;
		}*/
		FTimerHandle BackMoveTimer;
		GetWorldTimerManager().SetTimer(BackMoveTimer, [&]() { IsMovingBack = false; }, 2.0, false);

	}

}


void AAlien::ToMoveForward()
{

	if (Alien_AIConRef && MainHeroRef) {
		FVector Distance = GetActorLocation() - MainHeroRef->GetActorLocation();
		FVector HalfDistance = Distance / 2;
		FVector NewLocationForward = GetActorLocation() - HalfDistance;

		//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Forward Movement  ")));
		//FVector NewLocationToBack = FVector(GetActorLocation().X, GetActorLocation().Y + Forward_WalkingAmount, GetActorLocation().Z);
		Alien_AIConRef->MoveToLocation(NewLocationForward);

	}

	FTimerHandle ForwardMoveTimer;
	GetWorldTimerManager().SetTimer(ForwardMoveTimer, [&]() { IsMovingForward = false; }, 5.0, false);

}


void AAlien::NewRotationToTarget(FVector ActorLocation, FVector TargetLocation)
{
	if (!AlienDead) {
		FVector Distance = TargetLocation - ActorLocation;
		FRotator NewRotation = Distance.Rotation();
		NewRotation.Roll = 0.0;
		NewRotation.Pitch = 0.0;
		SetActorRotation(NewRotation);

	}

}


///--------------------------------------------------------- ATTACKING Functions --------------------------------------------------------------------


void AAlien::LegDamageBox(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!AlienDead) {
		//IPlayer_Interface* PlayerInterface = Cast<IPlayer_Interface>(OtherActor);
			//PlayerInterface->OnPlayerTakeDamage(20);
			/*FTimerHandle DodgeTimer;
			GetWorldTimerManager().SetTimer(DodgeTimer, this, &AAlien::Dodge, 1.0, false);*/

	}

}

void AAlien::ToggleAttacks()
{

	// ------------------------ Ground State Moving And Attacks ---------------------------

	if (AlienMainState == EAlienMainState::GroundAttacksState) {

		// Attacking Setup 
		if (NumToAttackAfter == 1) {
			ShortRangeAttack();
			FTimerHandle EndShortRangeAttackTimer;
			GetWorldTimerManager().SetTimer(EndShortRangeAttackTimer, [&]() { ShortRangeAttack(); }, 1.5, false);

			//LongRangeAttack();
		}
		else if (NumToAttackAfter == 2) {
			ShortRangeAttack();
			/*FTimerHandle EndShortRangeAttackTimer;
			GetWorldTimerManager().SetTimer(EndShortRangeAttackTimer, [&]() { ShortRangeAttack(); }, 1.5, false);*/
			//LongRangeAttack();
			Alien_AIConRef->ThrowedSecondOne = false;
		}
		else if (NumToAttackAfter == 3) {
			LongRange_PickUpAndThrowOROnlyThrowAttack(true);
			//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("He Got To Throwing  ")));
			FTimerHandle EndShortRangeAttackTimer;
			GetWorldTimerManager().SetTimer(EndShortRangeAttackTimer, [&]() { LongRange_PickUpAndThrowOROnlyThrowAttack(true); }, 4.5, false);
		}
		else if (NumToAttackAfter == 4) {
			LongRange_PickUpAndThrowOROnlyThrowAttack(true);
			NumToAttackAfter = 0;
		}

		NumToAttackAfter++;

	}

	// ------------------------ FLYING State Moving And Attacks ---------------------------

	if (AlienMainState == EAlienMainState::FlyingAttacksState) {

		Dodge();
		//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Toggleing Attacking In The Flying State  ")));
		if (NumToAttackAfter == 2) {
			LongRange_PickUpAndThrowOROnlyThrowAttack(true);

		}
		else if (NumToAttackAfter == 3) {

			LongRange_PickUpAndThrowOROnlyThrowAttack(true);
		}
		else if (NumToAttackAfter == 5) {
			LongRange_PickUpAndThrowOROnlyThrowAttack(false);
			NumToAttackAfter = 0;
		}
		/*else if (NumToAttackAfter == 7) {
			LongRange_PickUpAndThrowOROnlyThrowAttack(true);

			NumToAttackAfter = 0;
		}*/

		NumToAttackAfter++;

	}

}


void AAlien::LongRange_PickUpAndThrowOROnlyThrowAttack(bool ShouldPickUpObject)
{

	if (!IsAttacking) {

		if (!Alien_AIConRef->IsHoldingObject_1 && !Alien_AIConRef->IsHoldingObject_2 && !Alien_AIConRef->IsHoldingObject_3) {

			//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("He Is Using The Long Range ")));
			IsAttacking = true;
			ToChangeStateAndSpeed(EAlienAttackState::LongRangeAttackState, WalkingSpeed);
			AlienAttackState = EAlienAttackState::LongRangeAttackState;


			FTimerHandle GrabTimer;
			FTimerDelegate TimerDel;
			TimerDel.BindUFunction(Alien_AIConRef, "ToGetSomethingToGrab", ShouldPickUpObject);
			GetWorldTimerManager().SetTimer(GrabTimer, TimerDel, 1.5, false);

			//GetWorldTimerManager().SetTimer(GrabTimer, Alien_AIConRef, &AAlien_AICont::ToGetSomethingToGrab, 1.5, false);

		}

	}

}



void AAlien::ShortRangeAttack()
{

	//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Short Range Attack ")));
	IsAttacking = true;
	AlienAttackState = EAlienAttackState::RunAndAttackState;
	ToChangeStateAndSpeed(EAlienAttackState::RunAndAttackState, RunSpeed);
	AbleToMoveAround = false;
	Alien_AIConRef->StopMovement();
	//Alien_AIConRef->MoveToActor(PlayerPawn, 2.0);

	OnShortAttackDelegte.Broadcast();

	//-----Ending Short Ranage Attack After Some Time---
	FTimerHandle EndShortRangeAttackTimer;
	GetWorldTimerManager().SetTimer(EndShortRangeAttackTimer, [&]() {
		IsAttacking = false;
		ToChangeStateAndSpeed(EAlienAttackState::NormalWalkingAround, WalkingSpeed);
		AbleToMoveAround = true;
		AlienAttackState = EAlienAttackState::NormalWalkingAround;

		}, 1.0, false);

}


void AAlien::OnOverlapAttack(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (!AlienDead) {

		AMyHeroo* HeroGotAttackedRef = Cast<AMyHeroo>(OtherActor);

		if (HeroGotAttackedRef) {
			if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) {

				bool IsRight;

				int RandNum = FMath::RandRange(0, 2);
				FVector PlayerLocation = PlayerPawn->GetActorLocation();
				switch (RandNum)
				{
				case(0):
					OnAttackingCast.Broadcast(RandNum, PlayerLocation, true);
					//PlayAnimMontage(Attack_One);
					IsRight = true;
					NewRotationToTarget(GetActorLocation(), PlayerPawn->GetActorLocation());
					break;
				case(1):
					OnAttackingCast.Broadcast(RandNum, PlayerLocation, false);
					IsRight = false;
					//PlayAnimMontage(Attack_Two);
					NewRotationToTarget(GetActorLocation(), PlayerPawn->GetActorLocation());
					break;
				case(2):
					OnAttackingCast.Broadcast(RandNum, PlayerLocation, true);
					IsRight = true;
					//PlayAnimMontage(Attack_Three);
					NewRotationToTarget(GetActorLocation(), PlayerPawn->GetActorLocation());
					break;

				}


				IsAttacking = false;
				ToChangeStateAndSpeed(EAlienAttackState::NormalWalkingAround, WalkingSpeed);
				AbleToMoveAround = true;
				AlienAttackState = EAlienAttackState::NormalWalkingAround;

			}

		}

	}

}


void AAlien::ToCallPlayMontage()
{
	if (ThrowMontage) {
		PlayAnimMontage(ThrowMontage);
	}

}


//------------------------------------- Hitting in the short Range attack ------------------------------------


void AAlien::CallingMulitiMeleeAttacksDamaging(int AttackNumber, FVector AttackLocation, bool IsUsingRightLeg)
{

	FTimerDelegate TimerDelegateForDamaging;
	TimerDelegateForDamaging.BindUFunction(this, "MeleeAttacksDamaging", AttackNumber, AttackLocation, IsUsingRightLeg);

	GetWorldTimerManager().SetTimer(MeleeAttackDamage_TimerHandle, TimerDelegateForDamaging, 0.02, true);

}


void AAlien::MeleeAttacksDamaging(int AttackNumber, FVector AttackLocation, bool IsUsingRightLeg)
{

	if (IsDamagingON) {

		TArray<FHitResult> SphereHitResults;
		if (IsUsingRightLeg) {
			FVector RightLegLocation = GetMesh()->GetSocketLocation(FName("RightToeBase"));
			SphereMainLocation = RightLegLocation;
		}
		else if (!IsUsingRightLeg) {
			FVector LeftLegLocation = GetMesh()->GetSocketLocation(FName("LeftToeBase"));
			SphereMainLocation = LeftLegLocation;
		}

		FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereRedius);

		bool Hit = GetWorld()->SweepMultiByChannel(SphereHitResults, SphereMainLocation, SphereMainLocation, FQuat::Identity, ECC_WorldDynamic, SphereShape);
		//DrawDebugSphere(GetWorld(), SphereMainLocation, SphereRedius, 1, FColor::Red, false, 2.0, 0, 2);

		if (Hit) {

			for (FHitResult SphereOneHit : SphereHitResults) {
				//To See if the Hit actor has the interface of the player 
				if (SphereOneHit.GetActor()->GetClass()->ImplementsInterface(UPlayer_GotDamageInterface::StaticClass()) && !DamagedSomeone) {

					Cast<IPlayer_GotDamageInterface>(SphereOneHit.GetActor())->Player_TakingDamage_Pure(20, this);
					IPlayer_GotDamageInterface::Execute_Player_TakingDamage(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
					//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Damaged The player")));
					DamagedSomeone = true;

				}

			}

		}

	}


}


void AAlien::StoppingSphere()
{
	GetWorldTimerManager().ClearTimer(MeleeAttackDamage_TimerHandle);

}


//To Fall To The Ground After Taking A Huge Damage  
void AAlien::FallingToGround()
{

	AlienMainState = EAlienMainState::FallingAfterHit;
	AbleToMoveAround = false;
	IsFallingToGround = true;
	StopAttacking = true;
	TakingHeavyDamage_Del.Broadcast();

	FTimerHandle ToEndMontageStateTimer;
	GetWorldTimerManager().SetTimer(ToEndMontageStateTimer, [&]() { GetCharacterMovement()->SetMovementMode(MOVE_Falling); }, 0.7, false);

}




