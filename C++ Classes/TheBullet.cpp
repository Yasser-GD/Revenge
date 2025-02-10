// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBullet.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MyHeroo.h"
#include "EnemyBoss.h"
#include "Damage_Systeem_InterFac.h"
#include "Kar98.h"

#include "Kismet/GameplayStatics.h"


// Sets default values
ATheBullet::ATheBullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletProjectile = CreateDefaultSubobject <UProjectileMovementComponent>(TEXT(" Projecttile Movement Component"));
	ColitionSphere = CreateDefaultSubobject <USphereComponent>(TEXT("Sphere Colition"));
	RootComponent = ColitionSphere;
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Mesh"));
	BulletMesh->SetupAttachment(ColitionSphere);
	//RootComponent = BulletMesh;

	BulletProjectile->UpdatedComponent = ColitionSphere;
	BulletProjectile->InitialSpeed = 4000.0f;
	BulletProjectile->MaxSpeed = 3000.0f;
	BulletProjectile->bShouldBounce = true;
	BulletProjectile->bRotationFollowsVelocity = true;
	BulletProjectile->ProjectileGravityScale = 0.0f;

	InitialLifeSpan = 10.0f;

}

// Called when the game starts or when spawned
void ATheBullet::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	HerooRef = Cast<AMyHeroo>(PlayerPawn);
	if (HerooRef) {

		//GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Cyan, FString::Printf(TEXT("The Bullet Got The Player Referance")));
	}

	ColitionSphere->OnComponentBeginOverlap.AddDynamic(this, &ATheBullet::OnBeginOverlap);
	//BulletMesh->OnComponentHit.AddDynamic(this, &ATheBullet::OnMeshHit);

}

// Called every frame
void ATheBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATheBullet::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	//GEngine->AddOnScreenDebugMessage(-1, 7.0, FColor::Cyan, FString::Printf(TEXT("What The Bullet Hit IN Main : %s"), *OtherActor->GetName()));

	if (OtherActor->GetClass()->ImplementsInterface(UDamage_Systeem_InterFac::StaticClass())) {
		//GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Blue, FString::Printf(TEXT("Hit the Bosssss : %s"), *OtherActor->GetName()));
		//GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Cyan, FString::Printf(TEXT("Hit the Bosssss : %s"), *SweepResult.BoneName.ToString()));

		if (SweepResult.BoneName == FName("Head")) {
			Cast<IDamage_Systeem_InterFac>(OtherActor)->TakeDamage_AllEnemys(90, SweepResult.BoneName);
		}
		else {
			Cast<IDamage_Systeem_InterFac>(OtherActor)->TakeDamage_AllEnemys(BulletDamage, SweepResult.BoneName);
		}

		Destroy();

	}
	else if (OtherActor != this) {
		Destroy();
		//GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Blue, FString::Printf(TEXT("Hit SomeThing Else "), *OtherActor->GetName()));
	}

}

void ATheBullet::OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}



