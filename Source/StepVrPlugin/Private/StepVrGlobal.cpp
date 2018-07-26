#include "StepVrGlobal.h"
#include "StepVrPlugin.h"
#include "StepVrServerModule.h"
#include "LocalDefine.h"

#include "IPluginManager.h"
#include "Misc/MessageDialog.h"

TSharedPtr<StepVrGlobal> StepVrGlobal::SingletonInstance = nullptr;

StepVrGlobal::StepVrGlobal()
{
	UE_LOG(LogStepVrPlugin, Warning, TEXT("StepVrGlobal Start"));

	/** Default DeviceID */
	ReplicateDevicesID.Add(StepVrDeviceID::DHead);
	ReplicateDevicesID.Add(StepVrDeviceID::DGun);

	//加载本地SDK
	LoadSDK();

	//加载Server
	LoadServer();	
}

StepVrGlobal::~StepVrGlobal()
{
	CloseSDK();
	UE_LOG(LogStepVrPlugin, Warning, TEXT("StepVrGlobal END"));
}

bool StepVrGlobal::ServerIsRun()
{
	if (!SingletonInstance.IsValid())
	{
		return false;
	}

	return SingletonInstance->ServerIsValid();
}

void StepVrGlobal::LoadServer()
{
	IModularFeatures& _ModularFeatures = IModularFeatures::Get();

	TArray<IStepvrServerModule*> _Servers = _ModularFeatures.GetModularFeatureImplementations<IStepvrServerModule>(IStepvrServerModule::GetModularFeatureName());

	if (_Servers.IsValidIndex(0))
	{
		StepVrServer = _Servers[0]->CreateServer();

		if (StepVrServer.IsValid())
		{ 
			/** Add Replicate Device */
			StepVrServer->SetReplciatedDeviceID(ReplicateDevicesID);
			UE_LOG(LogStepVrPlugin, Warning, TEXT("Load Server Success")); 
		}
		else 
		{ UE_LOG(LogStepVrPlugin, Error, TEXT("Load Server Faild")); }	
	}
}

void StepVrGlobal::LoadSDK()
{
	bool Success = false;
	FString Message;
	do
	{
		/** Load Dll */
		FString Platform = PLATFORM_WIN64 ? "x64" : "x32";
		FName   PluginName = FStepVrPluginModule::GetModularFeatureName();
		FString SDllPath = IPluginManager::Get().
			FindPlugin(*PluginName.ToString())->GetBaseDir() + "/ThirdParty/lib/" + Platform;

		FPlatformProcess::PushDllDirectory(*SDllPath);
		DllHandle = FPlatformProcess::GetDllHandle(*(SDllPath + "/StepVR.dll"));
		FPlatformProcess::PopDllDirectory(*SDllPath);

		if (!DllHandle)
		{
			Message = "Load Dll Fail";
			break;
		}


		/** Create StepVrManager */
		StepVrManager = MakeShareable(new StepVR::Manager());

		//SDK Start
		bool Flag = false; //StepVrManager->Start() == 0;
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
		UE_LOG(LogStepVrPlugin, Warning, TEXT("StepvrSDK Run Success"));
	}
	else
	{
		CloseSDK();

		UE_LOG(LogStepVrPlugin, Error, TEXT("StepvrSDK Open Failed,Message:%s"), *Message);
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("StepVR", "StepVR", "{0}"), FText::FromString(Message)));
	}
}

void StepVrGlobal::CloseSDK()
{
	if (DllHandle!=nullptr)
	{
		FPlatformProcess::FreeDllHandle(DllHandle);
		DllHandle = nullptr;
	}

	StepVrManager.Reset();
}


void StepVrGlobal::SetReplicatedDevices(TArray<int32> Devices)
{
	ReplicateDevicesID.Empty();
	ReplicateDevicesID = Devices;

	if (!StepVrServer.IsValid()) { return; }
	StepVrServer->SetReplciatedDeviceID(Devices);
}

TArray<int32> StepVrGlobal::GetReplicatedDevices()
{
	return ReplicateDevicesID;
}


StepVR::Manager* StepVrGlobal::GetStepVrManager()
{
	return StepVrManager.IsValid() ? StepVrManager.Get() : nullptr;
}


FStepVrServer* StepVrGlobal::GetStepVrServer()
{
	return StepVrServer.IsValid() ? StepVrServer.Get() : nullptr;
}

void StepVrGlobal::CreateInstance()
{
	if (SingletonInstance.IsValid()) 
	{ 
		return; 
	}

	SingletonInstance = MakeShareable(new StepVrGlobal());
}

void StepVrGlobal::Shutdown()
{
	if (SingletonInstance.IsValid())
	{
		SingletonInstance.Reset();
	}		
}

StepVrGlobal * StepVrGlobal::Get()
{
	if (!SingletonInstance.IsValid())
	{
		return nullptr;
	}
	return SingletonInstance.Get();
}
