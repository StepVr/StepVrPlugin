#include "StepVrAuxiliaryUDP.h"
#include "LocalDefine.h"

#include "Engine.h"
#include "IPAddress.h"
#include "SocketSubsystem.h"






FStepVrAuxiliaryUDP::FStepVrAuxiliaryUDP():
m_pSendScoket(nullptr),
m_pReceiveSocket(nullptr)
{

}

FStepVrAuxiliaryUDP::~FStepVrAuxiliaryUDP()
{
	StopServer();
}

void FStepVrAuxiliaryUDP::StartServer()
{
	FString SockeName = TEXT("StepVrAuxiliaryUDP");

	m_pSendScoket = FUdpSocketBuilder(SockeName)
		.AsNonBlocking()
		.AsReusable()
		.WithSendBufferSize(1024)
		.WithReceiveBufferSize(1024)
		.BoundToPort(AUXILIARY_PORT)
		.Build();

	if (m_pSendScoket == nullptr)
	{
		return;
	}

	m_pSendScoket->Listen(AUXILIARY_LISTEN_MAX);
	m_pReceiveSocket = MakeShareable(new FUdpSocketReceiver(m_pSendScoket, FTimespan::FromMilliseconds(1000), TEXT("StepVrReceive")));
	m_pReceiveSocket->OnDataReceived().BindRaw(this, &FStepVrAuxiliaryUDP::CallStepVrReceive);
	m_pReceiveSocket->Start();
}


void FStepVrAuxiliaryUDP::StopServer()
{
	if (m_pReceiveSocket.IsValid())
	{
		m_pReceiveSocket->Stop();
		m_pReceiveSocket.Reset();
	}
	if (m_pSendScoket)
	{
		m_pSendScoket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(m_pSendScoket);
		m_pSendScoket = nullptr;
	}
}

void FStepVrAuxiliaryUDP::SendDeviceData(FAuxiliaryDevice& InData)
{
	int32 MessgeType = ESendType::Etype_DeviceState;
	
	FArrayWriter ArrayWriter;
	ArrayWriter << MessgeType;
	ArrayWriter << InData;

	int32 SendNums = 0;
	if (CacheRemoteAddr.IsValid() && m_pSendScoket)
	{
		m_pSendScoket->SendTo(ArrayWriter.GetData(), ArrayWriter.Num(), SendNums, *CacheRemoteAddr);
	}
}

void FStepVrAuxiliaryUDP::SendControllData(FAuxiliaryControll& InData)
{
	int32 MessgeType = ESendType::Etype_ControllState;

	FArrayWriter ArrayWriter;
	ArrayWriter << MessgeType;
	ArrayWriter << InData;

	int32 SendNums = 0;
	if (CacheRemoteAddr.IsValid() && m_pSendScoket)
	{
		m_pSendScoket->SendTo(ArrayWriter.GetData(), ArrayWriter.Num(), SendNums, *CacheRemoteAddr);
	}
}

void FStepVrAuxiliaryUDP::SendStartState(FAuxiliaryStartState& InData)
{
	int32 MessgeType = ESendType::Etype_StartState;

	FArrayWriter ArrayWriter;
	ArrayWriter << MessgeType;
	ArrayWriter << InData;

	int32 SendNums = 0;
	if (CacheRemoteAddr.IsValid() && m_pSendScoket)
	{
		m_pSendScoket->SendTo(ArrayWriter.GetData(), ArrayWriter.Num(), SendNums, *CacheRemoteAddr);
	}
}

void FStepVrAuxiliaryUDP::CallStepVrReceive(const FArrayReaderPtr& InReaderPtr, const FIPv4Endpoint& InEndpoint)
{
	//数据反序列化
	int32 ReceiveType;
	*InReaderPtr << ReceiveType;

	//指令类型
	if (ReceiveType == ESendType::EType_Connect)
	{
		CacheSendType = (ESendType)ReceiveType;
		CacheRemoteAddr = InEndpoint.ToInternetAddr();
	}
	if (ReceiveType == ESendType::EType_DisConnect)
	{
		CacheSendType = (ESendType)ReceiveType;
		CacheRemoteAddr.Reset();
	}

	//同步线程
	AsyncTask(ENamedThreads::GameThread,[&,ReceiveType]()
	{
		if (m_AuxiliaryReceive.IsBound())
		{
			m_AuxiliaryReceive.Execute((ESendType)ReceiveType);
		}
	});
}

FAuxiliaryReceive& FStepVrAuxiliaryUDP::GetAuxiliaryReceive()
{
	return m_AuxiliaryReceive;
}

//
//void FStepVrAuxiliaryUDP::DelegateTick(float time)
//{
//	if (bIsStartListen == false)
//	{
//		return;
//	}
//
//#if SHOW_STATE
//	SCOPE_CYCLE_COUNTER(Stat_StepVrData_Send);
//#endif
//
//	//获取地址
//	TArray<TSharedPtr<FInternetAddr>> Addrs;
//	m_StepVrData->GetSendAddr(Addrs);
//
//#if SHOW_STATE
//	SET_DWORD_STAT(Statr_StepVrData_RemoteNums, Addrs.Num());
//#endif
//
//	//接收数据
//	FArrayWriter SendData;
//	m_StepVrData->GetSendData(SendData);
//
//	int32 SendNums;
//	for (auto& Addr : Addrs)
//	{
//		if (Addr.IsValid())
//		{
//			m_pSendScoket->SendTo(SendData.GetData(), SendData.Num(), SendNums, *Addr);
//		}
//	}
//
//	//处理录制数据
//	m_StepVrData->DealRecordData();
//}