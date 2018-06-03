// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
	FORCEINLINE bool ServerIsValid() { return StepVrServer.IsValid(); }

	/** Return true you can use macro STEPVR_FRAME */
	FORCEINLINE bool SDKIsValid() { return StepVrManager.IsValid(); }

	FORCEINLINE StepVR::Manager* GetStepVrManager() 
	{
		if (!StepVrManager.IsValid()) { return nullptr; }
		return StepVrManager.Get();
	}
	FORCEINLINE FStepVrServer* GetStepVrServer()
	{
		if (!StepVrServer.IsValid()) { return nullptr; }
		return StepVrServer.Get();
	}
public:
	/** Deal Singleton Instance Method */
	static void CreateInstance();
	static void Shutdown();
	static FORCEINLINE StepVrGlobal* Get()
	{
		if (!SingletonInstance.IsValid()) { return nullptr; }
		return SingletonInstance.Get();
	}
	
	/** Clear */
	void CloseSDK();

	/** Custom Set Replicate DevicesID */
	void SetReplicatedDevices(TArray<int32> Devices);
	FORCEINLINE TArray<int32>&	GetReplicatedDevices()
	{
		return ReplicateDevicesID;
	}
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
