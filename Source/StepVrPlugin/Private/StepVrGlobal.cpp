#include "StepVrGlobal.h"
#include "Engine.h"

#include "StepVrServerModule.h"
#include "StepVrBPLibrary.h"
#include "LocalDefine.h"
#include "StepVrPlugin.h"


/************************************************************************/
/* Global Data                                                                     */
/************************************************************************/
AllDevicesTrans	GLocalDevicesRT;
TMap<uint32, AllDevicesTrans> GReplicateDevicesRT;
FCriticalSection GReplicateSkeletonCS;
TAtomic<uint64> GUpdateReplicateSkeleton = 0;
TMap<uint32, AllSkeletonData> GReplicateSkeletonRT;

TArray<int32>	GNeedUpdateDevices = { StepVrDeviceID::DHead };

TSharedPtr<StepVrGlobal> StepVrGlobal::SingletonInstance = nullptr;


/************************************************************************/
/* Global Class                                                                     */
/************************************************************************/
StepVrGlobal::StepVrGlobal()
{
}

StepVrGlobal::~StepVrGlobal()
{
	CloseSDK();
}


StepVrGlobal* StepVrGlobal::GetInstance()
{
	if (SingletonInstance.IsValid())
	{
		return SingletonInstance.Get();
	}

	SingletonInstance = MakeShareable(new StepVrGlobal());
	return SingletonInstance.Get();
}
void StepVrGlobal::Shutdown()
{
	if (SingletonInstance.IsValid())
	{
		SingletonInstance.Reset();
	}
}

void StepVrGlobal::StartSDK()
{
	//加载本地SDK
	LoadSDK();

	//加载Server
	LoadServer();

	/**
	 * 注册开始帧，刷新数据
	 */
	EngineBeginFrameHandle = FCoreDelegates::OnBeginFrame.AddRaw(this, &StepVrGlobal::EngineBeginFrame);
	//PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddRaw(this, &StepVrGlobal::PostLoadMapWithWorld);
}

bool StepVrGlobal::ServerIsValid()
{
	return StepVrServer.IsValid();
}


bool StepVrGlobal::SDKIsValid()
{
	return StepVrManager.IsValid();
}

void StepVrGlobal::LoadServer()
{
	IStepvrServerModule* _Server = static_cast<IStepvrServerModule*>(FModuleManager::Get().GetModule(IStepvrServerModule::GetModularFeatureName()));

	if (_Server)
	{
		StepVrServer = _Server->CreateServer();

		if (StepVrServer.IsValid())
		{ 
			UE_LOG(LogStepVrPlugin, Warning, TEXT("StepVrServer Start Success"));
		}
		else 
		{ 
			UE_LOG(LogStepVrPlugin, Warning, TEXT("StepVrServer Start Faild"));
		}	
	}
}

void StepVrGlobal::LoadSDK()
{
	bool Success = false;
	FString Message;

	do
	{
		TArray<FString> DllPaths;
		FString Platform = PLATFORM_WIN64 ? "x64" : "x32";
		DllPaths.Add(FPaths::ProjectPluginsDir() + TEXT("StepVrPlugin/ThirdParty/lib/") + Platform);
		DllPaths.Add(FPaths::EnginePluginsDir() + TEXT("StepVrPlugin/ThirdParty/lib/") + Platform);
		DllPaths.Add(FPaths::EnginePluginsDir() + TEXT("Runtime/StepVrPlugin/ThirdParty/lib/") + Platform);

		for (int32 i = 0; i < DllPaths.Num(); i++)
		{
			FPlatformProcess::PushDllDirectory(*DllPaths[i]);
			DllHandle = FPlatformProcess::GetDllHandle(*(DllPaths[i] + "/StepVR.dll"));
			FPlatformProcess::PopDllDirectory(*DllPaths[i]);
			if (DllHandle != nullptr)
			{
				break;
			}
		}

		if (DllHandle == nullptr)
		{
			Message = "Load Dll Fail";
			break;
		}

		StepVrManager = MakeShareable(new StepVR::Manager());

		bool Flag = (StepVrManager->Start() == 0);
		if (!Flag)
		{
			Message = "Failed to connect server";
			break;
		}

		StepVR::StepVR_EnginAdaptor::MapCoordinate(StepVR::Vector3f(0, 0, 1), StepVR::Vector3f(-1, 0, 0), StepVR::Vector3f(0, 1, 0));
		StepVR::StepVR_EnginAdaptor::setEulerOrder(StepVR::EulerOrder_ZYX);

		Success = true;
	} while (0);

	if (Success)
	{
		UE_LOG(LogStepVrPlugin, Warning, TEXT("StepvrSDK Satrt Success"));
	}
	else
	{
		CloseSDK();

		UE_LOG(LogStepVrPlugin, Warning, TEXT("StepvrSDK Satrt Failed,Message:%s"), *Message);
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("StepVR", "StepVR", "{0}"), FText::FromString(Message)));
	}
}

void StepVrGlobal::CloseSDK()
{
	FCoreDelegates::OnBeginFrame.Remove(EngineBeginFrameHandle);
	//FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);
	if (DllHandle != nullptr)
	{
		FPlatformProcess::FreeDllHandle(DllHandle);
		DllHandle = nullptr;
	}
}

void StepVrGlobal::EngineBeginFrame()
{
	SCOPE_CYCLE_COUNTER(stat_EngineBeginFrame_tick);
	/**
	* 更新定位数据
	*/
	if (StepVrManager.IsValid())
	{
		StepVR::SingleNode Node = StepVrManager->GetFrame().GetSingleNode();

		for (auto DevID : GNeedUpdateDevices)
		{
			FTransform TempData;
			UStepVrBPLibrary::SVGetDeviceState(&Node, DevID, TempData);
		}
	}

	/**
	 * 更新同步数据
	 */
	if (StepVrServer.IsValid())
	{
		StepVrServer->SynchronizationStepVrData();
	}
}

void StepVrGlobal::PostLoadMapWithWorld(UWorld* UsingWorld)
{
	//CurUsingWorld = UsingWorld;
	//UNetDriver* Driver = CurUsingWorld->GetNetDriver();
	//if (Driver)
	//{
	//	ENetMode mode = Driver->GetNetMode();
	//	UE_LOG(LogTemp, Log, TEXT("%d"), mode);
	//}
}

UWorld* StepVrGlobal::GetWorld()
{
#if WITH_EDITOR
	if (GIsEditor)
	{
		return GWorld;
	}
#endif // WITH_EDITOR
	return GEngine->GetWorldContexts()[0].World();
}

StepVR::Manager* StepVrGlobal::GetStepVrManager()
{
	return StepVrManager.IsValid() ? StepVrManager.Get() : nullptr;
}


FStepVrServer* StepVrGlobal::GetStepVrServer()
{
	return StepVrServer.IsValid() ? StepVrServer.Get() : nullptr;
}