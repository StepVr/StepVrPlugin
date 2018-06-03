// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "StepVrPluginPrivatePCH.h"
#include "StepVrPlugin.h"
#include "StepVrInput.h"
#include "StepVrServerModule.h"
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
