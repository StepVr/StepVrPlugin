#include "StepVrServerModule.h"
#include "Engine.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
//#include "UnrealNetwork.h"
//#include "SocketSubsystem.h"
//#include "IPAddress.h"

AllPlayerData LocalAllPlayerData = {};
PlayerDeviceInfo LocalPlayerData;


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

void FStepVrServer::IkinemaSendData(const IKinemaReplicateData& InData)
{
	LocalPlayerData.PlayerAddr = InData.PlayerID;
	LocalPlayerData.IkinemaInfo = InData;
}


void FStepVrServer::IkinemaGetData(uint32 InPlayerAddr, IKinemaReplicateData& InData)
{
	PlayerDeviceInfo* Tempplayer = LocalAllPlayerData.Find(InPlayerAddr);
	if (Tempplayer == nullptr)
	{
		return;
	}

	InData.SkeletionIDs.Empty(Tempplayer->IkinemaInfo.SkeletionIDs.Num());
	InData.SkeletonInfos.Empty(Tempplayer->IkinemaInfo.SkeletonInfos.Num());
	InData = Tempplayer->IkinemaInfo;
}

void FStepVrServer::StepVrSendData(uint32 InPlayerAddr, TMap<int32, FTransform>& InData)
{
	LocalPlayerData.PlayerAddr = InPlayerAddr;
	LocalPlayerData.StepVrDeviceInfo = InData;
}

void FStepVrServer::StepVrGetData(uint32 InPlayerAddr, TMap<int32, FTransform>& OutData)
{
	PlayerDeviceInfo* Tempplayer = LocalAllPlayerData.Find(InPlayerAddr);
	if (Tempplayer == nullptr)
	{
		return;
	}

	OutData = Tempplayer->StepVrDeviceInfo;
}

AllPlayerData& FStepVrServer::LockAllPlayerData()
{
	Section_AllPlayerData.Lock();

	return LocalAllPlayerData;
}

void FStepVrServer::UnLockAllPlayerData()
{
	Section_AllPlayerData.Unlock();
}
