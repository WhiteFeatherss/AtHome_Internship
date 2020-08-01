	#pragma once

#include "Engine.h"
#include "CoreMinimal.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "GameFramework/WorldSettings.h"

#include "MC_Behaviour.generated.h"//Always at the bottom, don't touch.//

UENUM(BlueprintType)
enum StatsNames
{
	SN_Defense  UMETA(DisplayName = "Defense"),
	SN_Strength UMETA(DisplayName = "Strength"),
	SN_Spirit	UMETA(DisplayName = "Spirit"),
	SN_Vitality UMETA(DisplayName = "Vitality"),
};

enum InputMode
{
	IM_UI_Only,
	IM_GameModeOnly
};


UCLASS()
class FINAL_PROJECT_API AMC_Behaviour : public ACharacter
{
	GENERATED_BODY() 

public: //Create components.//

	// Sets default values for this character's properties
	AMC_Behaviour();

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyCharacter, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FirstPersonCam;

//DELEGATES :
public: //Delegates :

	DECLARE_MULTICAST_DELEGATE_OneParam(FEnemyDeath, float);
	FEnemyDeath EnemyDeath;
	DECLARE_MULTICAST_DELEGATE_OneParam(FEnemyDamage, float);
	FEnemyDamage EnemyDamage; 
	

public : //PUBLIC UPROPERTY :
	
	//Anim notify :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		bool animIsNotified = false;

	//Movement :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		bool isRunning = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		bool isMoving = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		bool isInAir = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		float AccumulatedSeconds = 0.0F;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		float teleportTrigger = 3.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		float AccumulatedSecondsRegen = 0.0F;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		float regenTrigger = 5.0F;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		bool isRegenerating = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		bool isPoisoned = false;
	
	//Sheath : 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		bool isSheathed = false;
		
	//Attacks : 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		bool isAttacking = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		bool attackingLeft = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		bool attackingRight = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		bool attackingForward = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		bool attackingBackward = false;

		
	//Shield :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		bool bool_IsBlocking = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		bool shieldHit = false;
		
	//Controller :
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	//Experience and Level :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		int characterLevel = 1;	

	UPROPERTY(BlueprintReadWrite)
		TEnumAsByte<StatsNames> Stat_Names;




public : //PUBLIC UFUNCTIONS : 

	//Attacks :
	UFUNCTION(BlueprintCallable)
		void SwordSwung
		(
			UPrimitiveComponent* hitComponent,
			AActor* enemyActor,
			UPrimitiveComponent* otherComponent,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult &hit
		);

	//Experience and Stats :
	UFUNCTION(BlueprintCallable)
		int Get_Stats(StatsNames statName);
	UFUNCTION(BlueprintCallable)
		int Get_CurrentAvailableStats();
	UFUNCTION(BlueprintCallable)
		float Get_Current_Experience();
	UFUNCTION(BlueprintCallable)
		float Get_ExperienceNeeded();

	UFUNCTION(BlueprintCallable)
		float Get_MaxHealth();
	UFUNCTION(BlueprintCallable)
		float Get_CurrentHealth();


	UFUNCTION(BlueprintCallable)
		float Get_CurrentRemainingPoint();

	UFUNCTION(BlueprintCallable)
		void Set_AvailableStats();
	UFUNCTION(BlueprintCallable)
		void Set_StatsPoints(StatsNames statName);

	UFUNCTION(BlueprintCallable)
		void BackgroundEvent();

	UFUNCTION(BlueprintCallable)
		void SetCameraSaturation(FVector4 NewColorSaturation);

	UFUNCTION(BlueprintCallable)
		void setSwordCollision();

	void EnableDisable_Input(int enableORDisable);


public: //PUBLIC CUSTOM EVENTS : 

	UFUNCTION(BlueprintImplementableEvent)
		void SetFOV();
	UFUNCTION(BlueprintImplementableEvent)
		void LevelUp();
	UFUNCTION(BlueprintImplementableEvent)
		void PlayerDeath();
	UFUNCTION(BlueprintImplementableEvent)
		void DeathWidgetAnimationDone();
	UFUNCTION(BlueprintImplementableEvent)
		void PlayerGetHit();

//PROTECTED C++ FUNCTIONS : 
protected: 
	virtual void BeginPlay() override;

	//Moving input : 
	void MoveForward(float Value); //Forward and backward.//
	void MoveRight(float Value); //Left and right.//
	void SetGoingBackward(float Value); //Check for direction of the player's direction.//

	//Running : 
	void RunSet(); //Running flip/flop.//

	//Jumping : 
	void Jump();
	void StopJumping();

	//Lock-On System :
	void LockOnPressed(); //Triggers the sphere, check for enemies within, return closest and lock onto it.//
	void TargetLock(AActor* closestActor);
	void AddSweepedEnemies(bool isHit, TArray<FHitResult>& OutHits_Array, TArray<AActor*>& Actor_Array); //Add every single enemies within the sweep sphere in a Vector.//
	FRotator MC_NewRotation(AActor* closestActor); //Break the player's camera rotation to fix it on the closest enemy.//
	static AActor* GetClosestActor(FVector& actorLocation, AActor* closestActor); //Sorts the closest enemy enemy to the play and puts it in given variable.//

	//Camera input : 
	void TurnAtRate(float Rate); //Look up and down.//	
	void LookUpAtRate(float Rate); //Turn left and right.//

	//Dodge functions : 
	void Dodge(); //Teleport player using Teleport() to given space, if it hits something in the way, it will teleport to it, otherwise it teleports at maximum range.
	void Teleport(bool somethingInTheWay, FHitResult& outHit, FVector& start, FVector& end);

	//Sheathing : 
	void SetSheath(); //Sheath flip/flop.//

	//Blocking :
	void SetBlock(); //Block flip/flop///

	//Attacks : 
	void AttackPressed();
	void AttackReleased(); //Resets the lot of attack variables.//
	void AttackDoOnce_Func();
	//Changes the attack animations given where the player's moving.//
	void LeftRight_Attack_Setter(float MoveRight_Value, float MoveForward_Value); 
	void ForwardBackward_Attack_Setter(float MoveForward_Value);

	void SwordAttack_OverlapSetter();

	//Experience :
	void Experience_NextLeveL_Update(); //Creates a new experience goal for the next level up given an exponential formula.//
	void enemyKilled(float experienceGain);
	void UpdateVitality();

	void Update_Stats_Affected(StatsNames statsnames, float multiplier, float incrementer);

	//Increases player's experience but also takes care of managing the player's level incrementation.//
	void TakeDamage(float damageInput); //Decreases Player's health on call.//

	void PoisonDamageTick(float deltaTime);

	void RegenHealth();

	void OnDeath();

	void RemoveRandomStat_Prequel();

	void RemoveRandomStat(float NumberToRemove);



	//Get Components : 
	UBoxComponent* GetBoxComponentByName(FString ComponentVariableName);
	
	//Damage Update
	void UpdateSwordDamage(); //Takes care of incrementing the sword's damage output depending of the STR stat, follows a set of 5 different linear formulas to create a soft and hard cap.//



	//HUD related :  
	void Set_TimeDilation(float inputValue); //Changes the current time dilation.//
	void Set_Inputmode(InputMode inputMode); //Changes the current input mode of the game.//
	void Set_ShowCursor(bool inputValue); //Displays or removes the cursor from the screen.//
	
	void OpenClose_StatHUD(); //Flip/flop to open up and close the <Stat HUD>, lets the player have access to it with the mouse and.//

	void Test();


public:	
	//Called every frame
	virtual void Tick(float DeltaTime) override;

	typedef void (AMC_Behaviour::* FunctionToCall)(void);
	void RetriggerableDelay(FunctionToCall functionToCall, float deltaSeconds, float triggerThreshold);
	void RetriggerableDelayRegen(FunctionToCall functionToCall, float deltaSeconds, float triggerThreshold);

	void ResetTeleport();

	//Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	//Timer
	FTimerHandle MemberTimerHandle;
};

