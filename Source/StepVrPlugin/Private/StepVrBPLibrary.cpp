// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "StepVrBPLibrary.h"
#include "StepVrGlobal.h"




UStepVrBPLibrary::UStepVrBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UStepVrBPLibrary::SetGameType(EStepGameType type, FString ServerIP)
{
	STEPVR_GLOBAL->SetGameType(type, ServerIP);
}

void UStepVrBPLibrary::SetScaleTransform(float Scales)
{
	STEPVR_GLOBAL->SetScaleTransform(Scales);
}
