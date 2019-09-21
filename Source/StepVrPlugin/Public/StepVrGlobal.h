// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "StepVr.h"
#include "CoreMinimal.h"

#define STEPVR_FRAME	(StepVrGlobal::GetInstance()->GetStepVrManager())
#define STEPVR_SERVER	(StepVrGlobal::GetInstance()->GetStepVrServer())

#define STEPVR_FRAME_IsValid	(StepVrGlobal::GetInstance()->SDKIsValid())
#define STEPVR_SERVER_IsValid	(StepVrGlobal::GetInstance()->ServerIsValid())

class FStepVrServer;
class UStepSetting;


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
	//FDelegateHandle PostLoadMapHandle;
};



/************************* Global Data ***************************************/
typedef TMap<int32, FTransform> AllDevicesTrans;
typedef TArray<FTransform>      AllSkeletonData;

//本机定位数据
extern AllDevicesTrans	GLocalDevicesRT;

//同步数据
extern TMap<uint32, AllDevicesTrans> GReplicateDevicesRT;
extern STEPVRPLUGIN_API FCriticalSection GReplicateSkeletonCS;
extern STEPVRPLUGIN_API TAtomic<uint64> GUpdateReplicateSkeleton;
extern STEPVRPLUGIN_API TMap<uint32, AllSkeletonData> GReplicateSkeletonRT;

//需要获取定位的设备ID
extern TArray<int32>	GNeedUpdateDevices;
/************************************************************************/