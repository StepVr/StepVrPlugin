﻿// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Networking.h"
#include "CoreMinimal.h"
#include "StepVrAuxiliaryUDP.generated.h"


class FSocket;
class FUdpSocketReceiver;



//辅助软件消息类型
enum ESendType
{
//发送到Game
	Etype_None,
	EType_Connect,
	EType_DisConnect,

//从Game接收
	Etype_DeviceState,
	Etype_ControllState,
};


DECLARE_DELEGATE_OneParam(FAuxiliaryReceive, ESendType)



const int32 _TotalSize = 30;
const char _AppendChar[_TotalSize] = {};
static void MyStrSerialize(FArchive& Ar, FString& str)
{
	int32 strSize = str.GetCharArray().Num();
	if (strSize > _TotalSize)
	{
		Ar.Serialize((void*)StringCast<ANSICHAR>(str.GetCharArray().GetData(), strSize).Get(), _TotalSize);
	}
	else {
		Ar.Serialize((void*)StringCast<ANSICHAR>(str.GetCharArray().GetData(), strSize).Get(), sizeof(ANSICHAR) * strSize);
		Ar.Serialize((void*)_AppendChar, _TotalSize - strSize);
	}
}

/**
 * 发送当前设备状态
 */
USTRUCT(BlueprintType)
struct FAuxiliaryDevice
{
	GENERATED_USTRUCT_BODY()

	FVector OffSet;

	uint64 TotalFrames;

	float FrameTimes;

	bool bHead;
	FVector Head; 

	bool bGun;
	FVector Gun;
	
	friend FArchive& operator<< (FArchive& Ar, FAuxiliaryDevice& ArData)
	{
		Ar << ArData.TotalFrames;
		Ar << ArData.FrameTimes;
		Ar << ArData.OffSet;

		Ar << ArData.bHead;
		Ar << ArData.Head;

		Ar << ArData.bGun;
		Ar << ArData.Gun;
		return Ar;
	}
};


USTRUCT(BlueprintType)
struct FAuxiliaryControll
{
	GENERATED_USTRUCT_BODY()

	//控制的是谁
	FString PawnName;

	//人的位置
	FVector PawnLocation;

	//人的姿态
	FVector PawnRotator;

	//是否包含StepCamera
	bool	bHaveCamera;

	//当前相机的位置
	FVector CameraLocation;

	//相机的姿态
	FVector CameraRotator;

	//是否包含StepComponent
	bool	bHaveComponent;

	//是否包含动捕
	bool	bHaveMocap;

	friend FArchive& operator<< (FArchive& Ar, FAuxiliaryControll& ArData)
	{
		MyStrSerialize(Ar, ArData.PawnName);
		Ar << ArData.PawnLocation;
		Ar << ArData.PawnRotator;

		Ar << ArData.bHaveCamera;
		Ar << ArData.CameraLocation;
		Ar << ArData.CameraRotator;

		Ar << ArData.bHaveComponent;
		Ar << ArData.bHaveMocap;
		return Ar;
	}
};







class FStepVrAuxiliaryUDP
{
public:
	FStepVrAuxiliaryUDP();
	virtual~FStepVrAuxiliaryUDP();

	//开始服务
	void StartServer();
	void StopServer();

	ESendType GetSendType() { return CacheSendType; }


	//发送数据
	void SendDeviceData(FAuxiliaryDevice& InData);
	void SendControllData(FAuxiliaryControll& InData);

	/*发送数据的回调*/
	void CallStepVrReceive(const FArrayReaderPtr& InReaderPtr, const FIPv4Endpoint& InEndpoint);

	//回调代理
	FAuxiliaryReceive& GetAuxiliaryReceive();

protected:
	ESendType	CacheSendType = ESendType::Etype_None;

	TSharedPtr<FInternetAddr>	CacheRemoteAddr;

	FAuxiliaryReceive					m_AuxiliaryReceive;
	FSocket*							m_pSendScoket;
	TSharedPtr<FUdpSocketReceiver>		m_pReceiveSocket;
};