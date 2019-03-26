// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericPlatform/GenericApplicationMessageHandler.h"
#include "Features/IModularFeatures.h"
#include "Features/IModularFeature.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

#define STEPVR_SERVER_MODULE_NAME	TEXT("StepVrServer")
class FStepVrServer;

enum EGameModeType
{
	EInValid,
	EClient,
	EServer,
};

/**
 * 数据
 */
struct IKinemaReplicateData
{
	float				Scale;
	uint32				PlayerID;
	TArray<int32>		SkeletionIDs;
	TArray<FTransform>	SkeletonInfos;
	IKinemaReplicateData()
	{
		Scale = 1.f;
		PlayerID = 0;
		SkeletionIDs = {};
		SkeletonInfos = {};
	}
};

FORCEINLINE FArchive& operator<<(FArchive& Ar, IKinemaReplicateData& ArData)
{
	Ar << ArData.Scale;
	Ar << ArData.PlayerID;
	Ar << ArData.SkeletionIDs;
	Ar << ArData.SkeletonInfos;
	return Ar;
}
struct PlayerDeviceInfo
{
	uint32					PlayerAddr;

	TMap<int32, FTransform>	StepVrDeviceInfo;

	IKinemaReplicateData	IkinemaInfo;

	PlayerDeviceInfo()
	{
		PlayerAddr = 0;
		StepVrDeviceInfo.Empty();
	}
};

FORCEINLINE FArchive& operator<<(FArchive& Ar, PlayerDeviceInfo& ArData)
{
	Ar << ArData.PlayerAddr;
	Ar << ArData.StepVrDeviceInfo;
	Ar << ArData.IkinemaInfo;
	return Ar;
}

typedef TMap<uint32, PlayerDeviceInfo> AllPlayerData;

struct SocketSendInfo
{
	//EGameModeType
	int32					FromWhere;

	uint64					FrameCounts;

	AllPlayerData			AllPlayerDatas;

	SocketSendInfo()
	{
		FromWhere = EGameModeType::EInValid;
		FrameCounts = 0;
		AllPlayerDatas.Empty();
	}
};

FORCEINLINE FArchive& operator<<(FArchive& Ar, SocketSendInfo& ArData)
{
	Ar << ArData.FromWhere;
	Ar << ArData.FrameCounts;
	Ar << ArData.AllPlayerDatas;
	return Ar;
}

//所有玩家
extern STEPVRPLUGIN_API AllPlayerData LocalAllPlayerData;
extern STEPVRPLUGIN_API PlayerDeviceInfo LocalPlayerData;

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


class STEPVRPLUGIN_API FStepVrServer
{
public:
	virtual ~FStepVrServer() {}

	uint32 GetLocalAddress();

	//IKinema
	void IkinemaSendData(const IKinemaReplicateData& InData);
	void IkinemaGetData(uint32 InPlayerAddr, IKinemaReplicateData& OutData);

	//StepVr
	void StepVrSendData(uint32 InPlayerAddr, TMap<int32, FTransform>& InData);
	void StepVrGetData(uint32 InPlayerAddr, TMap<int32, FTransform>& OutData);
	

	AllPlayerData& LockAllPlayerData();
	void UnLockAllPlayerData();

private:
	FCriticalSection Section_AllPlayerData;
};