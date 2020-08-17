// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "StepVr.h"
#include "StepVrDataInterface.h"

#include "CoreMinimal.h"


#define STEPVR_GLOBAL			(StepVrGlobal::GetInstance())
#define STEPVR_FRAME			(StepVrGlobal::GetInstance()->GetStepVrManager())
#define STEPVR_FRAME_ENABLE		(StepVrGlobal::GetInstance()->SDKIsValid())


enum class ECommandState : uint8
{
	Stat_RecordOnline,
	Stat_RecordCamera,
};


DECLARE_MULTICAST_DELEGATE_TwoParams(FStepCommandDelegate, ECommandState, int32);

/**
 * 全局控制
 */
class STEPVRPLUGIN_API StepVrGlobal 
{
public:
	static StepVrGlobal* GetInstance();
	static void Shutdown();

public:
	//命令行
	FStepCommandDelegate& GetCommandDelegate();
	void ExecCommand(ECommandState NewCommand, int32 Values);

	//SDK
	bool SDKIsValid();
	StepVR::Manager* GetStepVrManager();

	UWorld* GetWorld();

	/************************************************************************/
	/*                            本机设置                                */
	/************************************************************************/
	//本机唯一标识
	uint32 GetGUID();

	//客户端，服务器
	void SetGameType(EStepGameType NewType, FString& NewServerIP);
	EStepGameType GetGameType();

	//录制机器IP
	void SetRecordPCIP(const FString& PCIP);

	 //所有定位Location进行缩放
	void SetScaleTransform(float NewScale);
	void AddDeviceID(int32 DeviceID);
	bool GetDeviceTransform(FDeviceFrame& OutData);
	bool GetDeviceTransform(int32 DeviceID, FTransform& OutData);
	bool GetDeviceTransformImmediately(int32 DeviceID, FTransform& OutData);

	/************************************************************************/
	/*                            远端数据                                */
	/************************************************************************/
	bool GetRemoteDeviceTransform(uint32 GUID, FDeviceFrame& OutData);

protected:
	StepVrGlobal();

	void StartSDK();
	void CloseSDK();
	void LoadServer();
	void LoadSDK();

	void EngineBeginFrame();

	void DataLerp(FDeviceData& inputData, FDeviceData& outputData);

	/*获取定位状态*/
	void UpdateDeviceState(StepVR::SingleNode* InSingleNode, int32 EquipId, FDeviceData& outputData);

private:
	static TSharedPtr<StepVrGlobal>		SingletonInstance;

	//定位服务
	TSharedPtr<StepVR::Manager>			StepVrManager;
	FString								StepVrManagerComplieTime;
	FString								StepVrManagerVersion;

	//定位数据
	TSharedPtr<FStepVrDataInterface>	StepVrData;

	FDelegateHandle						EngineBeginFrameHandle;

	void* DllHandle = nullptr;

	FStepCommandDelegate				CommandDelegate;

private:
	float				ScaleTransform = 1.f;
	TArray<int32>		NeedUpdateDeviceID;

	EStepGameType		GameType = EStepGameType::EStandAlone;
	FString				GameServerIP = "";

	//唯一标识
	int64				GameGUID = 0;

	//本机定位数据
	FDeviceFrame		GameDevicesFrame;
	
	//远端定位数据
	AllPlayerDevice		GameAllPlayer;
	int64				GameAllPlayerLastTicks;

	//用于插值的参数
	//缓存上一帧输出的数据
	FDeviceData       OutPutDataBuff;
	bool isFirst = true;			//第一帧对齐的标志
	int32 DelatTime = 100;			//延迟的时间ms
	int64 FirstTime1, FirstTime2;	//第一帧对齐时候两个时间戳
};

