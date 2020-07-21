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
class FStepAllPlayerFrame;

enum EGameModeType
{
	EStandAlone,
	EClient,
	EServer,
};

////单个玩家数据
//struct FPlayerInfo;
//
//struct STEPVRPLUGIN_API FPlayerInfo
//{
//	//玩家ID
//	uint32	PlayerAddr;
//
//	//最后更新时间
//	float LastUpdate;
//
//	//定位建数据
//	TMap<int32, FTransform>	StepVrDeviceInfo;
//
//	//动捕数据
//	TArray<FTransform>		StepMocapInfo;
//
//public:
//	//设置某个玩家数据
//	static void SetPlayerData(uint32 PlayerAddr, FPlayerInfo& OutData);
//
//	//获取玩家
//	static void GetPlayerData(uint32 PlayerAddr, FPlayerInfo& OutData);
//
//	//设置属性
//	static void SetNewDeviceData(uint32 PlayerAddr, const TMap<int32, FTransform>& NewData);
//	static void SetNewMocapData(uint32 PlayerAddr, const TArray<FTransform>& NewData);
//
//private:
//	/**
//	 * 锁冲突次数 send 30/s + receive 30/s + 
//	 */
//	FCriticalSection CriticalSection;
//
//	
//
//};
//
//typedef TMap<uint32, FPlayerInfo> FAllPlayerInfos;
//static FAllPlayerInfos AllPlayerInfos;
//
//FORCEINLINE FArchive& operator<<(FArchive& Ar, FPlayerInfo& ArData)
//{
//	Ar << ArData.PlayerAddr;
//	Ar << ArData.LastUpdate;
//	Ar << ArData.StepVrDeviceInfo;
//	Ar << ArData.StepMocapInfo;
//	return Ar;
//}








/***************************定位数据************************************/
struct PlayerDeviceInfo
{
	uint32					PlayerAddr;

	//定位建数据
	TMap<int32, FTransform>	StepVrDeviceInfo;

	//动捕数据
	TArray<FTransform>		StepMocapInfo;

	PlayerDeviceInfo()
	{
		PlayerAddr = 0;
		StepVrDeviceInfo.Empty();
		StepMocapInfo.Empty();
	}
};

FORCEINLINE FArchive& operator<<(FArchive& Ar, PlayerDeviceInfo& ArData)
{
	Ar << ArData.PlayerAddr;
	Ar << ArData.StepVrDeviceInfo;
	Ar << ArData.StepMocapInfo;
	return Ar;
}

typedef TMap<uint32, PlayerDeviceInfo> AllPlayerData;
/************************************************************************/


/**************************传输数据************************************/
struct SocketSendInfo
{
	//EGameModeType
	int32					FromWhere;

	uint64					FrameCounts;

	AllPlayerData			AllPlayerDatas;

	SocketSendInfo()
	{
		FromWhere = EGameModeType::EStandAlone;
		FrameCounts = 0;
	}
};

FORCEINLINE FArchive& operator<<(FArchive& Ar, SocketSendInfo& ArData)
{
	Ar << ArData.FromWhere;
	Ar << ArData.FrameCounts;
	Ar << ArData.AllPlayerDatas;
	return Ar;
}
/************************************************************************/



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

	//开始服务
	virtual void StartServer() = 0;
	virtual void StopServer() = 0;

	//本机IP
	static FString GetLocalAddressStr();
	
	//更新联机状态
	EGameModeType GetGameModeType();
	void SetGameModeType(EGameModeType InGameModeType);
    void UpdateServerIP(const FString& InServerIP);

	//StepPlugin/StepMocap
	void StepVrSendData(uint32 InPlayerAddr, TMap<int32, FTransform>& InPlayerData);
	void StepMocapSendData(const TArray<FTransform>& InMocapData);

	bool SynchronizationStepVrData(FStepAllPlayerFrame* NewFrame);

protected:
	/**
	 * 锁冲突次数：GameFrame / ReciveData / SendData
	 */
	FCriticalSection Section_AllPlayerData;
	/**
	 * 锁冲突次数: Game人数变化
	 */
	FCriticalSection Section_GameModeType;

	//游戏状态
	EGameModeType GameModeType = EGameModeType::EStandAlone;
	FString		  ServerIP;

	/**
	 * 本机和远端数据 / Other Thread
	 */
	AllPlayerData		mRemotePlayerData;
	PlayerDeviceInfo	mLocalPlayerData;
	TAtomic<float>		mLastReceiveTime;
};