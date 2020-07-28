// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "StepVrBPLibrary.h"
#include "StepVrData.h"
#include "Engine.h"



UStepVrBPLibrary::UStepVrBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UStepVrBPLibrary::SetGameType(FGameType type, FString ServerIP)
{
	if (STEPVR_Data_IsValid)
	{
		STEPVR_Data->SetGameModeType((EGameModeType)type);
		if (type == FGameType::GameClient)
		{
			STEPVR_Data->UpdateServerIP(ServerIP);
		}
	}
}

void UStepVrBPLibrary::SetScaleTransform(float Scales)
{
	//GScaleTransform = Scales;
}
