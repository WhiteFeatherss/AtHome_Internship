// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"

/**
 * 
 */
class FINAL_PROJECT_API OutputDebug
{
public:
	OutputDebug();
	~OutputDebug();

	static void DebugFString(FString Text_Output);
	static void DebugFStringFloat(float input_value, FString text_Input);
	static void DebugFStringInt(int input_Value, FString text_Output);
	static void DebugFStringBool(bool value, FString Text_Output);

};
