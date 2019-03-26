// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "StepVrPlugin.h"
#include "StepVrInput.h"
#include "StepVrGlobal.h"


void FStepVrPluginModule::StartupModule()
{
	IInputDeviceModule::StartupModule();

	StepVrGlobal::GetInstance()->StartSDK();
}

void FStepVrPluginModule::ShutdownModule()
{
	StepVrGlobal::Shutdown();
}

TSharedPtr< class IInputDevice > FStepVrPluginModule::CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	return TSharedRef<class IInputDevice>(new FStepVrInput(InMessageHandler));
}

IMPLEMENT_MODULE(FStepVrPluginModule, StepVrPlugin)