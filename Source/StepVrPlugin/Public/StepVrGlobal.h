// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "StepVr.h"
#include "CoreMinimal.h"

#define STEPVR_FRAME	StepVrGlobal::Get()->GetStepVrManager()
#define STEPVR_SERVER	StepVrGlobal::Get()->GetStepVrServer()

class FStepVrServer;


class STEPVRPLUGIN_API StepVrGlobal
{
public:
	StepVrGlobal();
	~StepVrGlobal();

	/** Check whether the pointer is valid before use. */
	/** Return true you can use macro STEPVR_SERVER */
	bool ServerIsValid() { return StepVrServer.IsValid(); }

	/** Return true you can use macro STEPVR_FRAME */
	bool SDKIsValid() { return StepVrManager.IsValid(); }

	StepVR::Manager* GetStepVrManager();
	FStepVrServer* GetStepVrServer();

public:
	/** Deal Singleton Instance Method */
	static void CreateInstance();
	static void Shutdown();
	static StepVrGlobal* Get();
	
	static bool ServerIsRun();

	/** Clear */
	void CloseSDK();

	/** Custom Set Replicate DevicesID */
	void SetReplicatedDevices(TArray<int32> Devices);
	TArray<int32> GetReplicatedDevices();

private:
	void LoadServer();
	void LoadSDK();

private:
	static TSharedPtr<StepVrGlobal>	SingletonInstance;

	/** Server */
	TSharedPtr<FStepVrServer>	StepVrServer;
	TArray<int32> ReplicateDevicesID;

	/** StepVr SDK */
	TSharedPtr<StepVR::Manager>	StepVrManager;
	void*	DllHandle;
};
