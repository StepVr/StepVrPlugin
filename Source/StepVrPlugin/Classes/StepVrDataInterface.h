// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "StepVr.h"
#include "CoreMinimal.h"
#include "StepVrDataInterface.generated.h"





/**
 * 声明数据结构
 */
UENUM()
enum class EStepGameType : uint8
{
	EStandAlone UMETA(Hidden),
	EClient,
	EServer,
};



class STEPVRPLUGIN_API FStepDataTime
{
public:
	static int64 GetInterval(FDateTime& InTime)
	{
		return (FDateTime::Now().GetTicks() - InTime.GetTicks()) / ETimespan::TicksPerMillisecond;
	}

	static int64 GetInterval_Reset(FDateTime& InTime)
	{
		int64 Temp = FStepDataTime::GetInterval(InTime);
		FStepDataTime::ResetDateTime(InTime);
		return Temp;
	}

	static void ResetDateTime(FDateTime& InTime)
	{
		InTime = FDateTime::Now();
	}

	static int64 CurrentTicks()
	{
		return FDateTime::Now().GetTicks();
	}
};

/**
 * 时间戳 用于各种计时
 */
class STEPVRPLUGIN_API FStepTimer
{
public:
	virtual ~FStepTimer() {}

	static TSharedPtr<FStepTimer> GetTimer();

	//当前时间戳
	virtual int64 GetNanoseconds() = 0;
	virtual int64 GetMicroseconds() = 0;
	virtual int64 GetMilliseconds() = 0;
	virtual double GetSeconds() = 0;

	//重置当前时间戳
	virtual void Reset() {}

protected:
	FStepTimer() {}
};





/**
 * 一个定位键数据结构
 */
#pragma region FDeviceData
class STEPVRPLUGIN_API FDeviceData
{
public:
	FDeviceData();

	//临时时间戳
	int64 TemporaryTimestamp;

	//本机 从MMAP获取数据的时间间隔
	int64 GetInterval();

	//从MMAP刷新数据的时间戳
	int64 GetMMAPTicks();
	
	void SetTransform(FTransform& InData);
	FTransform& GetTransform();

	void SetSpeed(const FVector& Indata);

	void SetAcceleration(const FVector& InData);
	FVector& GetAcceleration();

	void SetPalstance(const FVector& InData);

	friend FArchive& operator<< (FArchive& Ar, FDeviceData& ArData)
	{
		Ar << ArData.MMAPTicks;
		Ar << ArData.Transform;
		//Ar << ArData.Speed;
		Ar << ArData.Acceleration;
		//Ar << ArData.Palstance;
		return Ar;
	}

private:
	//位置姿态
	FTransform Transform;

	//速度		GetSpeedVec
	FVector Speed;

	//加速度		GetSpeedAcc
	FVector Acceleration;

	//角速度		GetSpeedGyro
	FVector Palstance;

	//时间戳（UE获取属性开始计时）
	int64					MMAPTicks;
};



/**
 * 所有定位键一帧数据
 */
#pragma region FDeviceFrame
class STEPVRPLUGIN_API FDeviceFrame
{
public:
	bool	HasDevice(uint8 DeviceID);
	void	SetDeviceData(uint8 DeviceID , FDeviceData& InData);
	
	FDeviceData& GetDeviceRef(uint8 DeviceID);

	//获取所有设备信息
	TMap<uint8, FDeviceData>& GetAllDevicesData();

	friend FArchive& operator<< (FArchive& Ar, FDeviceFrame& ArData)
	{
		Ar << ArData.AllDevices;
		return Ar;
	}

	//最后更新时间戳
	void SetTicks(int64 NewTicks);
	int64 GetTIcks();

private:
	int64 UpdateTicks = 0;
	TMap<uint8, FDeviceData> AllDevices;
};



/**
 * 动补一帧数据 
 */
#pragma region FMocapFrame
class STEPVRPLUGIN_API FMocapFrame
{
public:
	FMocapFrame(){}

	float Scale = 1.f;

	TArray<FTransform>  Bones;
	TArray<FRotator>	Hands;

	friend FArchive& operator<< (FArchive& Ar, FMocapFrame& ArData)
	{
		Ar << ArData.Scale;
		Ar << ArData.Bones;
		Ar << ArData.Hands; 
		return Ar;
	}
};

/**
 * 所有玩家数据
 */
typedef TMap<int64, FDeviceFrame>	AllPlayerDevice;
typedef TMap<int64, FMocapFrame>	AllPlayerMocap;



/**
 * StepVR 数据类
 */
#pragma region FStepVrDataInterface
class STEPVRPLUGIN_API FStepVrDataInterface
{
public:
	friend class StepVrGlobal;

	virtual ~FStepVrDataInterface() {}

private:
	virtual void Init() = 0;

	//设置当前游戏状态
	virtual void SetNewGameType(EStepGameType NewType, const FString& NewServerIP) = 0;

	//同步本地数据到Server
	virtual bool SynchronizationToServer(uint32 PlayerGUID, const FDeviceFrame& LocalData) = 0;

	//同步Server远端数据到本地
	virtual bool SynchronizationToLocal(int64& LastTicks, AllPlayerDevice& RemoteData) = 0;

	//设置需要录制的那台机的IP
	virtual void SetNeedRecordIP(const FString& RecordIP) = 0;
};