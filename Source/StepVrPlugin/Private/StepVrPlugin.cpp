// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "StepVrPlugin.h"
#include "StepVrServerModule.h"
#include "StepVrInput.h"
#include "StepVrGlobal.h"


TSharedPtr< class IInputDevice > FStepVrPluginModule::CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	StepVrGlobal::CreateInstance();

	return TSharedRef<class IInputDevice>(new FStepVrInput(InMessageHandler));
}
	
IMPLEMENT_MODULE(FStepVrPluginModule, StepVrPlugin)

void FStepVrPluginModule::ShutdownModule()
{
	StepVrGlobal::Shutdown();
}
