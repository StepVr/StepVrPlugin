// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "StepVr.h"
#include "CoreMinimal.h"


#define STEPVR_FRAME	(StepVrGlobal::GetInstance()->GetStepVrManager())


#define STEPVR_FRAME_IsValid	(StepVrGlobal::GetInstance()->SDKIsValid())


#define STEPVR_GLOBAL   (StepVrGlobal::GetInstance())
#define STEPVR_GLOBAL_IsValid (StepVrGlobal::GetInstance()->GlobalIsValid())

//class FStepVrServer;
class UStepSetting;
class FStepFrames;



/************************* Global Data ***************************************/
typedef TArray<FTransform>					AllSkeletonData;
typedef TMap<int32, FTransform>				AllDevicesData;

/************************************************************************/
class STEPVRPLUGIN_API StepVrGlobal 
{
public:
	StepVrGlobal();
	~StepVrGlobal();

	static StepVrGlobal* GetInstance();

	bool GlobalIsValid();

	static void Shutdown();

	void StartSDK();

	//bool ServerIsValid();
	bool SDKIsValid();

	StepVR::Manager*	GetStepVrManager();
	FStepFrames*		GetStepVrReplicateFrame();

	void LoadServer();
	void LoadSDK();
	void CloseSDK();

	void EngineBeginFrame();

	/*获取定位状态*/
	void SVGetDeviceState(StepVR::SingleNode* InSingleNode, int32 EquipId, FTransform& Transform);


	bool IsValidPlayerAddr();
	uint32 GetPlayerAddr();
	UWorld* GetWorld();

	FString GetLocalAddressStr();
public:
	void SVGetDeviceStateWithID(int32 DeviceID, FTransform& Transform);

	AllDevicesData GetAllDevicesData();

	/*缩放*/
	void SetScaleTransform(float ScaleTransform);

	//获取同步数据
	void GetLastReplicateDeviceData(uint32 lPlayerID, int32 DeviceID, FTransform& Data);

private:
	static TSharedPtr<StepVrGlobal>	SingletonInstance;

	//定位服务
	TSharedPtr<StepVR::Manager>	StepVrManager;

	//同步数据管理
	TSharedPtr<FStepFrames>		StepVrReplicateData;

	//需要同步的ID
	TArray<int32>   ReplicateID;
	//需要更新的ID

	//DLL
	void*	DllHandle;

	FDelegateHandle EngineBeginFrameHandle;
public:
	TArray<int32>	NeedUpdateDevices;

	//需要获取定位的设备ID
	TArray<int32>	GNeedUpdateDevices;

	uint32   PlayerID;


	//本机定位数据
	AllDevicesData	GLocalDevicesRT;

	//定位缩放比例
	 float			GScaleTransform;
};


/************************************************************************/
/* 同步数据                                                                     */
/************************************************************************/


class FStepAllPlayerFrame
{
public:
	//最后更新时间
	float TimeStemp = 0.f;

	//当前帧
	int32 CurFrams = 0;

	//TMap<uint32, FStepFrame> AllPlayerInfo;
};


const int32 StepFramsMax = 2;
class FStepFrames
{
public:
	FStepFrames();

	//准备添加新数据的容器
	FStepAllPlayerFrame* GetHeadContainer();
	void FlushHeadContain();

private:
	//暂时缓存两帧，用于插值
	FStepAllPlayerFrame CacheFrames[StepFramsMax];

	//预测数据，临时记录
	FStepAllPlayerFrame ForecastFrame;

	//当前头ID
	int32 IndexCurHead;

	//预备头ID
	int32 IndexHeadContain;

	//最新数据
	FStepAllPlayerFrame* NewFrame;
};



//计时
class StepTime
{
public:
	static TSharedPtr<StepTime> GetTime();
	virtual ~StepTime() {}

	//刷新为当前时间
	virtual void ResetTime() {}

	//上次刷新间隔,并刷新当前时间 毫秒
	//double times = __Test->IntervalAndReset(); 再使用
	virtual double IntervalAndReset() { return -1; }

	//上次刷新间隔 毫秒
	virtual double Interval_MS() { return -1; }

	//上次刷新间隔 微秒
	virtual int64 Interval_Micro() { return -1; }
};