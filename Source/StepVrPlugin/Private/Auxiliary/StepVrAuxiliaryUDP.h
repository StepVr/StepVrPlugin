// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

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
};


DECLARE_DELEGATE_OneParam(FAuxiliaryReceive, ESendType)

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