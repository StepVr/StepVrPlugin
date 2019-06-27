#include "StepVrServerModule.h"
#include "Engine.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
//#include "UnrealNetwork.h"
//#include "SocketSubsystem.h"
//#include "IPAddress.h"

AllPlayerData LocalAllPlayerData = {};

//所有玩家
static AllPlayerData GAllPlayerData;
uint32 FStepVrServer::GetLocalAddress()
{
	bool CanBind = false;
	TSharedRef<FInternetAddr> LocalIp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, CanBind);
	if (!LocalIp->IsValid())
	{
		return 0;
	}

	FString Addr = LocalIp->ToString(false);

	return GetTypeHash(Addr);
}

void FStepVrServer::SetGameModeType(EGameModeType InGameModeType)
{
	if (GameModeType != InGameModeType)
	{
		FScopeLock Lock(&Section_GameModeType);
		GameModeType = InGameModeType;
	}
}

void FStepVrServer::UpdateServerState(TArray<FString>& InClientsIP)
{
	FScopeLock Lock(&Section_GameModeType);
	GameModeType = InClientsIP.Num() > 0 ? EServer : EStandAlone;
	ClientsIP = InClientsIP;
}

void FStepVrServer::UpdateClientState(FString& InServerIP)
{
	FScopeLock Lock(&Section_GameModeType);
	GameModeType = InServerIP.IsEmpty() ? EStandAlone : EClient;
	ServerIP = InServerIP;
}

//void FStepVrServer::IkinemaSendData(const IKinemaReplicateData& InData)
//{
//	LocalPlayerData.PlayerAddr = InData.PlayerID;
//	LocalPlayerData.IkinemaInfo = InData;
//}
//
//
//void FStepVrServer::IkinemaGetData(uint32 InPlayerAddr, IKinemaReplicateData& InData)
//{
//	PlayerDeviceInfo* Tempplayer = LocalAllPlayerData.Find(InPlayerAddr);
//	if (Tempplayer == nullptr)
//	{
//		return;
//	}
//
//	InData.SkeletionIDs.Empty(Tempplayer->IkinemaInfo.SkeletionIDs.Num());
//	InData.SkeletonInfos.Empty(Tempplayer->IkinemaInfo.SkeletonInfos.Num());
//	InData = Tempplayer->IkinemaInfo;
//}

void FStepVrServer::SynchronizationStepVrData()
{
	FScopeLock Lock(&Section_AllPlayerData);

	GAllPlayerData = RemotePlayerData;
}

void FStepVrServer::StepVrSendData(uint32 InPlayerAddr, TMap<int32, FTransform>& InPlayerData, TMap<int32, FTransform>& InGlobalData)
{
	FScopeLock Lock(&Section_AllPlayerData);
	LocalPlayerData.PlayerAddr = InPlayerAddr;
	LocalPlayerData.StepVrDeviceInfo = InPlayerData;
}

void FStepVrServer::StepVrGetData(uint32 InPlayerAddr, TMap<int32, FTransform>& OutData)
{
	//只能在Game线程调用，无需加锁
	PlayerDeviceInfo* Tempplayer = GAllPlayerData.Find(InPlayerAddr);
	if (Tempplayer == nullptr)
	{
		return;
	}

	OutData = Tempplayer->StepVrDeviceInfo;
}