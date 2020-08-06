// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "StepVr.h"
#include "StepVrDataInterface.h"

#include "CoreMinimal.h"


#define STEPVR_GLOBAL			(StepVrGlobal::GetInstance())
#define STEPVR_FRAME			(StepVrGlobal::GetInstance()->GetStepVrManager())
#define STEPVR_FRAME_ENABLE		(StepVrGlobal::GetInstance()->SDKIsValid())


/**
 * 全局控制
 */
class STEPVRPLUGIN_API StepVrGlobal 
{
public:
	static StepVrGlobal* GetInstance();
	static void Shutdown();

public:
	bool SDKIsValid();
	
	//本机唯一标识
	uint32 GetGUID();

	//客户端，服务器
	void SetGameType(EStepGameType NewType, FString& NewServerIP);
	EStepGameType GetGameType();

	//所有定位Location进行缩放
	void SetScaleTransform(float NewScale);

	//添加获取ID
	void AddDeviceID(int32 DeviceID);
	//获取定位数据
	bool GetDeviceTransform(SinglePlayer& OutData);
	bool GetDeviceTransformImmediately(int32 DeviceID, FTransform& OutData);
	bool GetRemoteDeviceTransform(uint32 GUID, SinglePlayer& OutData);

	StepVR::Manager* GetStepVrManager();

	UWorld* GetWorld();

protected:
	StepVrGlobal();

	void StartSDK();
	void CloseSDK();
	void LoadServer();
	void LoadSDK();

	void EngineBeginFrame();

	/*获取定位状态*/
	void UpdateDeviceState(StepVR::SingleNode* InSingleNode, int32 EquipId);

private:
	static TSharedPtr<StepVrGlobal>		SingletonInstance;

	//定位服务
	TSharedPtr<StepVR::Manager>			StepVrManager;

	//定位数据
	TSharedPtr<FStepVrDataInterface>	StepVrData;

	FDelegateHandle						EngineBeginFrameHandle;

	void* DllHandle;

private:
	float			ScaleTransform = 1.f;
	TArray<int32>	NeedUpdateDeviceID;

	EStepGameType	GameType = EStepGameType::EStandAlone;
	FString			GameServerIP = "";

	//唯一标识
	uint32			GameGUID;

	//本机定位数据
	SinglePlayer	GameLocalPlayer;
	//远端定位数据
	AllPlayer		GameAllPlayer;
};

