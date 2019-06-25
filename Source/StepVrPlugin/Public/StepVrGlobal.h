// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "StepVr.h"
#include "CoreMinimal.h"

#define STEPVR_FRAME	StepVrGlobal::GetInstance()->GetStepVrManager()
#define STEPVR_SERVER	StepVrGlobal::GetInstance()->GetStepVrServer()

#define STEPVR_FRAME_IsValid	StepVrGlobal::GetInstance()->SDKIsValid()
#define STEPVR_SERVER_IsValid	StepVrGlobal::GetInstance()->ServerIsValid()

class FStepVrServer;

class STEPVRPLUGIN_API StepVrGlobal
{
public:
	StepVrGlobal();
	~StepVrGlobal();

	static StepVrGlobal* GetInstance();
	static void Shutdown();

	void StartSDK();

	bool ServerIsValid();
	bool SDKIsValid();

	StepVR::Manager*	GetStepVrManager();
	FStepVrServer*		GetStepVrServer();

private:
	void LoadServer();
	void LoadSDK();
	void CloseSDK();
	
	void EngineBeginFrame();
	void PostLoadMapWithWorld(UWorld* UsingWorld);

	UWorld* GetWorld();
private:
	static TSharedPtr<StepVrGlobal>	SingletonInstance;

	TSharedPtr<FStepVrServer>	StepVrServer;
	TSharedPtr<StepVR::Manager>	StepVrManager;

	void*	DllHandle;

	FDelegateHandle EngineBeginFrameHandle;
	FDelegateHandle PostLoadMapHandle;

	UWorld* CurUsingWorld = nullptr;
};
