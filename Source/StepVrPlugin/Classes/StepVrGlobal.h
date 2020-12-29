// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "StepVr.h"
#include "StepVrDataInterface.h"

#include "CoreMinimal.h"


#define STEPVR_GLOBAL			(StepVrGlobal::GetInstance())
#define STEPVR_GLOBAL_ENABLE	(StepVrGlobal::CheckValidInstance())
#define STEPVR_FRAME			(StepVrGlobal::GetInstance()->GetStepVrManager())
#define STEPVR_FRAME_ENABLE		(StepVrGlobal::GetInstance()->SDKIsValid())



class FStepVrAuxiliaryCollection;



enum class ECommandState : uint8
{
	Stat_ServerReceiveRecord,
	Stat_ServerSendRecord,
	Stat_CameraRecord,
	Stat_MocapRecord,
};


DECLARE_MULTICAST_DELEGATE_TwoParams(FStepCommandDelegate, ECommandState, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FStepCommandDelegateStr, ECommandState, FString);

/**
 * 全局控制
 */
class STEPVRPLUGIN_API StepVrGlobal 
{
public:
	friend class FStepVrDataInterface;

	static StepVrGlobal* GetInstance();
	static bool	CheckValidInstance();
	static void Shutdown();

public:
	//命令行
	FStepCommandDelegate& GetCommandDelegate();
	void ExecCommand(ECommandState NewCommand, int32 Values);
	FStepCommandDelegateStr& GetCommandDelegateStr();
	void ExecCommand(ECommandState NewCommand, const FString& Values);

	//SDK
	bool SDKIsValid();
	StepVR::Manager* GetStepVrManager();
	FString GetManagerCompileTime();
	FString GetManagerCompileVersion();


	//本机唯一标识
	uint32 GetGUID();

	//客户端，服务器
	void SetGameType(EStepGameType NewType, FString& NewServerIP);
	EStepGameType GetGameType();

	//一帧数据
	void RefreshFrame(FDeviceFrame& outFrame);
	FDeviceFrame& GetDeviceFrame();
	bool GetDeviceTransform(int32 DeviceID, FTransform& OutData);
	

	/************************************************************************/
	/*                            远端数据                                */
	/************************************************************************/
	bool GetRemoteDeviceFrame(uint32 GUID, FDeviceFrame& OutData);




	/************************************************************************/
	/*                            配置                                */
	/************************************************************************/
	//录制机器IP
	void SetRecordPCIP(const FString& PCIP);
	//定位进行缩放
	void SetScaleTransform(const FVector& NewScale);
	FVector GetScaleTransform();

	//定位进行偏移
	void SetOffsetTransform(const FVector& NewOffset);
	FVector GetOffsetTransform();



	/************************************************************************/
	/*                          卡丁车                                          */
	/************************************************************************/
	//设置最大速度（0到100）
	int32 SetKartMaxSpeed(int32 speed);

	//开启或解除急刹
	int32 SetKartBrake(bool bSet);

	//禁用或启用物理倒挡
	int32 SetKartEnableReverse(bool bSet);

	//设置前进或后退(禁用物理倒挡后才起作用)
	int32 SetKartForward(bool bForward);



	/************************************************************************/
	/*                           Unreal                                           */
	/************************************************************************/
	UWorld* GetWorld();

	//是否使用StepMotionController
	bool UseStepMotionController = true;

protected:
	StepVrGlobal();

	void StartSDK();
	void CloseSDK();
	void LoadServer();
	void LoadSDK();

	void EngineBeginFrame();

protected:
	//辅助软件
	TSharedPtr<FStepVrAuxiliaryCollection> StepVrAuxiliaryCollection;
	
protected:
	/************************************************************************/
	/*                  	用于插值的参数缓存上一帧输出的数据
	/************************************************************************/
	FDeviceData       OutPutDataBuff;
	bool isFirst = true;			//第一帧对齐的标志
	int32 DelatTime = 100;			//延迟的时间ms
	int64 FirstTime1, FirstTime2;	//第一帧对齐时候两个时间戳

	void DataLerp(FDeviceData& inputData, FDeviceData& outputData);

private:
	static TSharedPtr<StepVrGlobal>		SingletonInstance;

	//定位服务
	FCriticalSection					StepVrManagerCritical;
	TSharedPtr<StepVR::Manager>			StepVrManager;
	FString								StepVrManagerComplieTime;
	FString								StepVrManagerVersion;

	//网络定位数据
	TSharedPtr<FStepVrDataInterface>	StepVrData;
	AllPlayerDevice						GameAllPlayer;
	int64								GameAllPlayerLastTicks = 0;

	FDelegateHandle						EngineBeginFrameHandle;

	void* DllHandle = nullptr;

	FStepCommandDelegate				CommandDelegate;
	FStepCommandDelegateStr				CommandDelegateStr;

private:
	FVector				OffsetTransform = FVector::ZeroVector;
	FVector				ScaleTransform = FVector::OneVector;
	TArray<uint8>		NeedUpdateDeviceID;

	//服务器客户端
	EStepGameType		GameType = EStepGameType::EStandAlone;
	FString				GameServerIP = "";

	//唯一标识
	uint32				GameGUID = 0;

	//本机定位数据
	FDeviceFrame		GameDevicesFrame;



};

