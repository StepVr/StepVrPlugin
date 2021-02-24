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

void UStepVrBPLibrary::SetScaleTransform(bool NewState, FVector Scales)
{
	STEPVR_GLOBAL->SetScaleTransform(NewState,Scales);
}


void UStepVrBPLibrary::SetOffsetTransform(bool NewState, FVector Offset)
{
	STEPVR_GLOBAL->SetOffsetTransform(NewState, Offset);
}

bool UStepVrBPLibrary::GetDeviceTransform(int32 DeviceID, FTransform& OutData)
{
	return STEPVR_GLOBAL->GetDeviceTransform(DeviceID, OutData);
}

void UStepVrBPLibrary::SetUseStepMotionController(bool UseStep)
{
	STEPVR_GLOBAL->UseStepMotionController = UseStep;
}

bool UStepVrBPLibrary::StepSetKartMaxSpeed(int32 Percent)
{
	return STEPVR_GLOBAL->SetKartMaxSpeed(Percent) == 0;
}

bool UStepVrBPLibrary::StepSetKartBrake(bool bSet)
{
	return STEPVR_GLOBAL->SetKartBrake(bSet) == 0;
}

bool UStepVrBPLibrary::StepSetKartEnableReverse(bool bset)
{
	return STEPVR_GLOBAL->SetKartEnableReverse(bset) == 0;
}

bool UStepVrBPLibrary::StepSetKartForward(bool bForward)
{
	return STEPVR_GLOBAL->SetKartForward(bForward) == 0;
}
