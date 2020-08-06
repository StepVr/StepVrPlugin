// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Features/IModularFeature.h"


#define STEPVR_SERVER_MODULE_NAME	TEXT("StepVrServer")

class FStepVrDataInterface;
class STEPVRPLUGIN_API IStepvrServerModule : public IModuleInterface, public IModularFeature
{
public:
	static FName GetModularFeatureName()
	{
		static FName FeatureName = FName(STEPVR_SERVER_MODULE_NAME);
		return FeatureName;
	}

	virtual void StartupModule() override
	{
		IModularFeatures::Get().RegisterModularFeature(GetModularFeatureName(), this);
	}
	virtual void ShutdownModule() override
	{
		IModularFeatures::Get().UnregisterModularFeature(GetModularFeatureName(), this);
	}
	static inline IStepvrServerModule& Get()
	{
		return FModuleManager::LoadModuleChecked< IStepvrServerModule >(STEPVR_SERVER_MODULE_NAME);
	}

	static inline bool IsAvailable()
	{

		return FModuleManager::Get().IsModuleLoaded(STEPVR_SERVER_MODULE_NAME);
	}

	virtual TSharedPtr<FStepVrDataInterface> CreateStepVrData() = 0;
};