// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Perception/PawnSensingComponent.h"
#include "MC_Behaviour.h"

#include "Blueprint/AIAsyncTaskBlueprintProxy.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Knight_Behaviour.generated.h"

UCLASS()
class AKnight_Behaviour : public ACharacter
{
	GENERATED_BODY()

public: //Generated upon creation :
	AKnight_Behaviour();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public: //Variables

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Awareness)
		class UPawnSensingComponent* PawnSensor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Awareness)
		bool  moveTowardPlayer = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Awareness)
		bool  longRangePartOne = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = LongRange)
		bool longRangeHit = false;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Death)
		bool isDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
		float MaxHealth = 650.0F;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Death)
		float CurrentHealth = 650.0F;

protected: //Generated upon creation :
	virtual void BeginPlay() override;

protected: //Self-made functions :

	//Damage and Experience :
	float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	void GiveXP(float experienceGain);


	//Animations : 
	void PowerUp();

	//Movement : 
	void AI_MoveTo(const FVector& destination, AMC_Behaviour* PlayerCharacter, float acceptanceRadius);
	typedef void (AKnight_Behaviour::* FunctionToCall)(void);
	void RetriggerableDelay(FunctionToCall functionToCall, float deltaSeconds, float triggerThreshold);

	//Casts : 
	AMC_Behaviour* CastToPlayerCharacter();

public: //BLUEPRINT ACCESSIBLE VARIABLES :


public: //BLUEPRINT FUNCTIONS : 
	UFUNCTION(BlueprintCallable)
		void OnSeePawn(APawn* OtherPawn);
	UFUNCTION(BlueprintCallable)
		void DoDamageToPlayer(float damageInput);

public: //C++ TO BLUEPRINT EVENTS
	UFUNCTION(BlueprintImplementableEvent)
		void PowerUp_Anim();
	UFUNCTION(BlueprintImplementableEvent)
		void LongRangeAttack();
	UFUNCTION(BlueprintImplementableEvent)
		void OnDeath();
	UFUNCTION(BlueprintImplementableEvent)
		void OnHit();
};
