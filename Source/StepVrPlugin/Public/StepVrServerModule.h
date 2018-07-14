// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#if !!(AFTER_ENGINEVERSION_413)
#include "CoreMinimal.h"
#endif

#include "GenericPlatform/GenericApplicationMessageHandler.h"
#include "Features/IModularFeatures.h"
#include "Features/IModularFeature.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

#define STEPVR_SERVER_MODULE_NAME	TEXT("StepVrServer")
class FStepVrServer;



/**
 * 数据
 */
struct IKinemaReplicateData
{
	uint32				PlayerID;
	//double				ReplicateTime;
	TArray<int32>		SkeletionIDs;
	TArray<FTransform>	SkeletonInfos;
	IKinemaReplicateData()
	{
		PlayerID = 0;
		//ReplicateTime = 0;
		SkeletionIDs = {};
		SkeletonInfos = {};
	}
};

FORCEINLINE FArchive& operator<<(FArchive& Ar, IKinemaReplicateData& ArData)
{
	Ar << ArData.PlayerID;
	//Ar << ArData.ReplicateTime;
	Ar << ArData.SkeletionIDs;
	Ar << ArData.SkeletonInfos;
	return Ar;
}


class IStepvrServerModule : public IModuleInterface, public IModularFeature
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

	static inline IStepvrServerModule& Get()
	{
		return FModuleManager::LoadModuleChecked< IStepvrServerModule >(STEPVR_SERVER_MODULE_NAME);
	}

	static inline bool IsAvailable()
	{
		
		return FModuleManager::Get().IsModuleLoaded(STEPVR_SERVER_MODULE_NAME);
	}

	virtual TSharedPtr<FStepVrServer> CreateServer() = 0;
};





typedef TMap<int32, FTransform> StepVRRemoteData;
class ReplciateComponment
{
public:
	virtual ~ReplciateComponment() {}
	virtual void ReceiveRemoteData(TMap<int32, FTransform>&	DeviceInfo);

protected:
	virtual void GetRemoteData(int32 DeviceID,FTransform& data);
	float LastReplicateTime;

	StepVRRemoteData	RemoteData;
};




//Server
class FStepVrServer
{
public:
	virtual ~FStepVrServer() {}

	//StepVR设备同步
	virtual void SetReplciatedDeviceID(TArray<int32> Devices) {}
	virtual void RegistDelegate(int32 Playerid, ReplciateComponment* delegate, bool IsLocal) {}
	virtual void UnRegistDelegate(int32 Playerid, ReplciateComponment* delegate, bool IsLocal) {}

	//IKinema 同步
	virtual void IkinemaSendData(const IKinemaReplicateData& InData) {}
	virtual void IkinemaGetData(uint32 InPlayerID , IKinemaReplicateData& InData) {}
};
