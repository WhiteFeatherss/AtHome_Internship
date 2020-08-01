#pragma once


#include "Perception/PawnSensingComponent.h"
#include "MC_Behaviour.h"

#include "Blueprint/AIAsyncTaskBlueprintProxy.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Demon_Behaviour.generated.h"

UCLASS(Blueprintable)
class ADemon_Behaviour : public ACharacter
{
	GENERATED_BODY()

public:
	ADemon_Behaviour();

protected:
	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;
	void UnseenReturnRandom(float deltaSecond);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	AMC_Behaviour* CastToPlayerCharacter();

	void UnseenForXSecond(float deltaSecond, float triggerTime);



	//Flip Flops :
	void FlipFlop_DoOnce_RandomMove();
	void FlipFlop_slowDownTrigger_True_DoOnce();
	void FlipFlop_slowDownTrigger_False_DoOnce();
	void FlipFlop_Roar();
	void FlipFlop_DoOnce_SeenTarget();
	void FlipFlop_DoOnce_AttackFromBack();



	//Ai Move To : 
	void AI_MoveTo(const FVector& destination, AMC_Behaviour* PlayerCharacter, float acceptanceRadius); //AI Move To equivalent
	void RandomLocationGoTo();
	void GetRandomLocation();


	//Begin play :
	void VariableInitialize();


	//Damage and Experience :
	float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void GiveXP(float experienceGain);
	


	void SeenTarget();
	void Timeline_SlowDown_Trigger();
	void DoTheRoar();
	void Reset_PlayerSeen();


	
	//timers :
	typedef void (ADemon_Behaviour::* FunctionToCall)(void);
	void RetriggerableDelay(FunctionToCall functionToCal, float deltaSeconds, float triggerThreshold);

	FTimerHandle MemberTimerHandle;


	typedef void (ADemon_Behaviour::* FunctionTEST)(void);
	void PointerTest(FunctionTEST funct);

public: //UPROPERTIES :
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Awareness)
		class UPawnSensingComponent* PawnSensor;

public: //UFUNCTIONS : 
	UFUNCTION(BlueprintCallable)
		void OnSeePawn(APawn* OtherPawn);
	UFUNCTION(BlueprintCallable)
		void slowDownTrigger_True_Speed_Lerp(float lerpValue);
	UFUNCTION(BlueprintCallable)
		void slowDownTrigger_False_Speed();
	UFUNCTION(BlueprintCallable)
		void DoDamageToPlayer();

public : //Blueprint Events :
	UFUNCTION(BlueprintImplementableEvent)
		void SpeedTimeline();

	UFUNCTION(BlueprintImplementableEvent)
		void SetAnim_Roar();

	UFUNCTION(BlueprintImplementableEvent)
		void OnDeath();



public: //ABP :
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BOOL)
		bool triggerRoar = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BOOL)
		bool runTowardEnemy = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BOOL)
		bool playerHasBeenSeen = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BOOL)
		bool playerIsBeingSeen = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BOOL)
		bool isCloseEnoughAttack = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BOOL)
		bool attackSlowDownTrigger = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BOOL)
		bool EnemyReturnRandomLocation = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BOOL)
		bool hasTakenDamage = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FLOAT)
		float speed = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = VECTOR)
		FVector playerLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = INT)
		int attackAnimationSelector;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HEALTH)
		float currentHealth = 400.0F;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HEALTH)
		float maxHealth = 400.0F;


public: //Weird delegate for Random Move To
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOAISimpleDelegate, EPathFollowingResult::Type, unsignificantName);
	FOAISimpleDelegate AiMoveTo_Success;

	UFUNCTION(BlueprintCallable, meta=(Category, OverrideNativeName = "OnSuccessFunction"))
	void MoveTo_Success(EPathFollowingResult::Type unsignificantName);

};
