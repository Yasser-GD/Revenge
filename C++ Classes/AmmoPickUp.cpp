// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickUp.h"

#include "Components/BoxComponent.h"
#include "MyHeroo.h"
#include "EquipAndUn_Items_Interface.h"

// Sets default values
AAmmoPickUp::AAmmoPickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AmmoBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Ammo Box"));
	AmmoBox->SetupAttachment(RootComponent);
	RootComponent = AmmoBox;
	AmmoBox->OnComponentBeginOverlap.AddDynamic(this, &AAmmoPickUp::OnBeginOverlap);
	
}

// Called when the game starts or when spawned
void AAmmoPickUp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAmmoPickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAmmoPickUp::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	AMyHeroo* PlayerRef = Cast<AMyHeroo>(OtherActor);
	
	if (PlayerRef)
	{
		IEquipAndUn_Items_Interface* SlotOne_HasInterface = Cast<IEquipAndUn_Items_Interface>(PlayerRef->AslotForItems_One);
		if (SlotOne_HasInterface)
		{
			SlotOne_HasInterface->AmmoPickUp(Ammo_Amount , AmmoType);
			Destroy();
		}
		
		IEquipAndUn_Items_Interface* SlotTwo_HasInterface = Cast<IEquipAndUn_Items_Interface>(PlayerRef->AslotForItems_Two);
		if (SlotTwo_HasInterface)
		{
			SlotTwo_HasInterface->AmmoPickUp(Ammo_Amount , AmmoType);
			Destroy();
		}
	}
	
}

