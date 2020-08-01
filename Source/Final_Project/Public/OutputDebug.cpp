// Fill out your copyright notice in the Description page of Project Settings.


#include "OutputDebug.h"
#include "Engine.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"

OutputDebug::OutputDebug()
{


}

OutputDebug::~OutputDebug()
{

}

//Warning Messages and other debug.//
void OutputDebug::DebugFString(FString Text_Output)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::FString(Text_Output));
}
void OutputDebug::DebugFStringFloat(float input_value, FString text_Output)
{
	FString inputValue = FString::SanitizeFloat(input_value);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::FString(text_Output + inputValue));
}

void OutputDebug::DebugFStringInt(int input_Value, FString text_Output)
{
	FString inputValue = FString::FromInt(input_Value);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::FString(text_Output + inputValue));
}

void OutputDebug::DebugFStringBool(bool value, FString Text_Output)
{
	if (value)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::FString(Text_Output + "True"));;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::FString(Text_Output + "False"));;
	}
}