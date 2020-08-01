// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Perception/PawnSensingComponent.h"
#include "MC_Behaviour.h"

#include "Blueprint/AIAsyncTaskBlueprintProxy.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Arachnid_Behaviour.generated.h"

UCLASS()
class AArachnid_Behaviour : public ACharacter
{
	GENERATED_BODY()

public: //Generated upon creation :
	AArachnid_Behaviour();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected: //Generated upon creation :
	virtual void BeginPlay() override;


protected: //Self-made functions :

	//Damage and Experience :
	float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	void GiveXP(float experienceGain);

	//Movement : 
	void AI_MoveTo(const FVector& destination, AMC_Behaviour* PlayerCharacter, float acceptanceRadius);
	typedef void (AArachnid_Behaviour::* FunctionToCall)(void);
	void RetriggerableDelay(FunctionToCall functionToCall, float deltaSeconds, float triggerThreshold);

	//Casts : 
	AMC_Behaviour* CastToPlayerCharacter();

public: //Variables

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Awareness)
		class UPawnSensingComponent* PawnSensor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Awareness)
		bool  moveTowardPlayer = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Death)
		bool isDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Anim)
		bool isShouting = false;


public: //BLUEPRINT FUNCTIONS : 

	//Pawn sensing : 
	UFUNCTION(BlueprintCallable)
		void OnSeePawn(APawn* OtherPawn);

	//Damage :
	UFUNCTION(BlueprintCallable)
		void DoDamageToPlayer(float damageInput);

	//Health :
	UFUNCTION(BlueprintCallable)
	float Get_CurrentHealth();
	UFUNCTION(BlueprintCallable)
	float Get_MaximumHealth();


public: //C++ TO BLUEPRINT EVENTS

	UFUNCTION(BlueprintImplementableEvent)
		void OnDeath();
	UFUNCTION(BlueprintImplementableEvent)
		void OnHit();

};
