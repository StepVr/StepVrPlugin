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
 * 定位键当前帧
 */
class STEPVRPLUGIN_API FDeviceFrame
{
public:
	FDeviceFrame();

	//临时时间戳
	int64 TemporaryTimestamp;

	//本机 从MMAP获取数据的时间间隔
	int64 GetInterval();

	//从MMAP刷新数据的时间戳
	int64 GetMMAPTicks();
	
	void SetTransform(FTransform& InData);
	void GetTransform(FTransform& OutData) const;

	void SetSpeed(FVector Indata);
	void SetAcceleration(FVector InData);
	void SetPalstance(FVector InData);

	friend FArchive& operator<< (FArchive& Ar, FDeviceFrame& ArData)
	{
		Ar << ArData.MMAPTicks;
		Ar << ArData.Transform;
		//Ar << ArData.Speed;
		//Ar << ArData.Acceleration;
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
 * 动补当前帧 
 */
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



typedef TMap<uint8,FDeviceFrame>	SinglePlayer;
typedef TMap<uint32,SinglePlayer>	AllPlayer;
typedef TMap<uint32, FMocapFrame>	AllPlayerMocap;


/**
 * StepVR 数据类
 */
class STEPVRPLUGIN_API FStepVrDataInterface
{
public:
	friend class StepVrGlobal;

	virtual ~FStepVrDataInterface() {}

private:
	//设置当前游戏状态
	virtual void SetNewGameType(EStepGameType NewType, const FString& NewServerIP) = 0;

	//同步本地数据到Server
	virtual void SynchronizationToServer(uint32 PlayerGUID, const SinglePlayer& LocalData) = 0;

	//同步Server远端数据到本地
	virtual void SynchronizationToLocal(AllPlayer& RemoteData) = 0;
};