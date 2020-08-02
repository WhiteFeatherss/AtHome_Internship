#include "MC_Behaviour.h"
#include "GameFramework/Actor.h"
//Engine.//
#include "Engine.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
//Math.//
#include "Kismet/KismetMathLibrary.h"
#include "math.h"
#include "Math/Vector.h"
//Timer.//
//Timer.//
#include "TimerManager.h"
//Components.//
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"

//Widget.//
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"

//Self created CPP.//
#include "EnemyInterface.h"
#include "OutputDebug.h"


//Defines.//
#define TargetSystem_InterfaceName UEnemyInterface
#define EMPTY 0
#define NOFUNCTION void()

//Times 
#define FROZEN_TIME 0
#define NORMAL_TIME 1

//Saturation
#define MONOCHROME FVector4 (0.0F, 0.0F, 0.0F, 1.0F)
#define POLYCHROME FVector4 (1.0F, 1.0F, 1.0F, 1.0F)
#define ENABLE 1
#define DISABLE 0


//Name spaces.//
namespace PlayerStatsConstants
{
	//Strength "
	constexpr float oneTen_SwordMultiplier = 4;
	constexpr float tenTwenty_SwordMultiplier = 3;
	constexpr float twentyThirty_SwordMultiplier = 2;
	constexpr float fortyFifty_SwordMultiplier = 0.5;

	constexpr float oneTen_SwordIncrementer = -4.0F;
	constexpr float tenTwenty_SwordIncrementer = 6.0F;
	constexpr float twentyThirty_SwordIncrementer = 26.0F;
	constexpr float thirtyForty_SwordIncrementer = 56.0F;
	constexpr float fortyFifty_SwordIncrementer = 76.0F;


	//Vitality :

	constexpr float baseHealth = 750.0F;

	constexpr float oneTen_Vitality_Multiplier = 10;
	constexpr float tenTwenty_Vitality_Multiplier = 9;
	constexpr float twentyThirty_Vitality_Multiplier = 8;
	constexpr float thirtyFourty_Vitality_Multipier = 7;
	constexpr float fortyFifty_Vitality_Multiplier = 5;

	constexpr float oneTen_Vitality_Incrementer = -10.0F;
	constexpr float tenTwenty_Vitality_Incrementer = 0.0F;
	constexpr float twentyThirty_Vitality_Incrementer = 20.0F;
	constexpr float thirtyForty_Vitality_Incrementer = 57.0F;
	constexpr float fortyFifty_Vitality_Incrementer = 185.0F;

	//Defense : 
	constexpr float defenseBaseMult = 0.15F;
	constexpr float defenseBaseIncrementer = 0.2F;


	//Experience : 
	constexpr float experienceExponent = 1.4F;
	constexpr float baseExperience = 90.0F;
	constexpr float maximum_Stat = 50.0F;

	//Stats :
	constexpr int statsGain_LevelUP = 5;
	constexpr int totalAmountStatBeginning = 4;

	//Debuff
	constexpr int NumberOfStatsToRemove = 2;

}
namespace PlayerConstants //Global constants reserved for MC_Behaviour.cpp//
{
	//Walking / Running : 
	constexpr float backward = -1.0F;
	constexpr float default_WalkingSpeed = 600.0F;
	constexpr float running_WalkSpeed = 900.0F;
	constexpr float isGoingBackwardMultiplier = 0.6F;


	//Teleport :
	constexpr float teleport_Multiplier = 1.0F;
	constexpr float default_Camera_FOV = 60.0F;
	constexpr float camera_FOV_Objective = 0.0F;
	constexpr int FOV_Decrement_Speed = 5;


	//Attack : 
	constexpr float forwardBackward_P_Trigger = 5.0F;
	constexpr float forwardBackward_N_Trigger = -5.0F;

	constexpr float base_SwordDamage = 50.0F;

	constexpr float blockBuff = 4.0F;
	
	//Lock On : 
	constexpr float lockOn_Sphere_Size = 5000.0F;


	//TimeDilation : 
	constexpr float timeFrozen = 0.0F;
	constexpr float timeNormal = 1.0F;

	//Health Regen : 
	constexpr float healthGainRegen = 0.12F;
	constexpr float poisonDamage = 0.5F;
	constexpr float poisonTime = 4.0F;


}
namespace PlayerVariables //Variables reserved for MC_Behaviour.cpp.//
{
	//This :
	APlayerController* thisController;
	FPostProcessSettings PostProData;

	//Lock On :
	TArray<AActor*> Enemy_Array;
	AActor* closestActor = nullptr;
	UBoxComponent* swordBoxComponent;

	//Movement :
	bool isGoingBackward = false;
	

	//Attack :
	bool attackDoOnce = false;
	bool attackHasHit = false;


	//Lock On :
	bool isTargetLocked = false;


	//Level up and Experience :
	float experienceNeeded = 0;
	float levelUpExperience = 0;
	float currentExperience = 0.0F;


	//Health : 
	float maxPlayerHealth = 750.0F;
	float currentPlayerHealth = 750.0F;


	//Stats :
	float killCount = 0;
	int availableStats = 0;


	int spirit_Points = 1;
	int strength_Points = 1;
	int defense_Points = 1;
	int vitality_Points = 1;
	
	//Sword Damage :
	float swordDamage = 10.0F;

	//HUD :
	bool isStatdHUD_Open = false;


	//Teleport : 
	bool isTeleportAllowed = false;

	//Poison : 
	float poisonAccumulated_Time = 0.0F;

	//
	float defenseMultiplier = 1.0F;
}


//Constructor
AMC_Behaviour::AMC_Behaviour()
{
	//Constant Variables.//
	
		//Controller
	BaseTurnRate = 100.0F;
	BaseLookUpRate = 100.0F;


		//Camera
	FirstPersonCam = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera")); //SetfieldofView will not work without this.
	FirstPersonCam->SetupAttachment(GetCapsuleComponent());
	FirstPersonCam->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); 
	FirstPersonCam->bUsePawnControlRotation = true;
	FirstPersonCam->SetFieldOfView(PlayerConstants::default_Camera_FOV); //Define the field of view at the start.//
		

		//Other
	PrimaryActorTick.bCanEverTick = true;
}

//Called when the game starts or when spawned
void AMC_Behaviour::BeginPlay()
{
	Super::BeginPlay();

	//Sword Damage :
	UpdateSwordDamage();

	//Experience :
	Experience_NextLeveL_Update();
	PlayerVariables::experienceNeeded = PlayerVariables::levelUpExperience - PlayerVariables::currentExperience;
	PlayerVariables::availableStats = 0;

	//Black and white screen : 
	PlayerVariables::PostProData.bOverride_ColorSaturation = true;

	//Assign Collide from BP to CPP
	PlayerVariables::swordBoxComponent = GetBoxComponentByName("SwordCollider");


	//Listener on component overlap > Do damage.
	PlayerVariables::swordBoxComponent ? PlayerVariables::swordBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AMC_Behaviour::SwordSwung) : NOFUNCTION;


	//Delegate : 
	EnemyDeath.AddUObject(this, &AMC_Behaviour::enemyKilled); //When an enemy dies.//

	EnemyDamage.AddUObject(this, &AMC_Behaviour::TakeDamage); //When the enemy does damage to the player.// 
}

//Called to bind functionality to input
void AMC_Behaviour::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
	//Movements
	PlayerInputComponent->BindAxis("MoveForward", this, &AMC_Behaviour::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMC_Behaviour::MoveRight);

	
	//Jump.//
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMC_Behaviour::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMC_Behaviour::StopJumping);
	

	//Run.//
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AMC_Behaviour::RunSet);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AMC_Behaviour::RunSet);

	
	//Dodge.//
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &AMC_Behaviour::Dodge);
	

	//Sheath and unsheathe.//
	PlayerInputComponent->BindAction("Sheath", IE_Pressed, this, & AMC_Behaviour::SetSheath);

	
	//Blocking.//
	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &AMC_Behaviour::SetBlock);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &AMC_Behaviour::SetBlock);
	

	//Attacks.//
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AMC_Behaviour::AttackPressed);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &AMC_Behaviour::AttackReleased);
		
	
	//Lock On.//
	PlayerInputComponent->BindAction("LockOn", IE_Pressed, this, &AMC_Behaviour::LockOnPressed);

	//OpenStats :
	//PlayerInputComponent->BindAction("OpenCloseStats", IE_Pressed, this, &AMC_Behaviour::OpenClose_StatHUD);

	//Camera Bindings :
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMC_Behaviour::TurnAtRate); //>In case we use a controller.
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMC_Behaviour::LookUpAtRate); //>In case we use a controller.


	//Test input.//
	PlayerInputComponent->BindAction("Test", IE_Pressed, this, &AMC_Behaviour::Test);
}

//Player input functions.//
	

	//Movements :
void AMC_Behaviour::MoveForward(float Value)
{
	FVector playerVector = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	
	SetGoingBackward(Value); //Set the boolean value of whether character is moving forward or backward.//
	
	isRunning ? GetCharacterMovement()->MaxWalkSpeed = PlayerConstants::running_WalkSpeed : GetCharacterMovement()->MaxWalkSpeed = PlayerConstants::default_WalkingSpeed; //Increase/decrease running speed.//

	PlayerVariables::isGoingBackward ? AddMovementInput(playerVector, Value * PlayerConstants::isGoingBackwardMultiplier) : AddMovementInput(playerVector, Value);//Adjust the walking/Running speed when going backward. Walking backward is slower.//
}
void AMC_Behaviour::MoveRight(float Value)
{
	FVector playerVector = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
		
	AddMovementInput(playerVector, Value);

}
void AMC_Behaviour::SetGoingBackward(float Value) //If character's forward value is -1.0F, sets the boolean value to true;
{
	Value == PlayerConstants::backward ? PlayerVariables::isGoingBackward = true : PlayerVariables::isGoingBackward = false;
}


	//Running inputs :
void AMC_Behaviour::RunSet()
{
	isRunning = !isRunning;
}

 
	//Dodge : 
void AMC_Behaviour::Dodge() //Teleport the player to a different location, according to the player's velocity.//
{
	//Trace by Channel Variables.//
	FVector start = GetActorLocation(); //Where the trace starts.//
	FVector end =  (start + GetCharacterMovement()->Velocity * PlayerConstants::teleport_Multiplier); //Where the trace ends.//
	
	FHitResult outHit; //Where the trace by channel will hit something.
	FCollisionQueryParams params;
	

	//FCollisionQueryParams - Parameters.//
	params.AddIgnoredActor(this); //Ignore the Main Character for the trace by channel.//
	

	bool somethingInTheWay = GetWorld()->LineTraceSingleByChannel
	(
		outHit,
		start,
		end,
		ECC_Camera,
		params
	);

	PlayerVariables::isTeleportAllowed ? Teleport(somethingInTheWay, outHit, start, end) : NOFUNCTION;
}


	//Teleport :
void AMC_Behaviour::Teleport(bool somethingInTheWay, FHitResult& outHit, FVector& start, FVector& end) 
//Teleport the MainCharacter in the direction he is going, can either teleport at the maximum distance or teleport where he hits and object. Prevents merging with objects of glitching through.//
{
	PlayerVariables::isTeleportAllowed = false;
	AccumulatedSeconds = 0.0F;
	SetFOV();

	//Look out if the player will hit an object, if it hits something.
	//Otherwise, keep on going maximum teleportation distance.

	somethingInTheWay ? this->TeleportTo(outHit.Location, GetActorRotation()) : this->TeleportTo(end, GetActorRotation());
}

void AMC_Behaviour::RetriggerableDelay(FunctionToCall functionToCall, float deltaSeconds, float triggerThreshold)
{
	AccumulatedSeconds += deltaSeconds;

	//AccumulatedSeconds > triggerThreshold ? AccumulatedSeconds == triggerThreshold : NOFUNCTION;
	

	if (AccumulatedSeconds >= triggerThreshold)
	{
		AccumulatedSeconds = triggerThreshold;
		(this->*functionToCall)();
	}
}

void AMC_Behaviour::RetriggerableDelayRegen(FunctionToCall functionToCall, float deltaSeconds, float triggerThreshold)
{
	AccumulatedSecondsRegen += deltaSeconds;

	AccumulatedSeconds >= triggerThreshold ? AccumulatedSeconds == triggerThreshold : NOFUNCTION;


	if (AccumulatedSecondsRegen >= triggerThreshold)
	{
		AccumulatedSecondsRegen = triggerThreshold;
		(this->*functionToCall)();
	}
}
void AMC_Behaviour::ResetTeleport()
{	
	PlayerVariables::isTeleportAllowed = true;
}


	//Sheathing : 
void AMC_Behaviour::SetSheath() //simple Bool switch for the isSheathed.//
{
	isSheathed = !isSheathed;
}


	//Blocking : 
void AMC_Behaviour::SetBlock() //simple Bool switch for the isBlocking.//
{
	bool_IsBlocking = !bool_IsBlocking;
}


	//Jumping :
void AMC_Behaviour::Jump()
{
	isInAir = true;
	JumpKeyHoldTime = 0.0f;
}
void AMC_Behaviour::StopJumping()
{
	isInAir = false;
	ACharacter::ResetJumpState();
}


	//Lock-On System :
void AMC_Behaviour::LockOnPressed() 
//When key is pressed, sphere sweep for actors, reduces the array to specific actors that contains an interface
//get the closest actor to the MC among all other actors and lock unto that closest actor.//
{
	//Local variables.//
	TArray<FHitResult> OutHits_Array; //Initialize new array.//
	FVector actorLocation = GetActorLocation(); //Get the location of the the MC.
	FCollisionShape collisionSphere = FCollisionShape::MakeSphere(PlayerConstants::lockOn_Sphere_Size);
	PlayerVariables::closestActor = nullptr;

	bool isHit = GetWorld()->SweepMultiByChannel
	(
		OutHits_Array,
		actorLocation, 
		actorLocation,	
		FQuat::Identity,
		ECC_Pawn,
		collisionSphere
	);

	AddSweepedEnemies(isHit, OutHits_Array, PlayerVariables::Enemy_Array); 
	//Add every actor that has the correct interface to the array. *As to not target NPCs.//
	PlayerVariables::closestActor =  GetClosestActor(actorLocation, PlayerVariables::closestActor); 
	//Get the closest Actor within the sphere sweep of the main character.//
	
	
	if (PlayerVariables::isTargetLocked)
	{
		PlayerVariables::isTargetLocked = false;
	}
	else
	{
		if (PlayerVariables::closestActor != nullptr)
		{
			PlayerVariables::isTargetLocked = true;
		}
	}
}
void AMC_Behaviour::AddSweepedEnemies(bool isHit, TArray<FHitResult>& outHits_Array, TArray<AActor*>& Actor_Array) 
//Add each actors that contains a specific interface during the sphere sweep to an array.// 
{
	Actor_Array.Empty(); 
	//Empties the array at each sweep, as to not add to many actors but also prevent an actor from being in the array twice.//
	
	if (isHit)
	{
		for (auto& Hit : outHits_Array)
		{
			//Check if the actor hit has the correct interface, then add the hit actor to the array. Otherwise, do nothing.//
			Hit.Actor->GetClass()->ImplementsInterface(TargetSystem_InterfaceName::StaticClass()) ? Actor_Array.Add(Hit.GetActor()) : NOFUNCTION;
		}
	}
}
AActor* AMC_Behaviour::GetClosestActor(FVector& actorLocation, AActor* closestActor) 
//Finds the closest actor within the array of actor that was created during the sweep, each of them has an Interface to differentiate them.//
{
	//Variables.//
	float closestDistance = PlayerConstants::lockOn_Sphere_Size; //Maximum distance where the actor can be anyways.//

	if (PlayerVariables::Enemy_Array.Num() <= EMPTY)
	{
		return closestActor = nullptr;
	}
	else
	{
		for (auto& Enemy : PlayerVariables::Enemy_Array)
		{
			float enemyDistance = FVector::Dist(actorLocation, Enemy->GetActorLocation()); //Returns distance between two points as a float.//

			if (enemyDistance < closestDistance)
			{
				//Update the closest actor and its distance if one is found closer than the others.//
				closestActor = Enemy;
				closestDistance = enemyDistance; 
			}
		}

		return closestActor;
	}
}
void AMC_Behaviour::TargetLock(AActor* closestEnemy) //Locks unto the closest actor.//
{
	if (PlayerVariables::Enemy_Array.Num() <= 0)
	{
		return;
	}
	else
	{
		GetController()->SetControlRotation(MC_NewRotation(closestEnemy));
	}
}
FRotator AMC_Behaviour::MC_NewRotation(AActor* closestEnemy)
{
	//Look at.//
	FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), closestEnemy->GetActorLocation());

	//New Interp To.//
	FRotator newInterpTo = UKismetMathLibrary::RInterpTo(GetControlRotation(), lookAtRotation, GetWorld()->DeltaTimeSeconds, 0);

	return UKismetMathLibrary::MakeRotator(GetControlRotation().Roll, newInterpTo.Pitch, newInterpTo.Yaw);;
}


	//Attacks :
void AMC_Behaviour::AttackPressed() //Check if the weapon is currently sheathed, if it is, unsheathe, else start to attack.//
{
		isSheathed ? SetSheath() : AttackDoOnce_Func();
}
void AMC_Behaviour::AttackReleased() //Resets all the variables to allow the player to attack again.//
{
	//Variables to reset.//
	isAttacking = false;

	attackingForward = false;
	attackingBackward = false;

	attackingLeft = false;
	attackingRight = false;

	PlayerVariables::attackDoOnce = false; //Reset the doOnce.//
}

void AMC_Behaviour::AttackDoOnce_Func() //Determines which attack will be performed based on the Main Character's velocity and perform that said attack.//
{
	//Get the value of the MC's directions.//
	float MoveRight_Value = FVector::DotProduct(GetVelocity(), GetActorRightVector()); 
	float MoveForward_Value = FVector::DotProduct(GetVelocity(), GetActorForwardVector());

	if (PlayerVariables::attackDoOnce == false) //DoOnce equivalent in Blueprint, prevents from attacking twice.//
	{
		PlayerVariables::attackDoOnce = true;
		isAttacking = true;

		if ((MoveRight_Value >= PlayerConstants::forwardBackward_N_Trigger) && (MoveRight_Value <= PlayerConstants::forwardBackward_P_Trigger))
		{
			ForwardBackward_Attack_Setter(MoveForward_Value);
		}
		else
		{
			LeftRight_Attack_Setter(MoveRight_Value, MoveForward_Value);
		}
	}
}

void AMC_Behaviour::LeftRight_Attack_Setter(float MoveRight_Value, float MoveForward_Value)
//Depending on the direction of which the player is moving, starts the animTree differently Left / Right / Thrust / Overhead attacks and so on.//
{
	MoveRight_Value < 0 ? attackingLeft = true : attackingRight = true;
}

void AMC_Behaviour::ForwardBackward_Attack_Setter(float MoveForward_Value)
//Depending on the direction of which the player is moving, starts the animTree differently Left / Right / Thrust / Overhead attacks and so on.//
{
	MoveForward_Value < 0 ? attackingBackward = true : attackingForward = true;
}

void AMC_Behaviour::SwordSwung(UPrimitiveComponent* hitComponent, AActor* enemyActor, UPrimitiveComponent* otherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& hit)
{

	if ((enemyActor->GetClass()->ImplementsInterface(TargetSystem_InterfaceName::StaticClass())))
	{
		PlayerVariables::swordBoxComponent->SetGenerateOverlapEvents(false);
		UGameplayStatics::ApplyDamage(enemyActor, PlayerVariables::swordDamage, nullptr, this, UDamageType::StaticClass());
	}
}

	//On enemy killed :
void AMC_Behaviour::enemyKilled(float experienceGain)
{
	//Variables :
	int previousLevel = characterLevel;


	PlayerVariables::currentExperience += experienceGain; //Increases current experience upon killing an enemy.
	PlayerVariables::killCount++;

	/*
	* As long as the current experience is higher or equal as the necessary amount of experience to level up,
	* increase the level.
	* Reduce the current Experience minus the previous amount of necessary experience to level up.
	* Update the new experience proper to the player's level.
	* Lastly, update the amount of experience needed to get to the next level.
	*/

	while (PlayerVariables::levelUpExperience < PlayerVariables::currentExperience)
	{ 
		characterLevel++;
		LevelUp();

		PlayerVariables::currentExperience -= PlayerVariables::levelUpExperience;

		if (previousLevel != characterLevel)
		{
			PlayerVariables::availableStats += PlayerStatsConstants::statsGain_LevelUP;
			Experience_NextLeveL_Update();
		}
	}

	PlayerVariables::experienceNeeded = PlayerVariables::levelUpExperience - PlayerVariables::currentExperience;
}
void AMC_Behaviour::Experience_NextLeveL_Update()
{
	/* levelUpExperience follows an exponential function */
	PlayerVariables::levelUpExperience = FGenericPlatformMath::RoundToFloat
	(
		FGenericPlatformMath::Pow
		(
			(PlayerStatsConstants::baseExperience * characterLevel),

			PlayerStatsConstants::experienceExponent
		)
	);
}


float AMC_Behaviour::Get_CurrentRemainingPoint()
{
	return PlayerVariables::availableStats;
}

//Stats :
void AMC_Behaviour::Set_AvailableStats() //Decrease the available points for stats upon clicking button.//
{
	if (PlayerVariables::availableStats > 0)
	{
		PlayerVariables::availableStats--;
	}
}
void AMC_Behaviour::Set_StatsPoints(StatsNames statName)
{
	if (PlayerVariables::availableStats > 0)
	{
		switch (statName)
		{

		case SN_Vitality:

			if (PlayerVariables::vitality_Points < PlayerStatsConstants::maximum_Stat)
			{
				Set_AvailableStats();
				PlayerVariables::vitality_Points++;
				UpdateVitality();
			}

			break;

		case SN_Spirit:

			if (PlayerVariables::spirit_Points < PlayerStatsConstants::maximum_Stat)
			{
				Set_AvailableStats();
				PlayerVariables::spirit_Points++;
			}

			break;

		case SN_Defense:

			if (PlayerVariables::defense_Points < PlayerStatsConstants::maximum_Stat)
			{
				Set_AvailableStats();

				PlayerVariables::defense_Points++;

				Update_Stats_Affected(SN_Defense, 0 ,0);
				
			}
			
			break;

		case SN_Strength:

			if (PlayerVariables::strength_Points < PlayerStatsConstants::maximum_Stat)
			{
				Set_AvailableStats();

				PlayerVariables::strength_Points++;

				UpdateSwordDamage();
			}

			break;
		}
	}
}
void AMC_Behaviour::Update_Stats_Affected(StatsNames statsnames, float multiplier, float incrementer)
{
	switch (statsnames)
	{
	case SN_Defense:

		multiplier = PlayerStatsConstants::defenseBaseMult + 
			(PlayerVariables::defense_Points * PlayerStatsConstants::defenseBaseIncrementer) - PlayerStatsConstants::defenseBaseIncrementer;
		
		incrementer = (1 + multiplier);
		

		PlayerVariables::defenseMultiplier = FGenericPlatformMath::LogX(10, incrementer) + 1;
	
		break;

	case SN_Strength:
		PlayerVariables::swordDamage = PlayerConstants::base_SwordDamage + ((multiplier * PlayerVariables::strength_Points) + incrementer);
		break;

	case SN_Spirit:
		break;

	case SN_Vitality:
		PlayerVariables::maxPlayerHealth = PlayerStatsConstants::baseHealth + ((multiplier * PlayerVariables::vitality_Points) + incrementer);
		break;
	}
}


	//Update sword Damage :
void AMC_Behaviour::UpdateSwordDamage()
{
	/*
	* Updates the damage the sword does given 5 different simple linear equations.
	* 01 - 10 = 4 * (stat level) + (-4) damage
	* 11 - 20 = 3 * (stat level) + 6 damage
	* 21 - 30 = 2 * (stat level) + 26 damage
	* 31 - 40 = 1 * (stat level) + 56 damage
	* 41 - 50 = 0.5 * (stat level) + 76 damage
	* Creates a soft and hard cap
	*/

	//1 to 10 points
	if (PlayerVariables::strength_Points <= 10)
	{
		Update_Stats_Affected(SN_Strength, PlayerStatsConstants::oneTen_SwordMultiplier, PlayerStatsConstants::oneTen_SwordIncrementer);
	}
	//11 to 20 points
	else if ((PlayerVariables::strength_Points > 10) && (PlayerVariables::strength_Points <= 20))
	{
		Update_Stats_Affected(SN_Strength, PlayerStatsConstants::tenTwenty_SwordMultiplier, PlayerStatsConstants::tenTwenty_SwordIncrementer);
	}
	//21 to 30 points
	else if ((PlayerVariables::strength_Points > 20) && (PlayerVariables::strength_Points <= 30))
	{
		Update_Stats_Affected(SN_Strength, PlayerStatsConstants::twentyThirty_SwordMultiplier, PlayerStatsConstants::twentyThirty_SwordIncrementer);
	}
	//31 to 40 points
	else if ((PlayerVariables::strength_Points > 30) && (PlayerVariables::strength_Points <= 40))
	{
		Update_Stats_Affected(SN_Strength, 1, PlayerStatsConstants::thirtyForty_SwordIncrementer);
	}
	//41 to 50 points
	else
	{
		Update_Stats_Affected(SN_Strength, PlayerStatsConstants::fortyFifty_SwordMultiplier, PlayerStatsConstants::fortyFifty_SwordIncrementer);
	}
}


	//Update maximum health : 
void AMC_Behaviour::UpdateVitality()
{
	//1 to 10 points
	if (PlayerVariables::vitality_Points <= 10)
	{
		Update_Stats_Affected(SN_Vitality, PlayerStatsConstants::oneTen_Vitality_Multiplier, PlayerStatsConstants::oneTen_Vitality_Incrementer);
	}
	//11 to 20 points
	else if ((PlayerVariables::vitality_Points > 10) && (PlayerVariables::vitality_Points <= 20))
	{
		Update_Stats_Affected(SN_Vitality, PlayerStatsConstants::tenTwenty_Vitality_Multiplier, PlayerStatsConstants::tenTwenty_Vitality_Incrementer);
	}
	//21 to 30 points
	else if ((PlayerVariables::vitality_Points > 20) && (PlayerVariables::vitality_Points <= 30))
	{
		Update_Stats_Affected(SN_Vitality, PlayerStatsConstants::twentyThirty_Vitality_Multiplier, PlayerStatsConstants::twentyThirty_Vitality_Incrementer);
	}
	//31 to 40 points
	else if ((PlayerVariables::vitality_Points > 30) && (PlayerVariables::vitality_Points <= 40))
	{
		Update_Stats_Affected(SN_Vitality, PlayerStatsConstants::thirtyFourty_Vitality_Multipier, PlayerStatsConstants::thirtyForty_Vitality_Incrementer);
	}
	//41 to 50 points
	else
	{
		Update_Stats_Affected(SN_Vitality, PlayerStatsConstants::fortyFifty_Vitality_Multiplier, PlayerStatsConstants::fortyFifty_Vitality_Incrementer);
	}
}



	//Damage related :
void AMC_Behaviour::TakeDamage(float damageInput)
{	
	bool_IsBlocking ? 
		PlayerVariables::currentPlayerHealth -= ((damageInput / PlayerVariables::defenseMultiplier) / PlayerConstants::blockBuff) :
		PlayerVariables::currentPlayerHealth -= (damageInput / PlayerVariables::defenseMultiplier);

	PlayerGetHit();

	AccumulatedSecondsRegen = 0.0F;

	isRegenerating = false;

	if (PlayerVariables::currentPlayerHealth <= 0)
	{
		PlayerVariables::currentPlayerHealth = 0; //If the value of the player's health is lower than 0, sets it back to 0 automatically. 
		
		OnDeath();
	}
}
void AMC_Behaviour::PoisonDamageTick(float deltaTime)
{
	PlayerVariables::poisonAccumulated_Time += deltaTime;

	if (isPoisoned == true)
	{
		AccumulatedSecondsRegen = 0.0F;
		isRegenerating = false;

		PlayerVariables::currentPlayerHealth -= PlayerConstants::poisonDamage;

		PlayerVariables::poisonAccumulated_Time >= PlayerConstants::poisonTime ? isPoisoned = false : NOFUNCTION;
	}
	else
	{
		PlayerVariables::poisonAccumulated_Time = 0;
	}
}
void AMC_Behaviour::RegenHealth()
{
	if (PlayerVariables::currentPlayerHealth < PlayerVariables::maxPlayerHealth)
	{
		isRegenerating = true;
		PlayerVariables::currentPlayerHealth += PlayerConstants::healthGainRegen;
	}
	else
	{
		isRegenerating = false;
	}
	
}


	//Player's death related functions :
void AMC_Behaviour::OnDeath()
{
	//Turn everything to monochrome.
	FVector4 monochrome = MONOCHROME;
	SetCameraSaturation(monochrome);

	//Disable player's input so he can't look around.
	EnableDisable_Input(DISABLE);

	//Blueprint Callable Event > On Death add Widget to viewport :
	PlayerDeath();
	
	//Freeze time
	Set_TimeDilation(FROZEN_TIME);

	//If the player's level if higher than one, reduces player's level by one.
	characterLevel > 1 ? characterLevel-- : NOFUNCTION;

	//Reduces EXP back to 0 and update level experience to the new player's level.
	PlayerVariables::currentExperience = 0;
	Experience_NextLeveL_Update();

	//Increase player's health back to maximum.
	PlayerVariables::currentPlayerHealth += PlayerVariables::maxPlayerHealth;

	//Remove X number of random stats
	RemoveRandomStat_Prequel();
}
void AMC_Behaviour::BackgroundEvent()
{
	/*
	*	Once the Death Widget animation is over ;
	*	resume time to normal,
	*	Re-activate player controller,
	*	then fade the saturation back to polychromatic colors over X amount of seconds (BP_MyMC_Behaviour)
	*/

	// * Timelines do not work when time dilation is frozen, therefore needs to have normal dilation.


	Set_TimeDilation(NORMAL_TIME);

	EnableDisable_Input(ENABLE);

	DeathWidgetAnimationDone();
}
void AMC_Behaviour::RemoveRandomStat_Prequel()
{
	//Check the amount of stats the player has attributed to his class. 
	//(Total amount of points - 4 points since each stats begins with 1.)
	int totalAmount_StatPoints = 
		(
			PlayerVariables::defense_Points + 
			PlayerVariables::strength_Points +
			PlayerVariables::spirit_Points +
			PlayerVariables::vitality_Points +
			PlayerVariables::availableStats -
			PlayerStatsConstants::totalAmountStatBeginning
		);

	if (totalAmount_StatPoints == 0)
	{
		NOFUNCTION;
	}
	else if (totalAmount_StatPoints < PlayerStatsConstants::NumberOfStatsToRemove)
	{
		RemoveRandomStat(totalAmount_StatPoints);
	}
	else
	{
		RemoveRandomStat(PlayerStatsConstants::NumberOfStatsToRemove);
	}

	//Do not need to take available stats into account, the decrement of the state in the next function 
	//takes care of it automatically if all the stats are still at level 1.
}
void AMC_Behaviour::RemoveRandomStat(float NumberToRemove)
{
	//Remove X amount of stats from the player's attributed points or freeloading points
	//If the stat that's pending to be removed is equal to 1, increases the iteration of the FOR LOOP.
	//Since the amount to be removed cannot be removed, gives the randomizer another chance to remove a stat that can be removed.

	for (int state = 0; state < NumberToRemove; state++)
	{
		int statToRemove = FMath::RandRange(0, 4);

		switch (statToRemove)
		{
		case 0:

			PlayerVariables::defense_Points > 1 ? PlayerVariables::defense_Points-- : state--;


			break;

		case 1:

			PlayerVariables::strength_Points > 1 ? PlayerVariables::strength_Points-- : state--;

			break;

		case 2:

			PlayerVariables::spirit_Points > 1 ? PlayerVariables::spirit_Points-- : state--;

			break;

		case 3:

			PlayerVariables::vitality_Points > 1 ? PlayerVariables::vitality_Points-- : state--;

			break;

		case 4:

			PlayerVariables::availableStats > 0 ? PlayerVariables::availableStats-- : state--;

			break;
		}
	}
}




void AMC_Behaviour::Test() //T key, performs test actions.//
{
	//PlayerVariables::currentPlayerHealth -= 100.0F;
	//OutputDebug::DebugFStringFloat(PlayerVariables::currentPlayerHealth, "Current helth : ");
}


//Get Box Components from blueprint :
UBoxComponent* AMC_Behaviour::GetBoxComponentByName(FString ComponentVariableName)
{
	TArray<UBoxComponent*>	boxComponents;
	GetComponents(boxComponents);

	for (auto boxComponent : boxComponents)
	{
		if (boxComponent->GetName() == ComponentVariableName)
		{
			return boxComponent;
		}
	}

	return nullptr;
}


//Time functions : 
void AMC_Behaviour::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerVariables::isTargetLocked)
	{
		TargetLock(PlayerVariables::closestActor);
	}

	PoisonDamageTick(DeltaTime);

	RetriggerableDelay(&AMC_Behaviour::ResetTeleport, DeltaTime, teleportTrigger);
	RetriggerableDelayRegen(&AMC_Behaviour::RegenHealth, DeltaTime, regenTrigger);
}
void AMC_Behaviour::Set_TimeDilation(float inputValue)
{
	AWorldSettings* worldSettings = GetWorldSettings();

	worldSettings->SetTimeDilation(inputValue);
}



//Camera functions : 
void AMC_Behaviour::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}
void AMC_Behaviour::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMC_Behaviour::SetCameraSaturation(FVector4 NewColorSaturation)
{
	//Change the saturation of the player's camera. 
	PlayerVariables::PostProData.ColorSaturation = NewColorSaturation;
	FirstPersonCam->PostProcessSettings = PlayerVariables::PostProData;
}


void AMC_Behaviour::setSwordCollision()
{
	PlayerVariables::swordBoxComponent->SetGenerateOverlapEvents(true);
}

//Re-usable character functions :
void AMC_Behaviour::EnableDisable_Input(int enableORDisable)//Enables or disables on the character controller
{
	switch (enableORDisable)
	{
	case 0:
		DisableInput(Cast<APlayerController>(this));
		break;
	case 1:
		EnableInput(Cast<APlayerController>(this));
		break;
	}
}
void AMC_Behaviour::Set_Inputmode(InputMode inputMode)
{
	PlayerVariables::thisController = UGameplayStatics::GetPlayerController(this, 0);

	if (PlayerVariables::thisController != nullptr)
	{
		switch (inputMode)
		{
		case IM_UI_Only:
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerVariables::thisController);
			break;

		case IM_GameModeOnly:
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerVariables::thisController);
			break;
		}
	}
}
void AMC_Behaviour::Set_ShowCursor(bool inputValue)
{
	PlayerVariables::thisController = UGameplayStatics::GetPlayerController(this, 0);

	if (PlayerVariables::thisController != nullptr)
	{
		PlayerVariables::thisController->bShowMouseCursor = inputValue;
		PlayerVariables::thisController->bEnableClickEvents = inputValue;
		PlayerVariables::thisController->bEnableMouseOverEvents = inputValue;
	}
}


//Getter stat :
int AMC_Behaviour::Get_CurrentAvailableStats()
{
	return PlayerVariables::availableStats;
}
int AMC_Behaviour::Get_Stats(StatsNames statName)
{
	switch (statName)
	{

	case SN_Vitality:
		return PlayerVariables::vitality_Points;
		break;

	case SN_Spirit:
		return PlayerVariables::spirit_Points;
		break;

	case SN_Defense:
		return PlayerVariables::defense_Points;
		break;

	case SN_Strength:
		return PlayerVariables::strength_Points;
		break;
	}

	return NULL;
}


//Getter experience :
float AMC_Behaviour::Get_Current_Experience()
{
	return PlayerVariables::currentExperience;
}
float AMC_Behaviour::Get_ExperienceNeeded() //Return the necessary experience to level up all while being at the lowest scope possible.
{
	return PlayerVariables::levelUpExperience;
}

//Health related :
float AMC_Behaviour::Get_MaxHealth()
{
	return PlayerVariables::maxPlayerHealth;
}
float AMC_Behaviour::Get_CurrentHealth()
{
	return PlayerVariables::currentPlayerHealth;
	
}


//Widget related : 
void AMC_Behaviour::OpenClose_StatHUD()
{
	PlayerVariables::isStatdHUD_Open = !PlayerVariables::isStatdHUD_Open; //Flipflop

	if (PlayerVariables::isStatdHUD_Open == true)
	{
		//Open it
		Set_ShowCursor(true);
		Set_Inputmode(IM_UI_Only);
		Set_TimeDilation(PlayerConstants::timeFrozen);
	}
	else
	{
		//Close it
		Set_ShowCursor(false);
		Set_Inputmode(IM_GameModeOnly);
		Set_TimeDilation(PlayerConstants::timeNormal);
	}
}

//Timer
//GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AMC_Behaviour::FUNCTION DESIRED, 0.5F, false, 1.0F);
