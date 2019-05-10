// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IInputDeviceModule.h"


#define STEPVR_PLUGIN_MODULE_NAME	TEXT("StepVrPlugin")

class FStepVrPluginModule : public IInputDeviceModule
{
public:
	static inline FStepVrPluginModule& Get()
	{
		return FModuleManager::LoadModuleChecked< FStepVrPluginModule >(STEPVR_PLUGIN_MODULE_NAME);
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded(STEPVR_PLUGIN_MODULE_NAME);
	}

	static FName GetModularFeatureName()
	{
		static FName FeatureName = FName(STEPVR_PLUGIN_MODULE_NAME);
		return FeatureName;
	}

	virtual TSharedPtr< class IInputDevice > CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};