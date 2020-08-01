#include "OutputDebug.h"
#include "MC_Behaviour.h"
#include "Math/Vector.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"

#include "Knight_Behaviour.h"


//Defines
#define VECTOR_NULL FVector(0.000000, 0.000000, 0.000000)
#define NOFUNCTION void()

namespace KnightConstants
{
	constexpr float experienceGive = 500.0F;
	constexpr float knightDamage = 100.0F;
	constexpr float knightLongRangeDamage = 150.0F;

	constexpr float knight_FOV = 70.0F;
	constexpr float knight_SightRadius = 3000.0F;

	constexpr float acceptanceRadius = 150.0F;

	constexpr int hitTrigger = 4;

	constexpr float unseenTrigger = 2.0F;
}
namespace KnightVariables
{
	float knightHealth = 450.0F;
	float playerIsBeingSeen = false;
	
	bool bPowerUp = false;

	float AccumulatedSeconds = 0;

	int numberOfTimeHit = 0;
}


AKnight_Behaviour::AKnight_Behaviour()
{
	PrimaryActorTick.bCanEverTick = true;

	KnightVariables::knightHealth = 100.0F;
	MaxHealth = KnightVariables::knightHealth;
	CurrentHealth = KnightVariables::knightHealth;



	//Add pawn sensing to Demon : 
	PawnSensor = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	PawnSensor->SetPeripheralVisionAngle(KnightConstants::knight_FOV);
	PawnSensor->SightRadius = KnightConstants::knight_SightRadius;
}
void AKnight_Behaviour::BeginPlay()
{
	Super::BeginPlay();	

	KnightVariables::bPowerUp = false;
	KnightVariables::numberOfTimeHit = 0;



	PawnSensor->OnSeePawn.AddDynamic(this, &AKnight_Behaviour::OnSeePawn); //When sees something calls the OnSeePawn function.
}
void AKnight_Behaviour::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

	if (KnightVariables::numberOfTimeHit == KnightConstants::hitTrigger)
	{
		KnightVariables::numberOfTimeHit = 0;
		LongRangeAttack();
	}

	//RetriggerableDelay(&AKnight_Behaviour::ResetVision, DeltaTime, KnightConstants::unseenTrigger);
}
void AKnight_Behaviour::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


//Damage and Experience :
float AKnight_Behaviour::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	KnightVariables::numberOfTimeHit++;

	OnHit();

	TArray<UBoxComponent*> causerComponents;

	DamageCauser->GetComponents(causerComponents);

	float experienceGain = KnightConstants::experienceGive;

	for (auto causerComponent : causerComponents)
	{
		if (causerComponent->GetName() == "SwordCollider")
		{
			KnightVariables::knightHealth > 0 ? KnightVariables::knightHealth -= Damage : NULL;
			KnightVariables::knightHealth <= 0 ? GiveXP(experienceGain) : NULL;
			CurrentHealth = KnightVariables::knightHealth;
			return Damage;
		}
	}

	return Damage;
}
void AKnight_Behaviour::DoDamageToPlayer(float damageInput)
{

	AMC_Behaviour* PlayerCharacter = CastToPlayerCharacter();

	if (::IsValid(PlayerCharacter) && (damageInput == 0))
	{
		PlayerCharacter->EnemyDamage.Broadcast(KnightConstants::knightDamage);
	}
	else
	{
		PlayerCharacter->EnemyDamage.Broadcast(damageInput);
	}
}



void AKnight_Behaviour::GiveXP(float experienceGain)
{
	AMC_Behaviour* PlayerCharacter = CastToPlayerCharacter();

	if (isDead == false)
	{
		PlayerCharacter->EnemyDeath.Broadcast(experienceGain);
		OnDeath();
	}

}


//Pawn Sensor :
void AKnight_Behaviour::OnSeePawn(APawn* OtherPawn)
{
	AMC_Behaviour* PlayerCharacter = CastToPlayerCharacter();

	PowerUp();

	AI_MoveTo(VECTOR_NULL, PlayerCharacter, 250.0F);
	
}

void AKnight_Behaviour::PowerUp()
{
	if (KnightVariables::bPowerUp == false)
	{
		KnightVariables::bPowerUp = true;

		this->GetCharacterMovement()->Deactivate();

		PowerUp_Anim(); //Redirects to BP_DemonAI.//
	}
}

//Death

//Reusable :
AMC_Behaviour* AKnight_Behaviour::CastToPlayerCharacter()
{
	AMC_Behaviour* PlayerCharacter = Cast<AMC_Behaviour>(UGameplayStatics::GetPlayerCharacter(this, 0));
	return	PlayerCharacter;
}
void AKnight_Behaviour::AI_MoveTo(const FVector& destination, AMC_Behaviour* PlayerCharacter, float acceptanceRadius)
{
	UAIAsyncTaskBlueprintProxy* unsignificantVariableName = nullptr;

	unsignificantVariableName = UAIBlueprintHelperLibrary::CreateMoveToProxyObject
	(
		this,
		this,
		destination,
		PlayerCharacter,
		acceptanceRadius,
		false
	);
}
void AKnight_Behaviour::RetriggerableDelay(FunctionToCall functionToCall, float deltaSeconds, float triggerThreshold)
{
	KnightVariables::AccumulatedSeconds += deltaSeconds;

	if (KnightVariables::AccumulatedSeconds >= triggerThreshold)
	{
		(this->*functionToCall)();

		KnightVariables::AccumulatedSeconds -= triggerThreshold;
	}
}
