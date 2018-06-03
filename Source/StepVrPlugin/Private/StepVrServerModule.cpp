#include "StepVrPluginPrivatePCH.h"
#include "StepVrServerModule.h"

void ReplciateComponment::ReceiveRemoteData(TMap<int32, FTransform>& DeviceInfo)
{
	//if (GWorld)
	//{
	//	LastReplicateTime = GWorld->TimeSeconds;
	//}
	
	RemoteData = DeviceInfo;
}

void ReplciateComponment::GetRemoteData(int32 DeviceID, FTransform & data)
{
	auto temp = RemoteData.Find(DeviceID);
	if (temp)
	{
		data = *temp;
	}
}
