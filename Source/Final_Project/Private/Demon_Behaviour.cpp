#include "OutputDebug.h"
#include "MC_Behaviour.h"
#include "Math/Vector.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Demon_Behaviour.h"

#define VECTOR_NULL FVector(0.000000, 0.000000, 0.000000)
#define NOFUNCTION NULL

namespace DemonConstants
{
	constexpr float Demon_FOV = 57.0F; //Average human binocular visual field
	constexpr float Demon_SightRadius = 5000.0F; // 2500 = 25m
	constexpr float Demon_VisionReset = 1.2F;

	constexpr float idle_MaxSpeed = 100.0F;
	constexpr float idle_MaxAcceleration = 150.0F;

	constexpr float triggerSpeed_Initial = 70.0F; //B
	constexpr float triggerSpeed_Maximum = 380.0F; //A

	constexpr float triggerAccel_Initial = 300.0F; //B
	constexpr float triggerAccel_Maximum = 100.0F; //A

	constexpr float triggerSpeed_Reset = 500.0F; 
	constexpr float triggerAccel_Reset = 400.0F;

	//Experience : 
	constexpr float experienceGive = 150.0F;

	//Damage : 
	constexpr float demonDamage = 50.0F;
}
namespace DemonVariables
{
	//AI Move To :
	float acceptanceRadius = 150.0F;


	//Get Random Location : 
	float randomReachableRadius = 5000.0F;
	FVector randomLocation;


	//Re-occurrence time :
	float redoRandomMovement_Time = 5.0F;


	//Do once : 
	bool doOnce_RandomMove = false;
	bool doOnce_SeenTarget = true;
	bool doOnce_TriggerSpeed_True = false;
	bool doOnce_TriggerSpeed_False = false;

	bool doOnce_Roar = false;

	bool doOnce_AttackFromBack = false;

	//Health and Damage : 
	//float demonHealth = 50.0F; 


	//Timer
	float AccumulatedSeconds = 0.0F;
	float SeenTrigger = 0.6F;

	float unseenAccumulatedSeconds = 0.0F;
	float resumeRandomMovementTrigger = 5.0F;
}

// Sets default values
ADemon_Behaviour::ADemon_Behaviour()
{
	PrimaryActorTick.bCanEverTick = true;

	//Add pawn sensing to Demon : 
	PawnSensor = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	PawnSensor->SetPeripheralVisionAngle(DemonConstants::Demon_FOV);
	PawnSensor->SightRadius = DemonConstants::Demon_SightRadius;
}


// Called when the game starts or when spawned
void ADemon_Behaviour::BeginPlay()
{
	Super::BeginPlay();

	PawnSensor->OnSeePawn.AddDynamic(this, &ADemon_Behaviour::OnSeePawn); //When Demon sees something calls the OnSeePawn function.
}


// Called every frame
void ADemon_Behaviour::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (playerIsBeingSeen == false)
	{
		//UnseenReturnRandom(DeltaTime); //Too OP

		DemonVariables::doOnce_RandomMove ?  NOFUNCTION : RandomLocationGoTo();
		
		FlipFlop_slowDownTrigger_False_DoOnce();
		FlipFlop_slowDownTrigger_True_DoOnce();
	}
	if (playerIsBeingSeen == true)
	{
		SeenTarget();
	}

	//RetriggerableDelay(&ADemon_Behaviour::Reset_PlayerSeen, DeltaTime, DemonConstants::Demon_VisionReset);
}
void ADemon_Behaviour::UnseenReturnRandom(float deltaSecond)
{
	//Increments the time for each time the player is considered as 'unseen'
	//If the player is seen >SeenTarget(), it resets the accumulated time so far
	//Once the trigger is reached, re-establish the possibility of the Enemy to relocate randomly.


	DemonVariables::unseenAccumulatedSeconds += deltaSecond;

	if (DemonVariables::unseenAccumulatedSeconds >= DemonVariables::resumeRandomMovementTrigger)
	{
		DemonVariables::doOnce_RandomMove = false;
		EnemyReturnRandomLocation = true;

		DemonVariables::unseenAccumulatedSeconds -= DemonVariables::resumeRandomMovementTrigger;
	}
}

// Called to bind functionality to input
void ADemon_Behaviour::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//Pawn Sensor :
void ADemon_Behaviour::OnSeePawn(APawn* OtherPawn)
{
	EnemyReturnRandomLocation = false;


	SeenTarget();
}
void ADemon_Behaviour::SeenTarget()
{
	/*  
	* Once the player has been seen ;
	*	>Resets the accumulated time for the possibility to make the enemy re-wander around randomly
	*	>Flip Do once as to not recall this function every tick
	*	>Trigger 'Roar' anim and disable this character ability to move temporarily
	*	>Redirects to BP_DemonAI for blueprint event (SetAnimRoar())
	*	>Increase this character's speed and acceleration
	*	>Move this character toward the current location of the player
	*	>If this character is close enough, change the speed
	*/

	DemonVariables::unseenAccumulatedSeconds = 0;



	AMC_Behaviour* PlayerCharacter = CastToPlayerCharacter();

	playerIsBeingSeen = true;
	playerHasBeenSeen = true;

	DoTheRoar();

	GetCharacterMovement()->MaxWalkSpeed = DemonConstants::idle_MaxSpeed;
	GetCharacterMovement()->MaxAcceleration = DemonConstants::idle_MaxAcceleration;

	AI_MoveTo(VECTOR_NULL, PlayerCharacter, DemonVariables::acceptanceRadius);

	Timeline_SlowDown_Trigger();
}
void ADemon_Behaviour::Timeline_SlowDown_Trigger()
{
	if (attackSlowDownTrigger)
	{
		SpeedTimeline();
	}
	else
	{
		slowDownTrigger_False_Speed();
	}
}
void ADemon_Behaviour::DoTheRoar()
{
	//Check if the Demon has roared once
	
	if (DemonVariables::doOnce_Roar == false)
	{
		FlipFlop_Roar();

		this->GetCharacterMovement()->Deactivate();

		SetAnim_Roar(); //Redirects to BP_DemonAI.//
	}
}
void ADemon_Behaviour::Reset_PlayerSeen()
{
	FlipFlop_DoOnce_SeenTarget();
	playerIsBeingSeen = false;
}



//Damage and Experience :
float ADemon_Behaviour::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	playerIsBeingSeen = true;

	TArray<UBoxComponent*> causerComponents;

	DamageCauser->GetComponents(causerComponents);


	for (auto causerComponent : causerComponents)
	{
		if (causerComponent->GetName() == "SwordCollider")
		{
			currentHealth = currentHealth;

			currentHealth > 0 ? currentHealth -= Damage : NULL;
			currentHealth <= 0 ? GiveXP(DemonConstants::experienceGive) : NULL;
			return Damage;
		}
	}

	return Damage;
}
void ADemon_Behaviour::DoDamageToPlayeer()
{
	AMC_Behaviour* PlayerCharacter = CastToPlayerCharacter();

	if (::IsValid(PlayerCharacter))
	{
		PlayerCharacter->EnemyDamage.Broadcast(DemonConstants::demonDamage);
	}
}
void ADemon_Behaviour::GiveXP(float experienceGain)
{
	AMC_Behaviour* PlayerCharacter = CastToPlayerCharacter();

	for (int currentSequence = 1; currentSequence < 4; currentSequence++)
	{
		switch (currentSequence)
		{
		case 1: //Cast to player character.

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
			OnDeath();
			break;
		}
	}
}

//Slow down Triggers :
void ADemon_Behaviour::slowDownTrigger_True_Speed_Lerp(float lerpValue)
{
	if (DemonVariables::doOnce_TriggerSpeed_False == false)
	{
		FlipFlop_slowDownTrigger_False_DoOnce();

		GetCharacterMovement()->MaxWalkSpeed = UKismetMathLibrary::Lerp
		(
			DemonConstants::triggerSpeed_Maximum, //A
			DemonConstants::triggerSpeed_Initial, //B
			lerpValue
		);
	
	
		GetCharacterMovement()->MaxAcceleration = UKismetMathLibrary::Lerp
		(
			DemonConstants::triggerAccel_Maximum, //A
			DemonConstants::triggerAccel_Initial, //B
			lerpValue
		);

		FlipFlop_slowDownTrigger_True_DoOnce();
	}
}
void ADemon_Behaviour::slowDownTrigger_False_Speed()
{
	if (DemonVariables::doOnce_TriggerSpeed_True == false)
	{
		FlipFlop_slowDownTrigger_True_DoOnce();

		GetCharacterMovement()->MaxWalkSpeed = DemonConstants::triggerSpeed_Reset;
		GetCharacterMovement()->MaxAcceleration = DemonConstants::triggerAccel_Reset;

		FlipFlop_slowDownTrigger_False_DoOnce();
	}
}


//Flip flops : 
void ADemon_Behaviour::FlipFlop_DoOnce_RandomMove()
{
	DemonVariables::doOnce_RandomMove = !DemonVariables::doOnce_RandomMove;
}
void ADemon_Behaviour::FlipFlop_slowDownTrigger_True_DoOnce()
{
	DemonVariables::doOnce_TriggerSpeed_True = !DemonVariables::doOnce_TriggerSpeed_True;
}
void ADemon_Behaviour::FlipFlop_slowDownTrigger_False_DoOnce()
{
	DemonVariables::doOnce_TriggerSpeed_False = !DemonVariables::doOnce_TriggerSpeed_False;
}
void ADemon_Behaviour::FlipFlop_Roar()
{
	DemonVariables::doOnce_Roar = !DemonVariables::doOnce_Roar;
}
void ADemon_Behaviour::FlipFlop_DoOnce_SeenTarget()
{
	DemonVariables::doOnce_SeenTarget = !DemonVariables::doOnce_SeenTarget;
}
void ADemon_Behaviour::FlipFlop_DoOnce_AttackFromBack()
{
	DemonVariables::doOnce_AttackFromBack = !DemonVariables::doOnce_AttackFromBack;
}



//Functions to make universal for enemy class : 

//Move To AI (BP Function) :
void ADemon_Behaviour::RandomLocationGoTo()
{
	/*
	*	>Flip the doOnce switch so tick doesn't use it again
	*   >Adjust walking speed and acceleration
	*	>Get a random location in a certain radius
	*	>Move the Demon to new random location
	*	>Delegate for OnSuccess of Ai Move To
	*   >On Success, repeat this function so he moves to another place.
	*/

	FlipFlop_DoOnce_RandomMove();

	GetCharacterMovement()->MaxWalkSpeed = DemonConstants::idle_MaxSpeed;
	GetCharacterMovement()->MaxAcceleration = DemonConstants::idle_MaxAcceleration;

	GetRandomLocation();

	UAIAsyncTaskBlueprintProxy* unsignificantVariableName = nullptr;

	unsignificantVariableName = UAIBlueprintHelperLibrary::CreateMoveToProxyObject
	(
		this,
		this,
		DemonVariables::randomLocation,
		nullptr,
		DemonVariables::acceptanceRadius,
		false
	);

	//Create delegate for OnSuccess Function.//
	AiMoveTo_Success.BindUFunction(this, FName(TEXT("OnSuccessFunction")));
	unsignificantVariableName->OnSuccess.AddUnique(AiMoveTo_Success);
}
void ADemon_Behaviour::MoveTo_Success(EPathFollowingResult::Type unsignificantName)
{
	RandomLocationGoTo();
}
void ADemon_Behaviour::AI_MoveTo(const FVector& destination, AMC_Behaviour* PlayerCharacter, float acceptanceRadius)
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
void ADemon_Behaviour::GetRandomLocation()
{
	bool unsignificantVariableName = UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, this->GetActorLocation(), DemonVariables::randomLocation, DemonVariables::randomReachableRadius, ((ANavigationData*)nullptr), ((UClass*)nullptr));
}


//On Begin : 
void ADemon_Behaviour::VariableInitialize()
{
	using namespace DemonVariables;

	//AI Move To :
	acceptanceRadius = 150.0F;

	//Get Random Location : 
	randomReachableRadius = 3000.0F;
	randomLocation;

	//Re-occurrence time
	redoRandomMovement_Time = 5.0F;
	doOnce_RandomMove = false;

	doOnce_TriggerSpeed_True = false;
	doOnce_TriggerSpeed_False = false;

	doOnce_Roar = false;

	//Health and Damage : 
}




//Reusable :
AMC_Behaviour* ADemon_Behaviour::CastToPlayerCharacter()
{
	AMC_Behaviour* PlayerCharacter = Cast<AMC_Behaviour>(UGameplayStatics::GetPlayerCharacter(this, 0));

	return	PlayerCharacter;
}
void ADemon_Behaviour::RetriggerableDelay(FunctionToCall functionToCall, float deltaSeconds, float triggerThreshold)
{
	DemonVariables::AccumulatedSeconds += deltaSeconds;

	if (DemonVariables::AccumulatedSeconds >= triggerThreshold)
	{
		(this->*functionToCall)();

		DemonVariables::AccumulatedSeconds -= triggerThreshold;
	}
}