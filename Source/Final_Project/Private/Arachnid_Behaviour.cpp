#include "OutputDebug.h"
#include "MC_Behaviour.h"
#include "Math/Vector.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"

#include "Arachnid_Behaviour.h"

//Defines
#define VECTOR_NULL FVector(0.000000, 0.000000, 0.000000)
#define NOFUNCTION void()


namespace ArachnidConstants
{
	constexpr float experienceGive = 1000.0F;
	constexpr float arachnidDamage = 75.0F;

	constexpr float arachnid_FOV = 30.0F;
	constexpr float arachnid_SightRadius = 3000.0F;

	constexpr float acceptanceRadius = 500.0F;

	constexpr float maximumHealth = 1250.0f;

	AMC_Behaviour* MC = nullptr;
}
namespace ArachnidVariables
{
	float arachnidHealth = 1250.0F;
	
	float playerIsBeingSeen = false;

	bool bPowerUp = false;

	float AccumulatedSeconds = 0;

	float distanceFromPlayer = 0;
}


AArachnid_Behaviour::AArachnid_Behaviour()
{
	PrimaryActorTick.bCanEverTick = true;

	//Add pawn sensing to Demon : 
	PawnSensor = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	PawnSensor->SetPeripheralVisionAngle(ArachnidConstants::arachnid_FOV);
	PawnSensor->SightRadius = ArachnidConstants::arachnid_SightRadius;
}
void AArachnid_Behaviour::BeginPlay()
{
	Super::BeginPlay();

	ArachnidConstants::MC = CastToPlayerCharacter();

	PawnSensor->OnSeePawn.AddDynamic(this, &AArachnid_Behaviour::OnSeePawn); //When sees something calls the OnSeePawn function.
}
void AArachnid_Behaviour::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AArachnid_Behaviour::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}//Damage and Experience :

//Damage :
float AArachnid_Behaviour::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	OnHit();

	TArray<UBoxComponent*> causerComponents;

	DamageCauser->GetComponents(causerComponents);

	float experienceGain = ArachnidConstants::experienceGive;

	for (auto causerComponent : causerComponents)
	{
		if (causerComponent->GetName() == "SwordCollider")
		{
			ArachnidVariables::arachnidHealth > 0 ? ArachnidVariables::arachnidHealth -= Damage : NULL;
			ArachnidVariables::arachnidHealth <= 0 ? GiveXP(experienceGain) : NULL;
			return Damage;
		}
	}

	return Damage;
}
void AArachnid_Behaviour::DoDamageToPlayer(float damageInput)
{
	AMC_Behaviour* PlayerCharacter = CastToPlayerCharacter();
	OutputDebug::DebugFStringFloat(damageInput, "Did damage");

	if (::IsValid(PlayerCharacter) && (damageInput == 0))
	{
		PlayerCharacter->EnemyDamage.Broadcast(ArachnidConstants::arachnidDamage);
	}
	else
	{
		PlayerCharacter->EnemyDamage.Broadcast(damageInput);
	}
}

//Experience :
void AArachnid_Behaviour::GiveXP(float experienceGain)
{
	AMC_Behaviour* PlayerCharacter = CastToPlayerCharacter();

	if (isDead == false)
	{
		PlayerCharacter->EnemyDeath.Broadcast(experienceGain);
		OnDeath();
	}

}

//Health :
float AArachnid_Behaviour::Get_CurrentHealth()
{
	return ArachnidVariables::arachnidHealth;
}
float AArachnid_Behaviour::Get_MaximumHealth()
{
	return ArachnidConstants::maximumHealth;
}


//Pawn Sensor :
void AArachnid_Behaviour::OnSeePawn(APawn* OtherPawn)
{
	if (isShouting == false)
	{
		AI_MoveTo(VECTOR_NULL, ArachnidConstants::MC, ArachnidConstants::acceptanceRadius);
	}
}

//Reusable :
AMC_Behaviour* AArachnid_Behaviour::CastToPlayerCharacter()
{
	AMC_Behaviour* PlayerCharacter = Cast<AMC_Behaviour>(UGameplayStatics::GetPlayerCharacter(this, 0));
	return	PlayerCharacter;
}
void AArachnid_Behaviour::AI_MoveTo(const FVector& destination, AMC_Behaviour* PlayerCharacter, float acceptanceRadius)
{
	OutputDebug::DebugFString("Moving toward player");

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
void AArachnid_Behaviour::RetriggerableDelay(FunctionToCall functionToCall, float deltaSeconds, float triggerThreshold)
{
	ArachnidVariables::AccumulatedSeconds += deltaSeconds;

	if (ArachnidVariables::AccumulatedSeconds >= triggerThreshold)
	{
		(this->*functionToCall)();

		ArachnidVariables::AccumulatedSeconds -= triggerThreshold;
	}
}

