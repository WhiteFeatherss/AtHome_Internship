// Fill out your copyright notice in the Description page of Project Settings.

//Includes :
#include "CubeActor_Tests.h"

#include "Components/BoxComponent.h"

#include "Kismet/KismetMathLibrary.h"

#include "Engine/World.h"
#include "Engine.h"

#include "MC_Behaviour.h"
#include "OutputDebug.h"


// Sets default values
ACubeActor_Tests::ACubeActor_Tests()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

float ACubeActor_Tests::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	TArray<UBoxComponent*> causerComponents;
	DamageCauser->GetComponents(causerComponents);
	float experienceGain = 150;


	for (auto causerComponent : causerComponents)
	{
		if (causerComponent->GetName() == "SwordCollider")
		{
			HealthPoint > 0 ? HealthPoint -= Damage : NULL;
			HealthPoint <= 0 ? GiveXP(experienceGain) : NULL;
			return Damage;
		}
	}

	return Damage;
}

void ACubeActor_Tests::GiveXP(float experienceGain)
{
	AMC_Behaviour* PlayerCharacter = nullptr;

	for (int currentSequence = 1; currentSequence < 4; currentSequence++)
	{
		switch (currentSequence)
		{
		case 1: //Cast to player character.

			PlayerCharacter = Cast<AMC_Behaviour>(UGameplayStatics::GetPlayerCharacter(this, 0));

			if (PlayerCharacter != nullptr)
			{
				break;
			}

		case 2: //Adds experience to the player.

			if (::IsValid(PlayerCharacter))
			{

				PlayerCharacter->EnemyDeath.Broadcast(experienceGain);
				break;
			}

		case 3: //Destroy the dead actor./
			Destroy();

			break;
		}
	}


}


// Called when the game starts or when spawned
void ACubeActor_Tests::BeginPlay()
{

	Super::BeginPlay();

}

// Called every frame
void ACubeActor_Tests::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

