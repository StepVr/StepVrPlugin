#include "StepVrServerModule.h"
#include "StepVrGlobal.h"

#include "Engine.h"
#include "IPAddress.h"
#include "SocketSubsystem.h"


//所有玩家
static FString GLocaclIP = "";
static uint32 GLocaclIPValue = 0;

FString FStepVrServer::GetLocalAddressStr()
{
	do
	{
		if (!GLocaclIP.IsEmpty())
		{
			break;
		}

		bool CanBind = false;
		auto OnlinePtr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		if (OnlinePtr == nullptr)
		{
			break;
		}

		TSharedRef<FInternetAddr> LocalIp = OnlinePtr->GetLocalHostAddr(*GLog, CanBind);
		if (!LocalIp->IsValid())
		{
			break;
		}

		GLocaclIP = LocalIp->ToString(false);
	} while (0);

	return GLocaclIP;
}

EGameModeType FStepVrServer::GetGameModeType()
{
	return GameModeType;
}

void FStepVrServer::SetGameModeType(EGameModeType InGameModeType)
{
	if (GameModeType != InGameModeType)
	{
		FScopeLock Lock(&Section_GameModeType);
		GameModeType = InGameModeType;
	}
}

void FStepVrServer::UpdateServerIP(const FString& InServerIP)
{
	FScopeLock Lock(&Section_GameModeType);
	ServerIP = InServerIP;
}

bool FStepVrServer::SynchronizationStepVrData(FStepAllPlayerFrame* NewFrame)
{
	//时间校验
	float LastTime = mLastReceiveTime.Load();
	if (LastTime > GLastReceiveTime)
	{
		//更新时间
		GLastReceiveTime = LastTime;
		NewFrame->TimeStemp = LastTime;

		FScopeLock Lock(&Section_AllPlayerData);
		FScopeLock Lock1(&GReplicateSkeletonCS);
		for (auto& Pair : mRemotePlayerData)
		{
			NewFrame->AllPlayerInfo.FindOrAdd(Pair.Key).CurDeviceData = Pair.Value.StepVrDeviceInfo;
			NewFrame->AllPlayerInfo.FindOrAdd(Pair.Key).CurMocapData = Pair.Value.StepMocapInfo;
			
			//GReplicateDevicesRT.FindOrAdd(Pair.Key) = Pair.Value.StepVrDeviceInfo;
			//GReplicateSkeletonRT.FindOrAdd(Pair.Key) = Pair.Value.StepMocapInfo;
			//GUpdateReplicateSkeleton = GUpdateReplicateSkeleton + 1;
		}

		return true;
	}

	return false;
}

void FStepVrServer::StepMocapSendData(const TArray<FTransform>& InMocapData)
{
	FScopeLock Lock(&Section_AllPlayerData);
	mLocalPlayerData.StepMocapInfo = InMocapData;
}

void FStepVrServer::StepVrSendData(uint32 InPlayerAddr, TMap<int32, FTransform>& InPlayerData)
{
	FScopeLock Lock(&Section_AllPlayerData);
	mLocalPlayerData.PlayerAddr = InPlayerAddr;
	mLocalPlayerData.StepVrDeviceInfo = InPlayerData;
}




/**********************数据同步***************************************/
/*                                                                      */
/************************************************************************/
 
//void FPlayerInfo::GetPlayerData(uint32 PlayerAddr, FPlayerInfo& OutData)
//{
//	FScopeLock Lock(CriticalSection);
//
//	OutData = this;
//}
//
//void FPlayerInfo::SetNewDeviceData(uint32 PlayerAddr, const TMap<int32, FTransform>& NewData)
//{
//
//}
//
//void FPlayerInfo::SetNewMocapData(uint32 PlayerAddr, const TArray<FTransform>& NewData)
//{
//
//}
