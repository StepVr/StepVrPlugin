#include "StepVrGlobal.h"
#include "LocalDefine.h"
#include "StepVrConfig.h"
#include "StepVrServerInterface.h"



#include "Engine/Engine.h"
#include "Misc/MessageDialog.h"
#include "Containers/Array.h"




TSharedPtr<StepVrGlobal> StepVrGlobal::SingletonInstance = nullptr;
StepVrGlobal::StepVrGlobal()
{
}

StepVrGlobal* StepVrGlobal::GetInstance()
{
	if (SingletonInstance.IsValid())
	{
		return SingletonInstance.Get();
	}

	SingletonInstance = MakeShareable(new StepVrGlobal());
	SingletonInstance->StartSDK();
	return SingletonInstance.Get();
}

void StepVrGlobal::Shutdown()
{
	if (SingletonInstance.IsValid())
	{
		SingletonInstance->CloseSDK();
		SingletonInstance.Reset();
	}
}

void StepVrGlobal::StartSDK()
{
	LoadSDK();

	LoadServer();

	//当前机器唯一标识
	FGuid NewGUID = FGuid::NewGuid();
	GameGUID = GetTypeHash(NewGUID);

	NeedUpdateDeviceID = { 
		StepVrDeviceID::DHead,
		StepVrDeviceID::DGun 
	};

	EngineBeginFrameHandle = FCoreDelegates::OnBeginFrame.AddRaw(this, &StepVrGlobal::EngineBeginFrame);
}

bool StepVrGlobal::SDKIsValid()
{
	return StepVrManager.IsValid();
}

uint32 StepVrGlobal::GetGUID()
{
	return GameGUID;
}

void StepVrGlobal::LoadServer()
{
	if (IStepvrServerModule::IsAvailable())
	{
		StepVrData = IStepvrServerModule::Get().CreateStepVrData();
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
		StepVR::StepVR_EnginAdaptor::MapCoordinate(StepVR::Vector3f(0, 0, 1), StepVR::Vector3f(-1, 0, 0), StepVR::Vector3f(0, 1, 0));
		StepVR::StepVR_EnginAdaptor::setEulerOrder(StepVR::EulerOrder_ZYX);

		Success = (StepVrManager->Start() == 0);
	} while (0);

	if (!Success)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("StepVR", "StepVR", "{0}"), FText::FromString("Failed To Connect MMAP")));
	}
}

void StepVrGlobal::CloseSDK()
{
	FCoreDelegates::OnBeginFrame.Remove(EngineBeginFrameHandle);

	if (DllHandle != nullptr)
	{
		FPlatformProcess::FreeDllHandle(DllHandle);
		DllHandle = nullptr;
	}
}

void StepVrGlobal::EngineBeginFrame()
{
#if SHOW_STATE
	SCOPE_CYCLE_COUNTER(STAT_StepVR_SVGlobal_UpdateFrame);
#endif

	//更新本机数据
	if (StepVrManager.IsValid())
	{
		StepVR::SingleNode Node = StepVrManager->GetFrame().GetSingleNode();

		for (auto DevID : NeedUpdateDeviceID)
		{
			UpdateDeviceState(&Node, DevID);
		}
	}
	
	if (StepVrData.IsValid())
	{
		//同步本机数据到Server
		StepVrData->SynchronizationToServer(GameGUID, GameLocalPlayer);

		//同步数据到Game
		StepVrData->SynchronizationToLocal(GameAllPlayer);
	}
}


void StepVrGlobal::UpdateDeviceState(StepVR::SingleNode* InSingleNode, int32 EquipId)
{
	FDeviceFrame* DeviceFrame = GameLocalPlayer.Find(EquipId);
	if (DeviceFrame == nullptr)
	{
		GameLocalPlayer.Add(EquipId, FDeviceFrame());
		return ;
	}

	if (!InSingleNode->IsHardWareLink(EquipId))
	{
		return;
	}

	static StepVR::Vector3f vec3;
	static StepVR::Vector4f vec4;
	static FTransform       Transform;

	//加速度角速度
	vec3 = InSingleNode->GetSpeedVec(SDKNODEID(EquipId));
	DeviceFrame->SetSpeed(FVector(vec3.x, vec3.y, vec3.z));
	vec3 = InSingleNode->GetSpeedAcc(SDKNODEID(EquipId));
	DeviceFrame->SetAcceleration(FVector(vec3.x, vec3.y, vec3.z));
	vec3 = InSingleNode->GetSpeedGyro(SDKNODEID(EquipId));
	DeviceFrame->SetPalstance(FVector(vec3.x, vec3.y, vec3.z));

	//定位
	vec3 = InSingleNode->GetPosition(SDKNODEID(EquipId));
	vec3 = StepVR::StepVR_EnginAdaptor::toUserPosition(vec3);
	//定位缩放
	Transform.SetLocation(FVector(vec3.x * 100, vec3.y * 100, vec3.z * 100) * ScaleTransform);

	vec4 = InSingleNode->GetQuaternion(SDKNODEID(EquipId));
	vec4 = StepVR::StepVR_EnginAdaptor::toUserQuat(vec4);
	if (EquipId == 6)
	{
		Transform.SetRotation(FQuat(vec4.y * -1, vec4.x, vec4.z, vec4.w));
	}
	else
	{
		Transform.SetRotation(FQuat(vec4.x, vec4.y, vec4.z, vec4.w));
	}

	DeviceFrame->SetTransform(Transform);
}

void StepVrGlobal::SetScaleTransform(float NewScale)
{
	ScaleTransform = NewScale;
}

void StepVrGlobal::AddDeviceID(int32 DeviceID)
{
	NeedUpdateDeviceID.AddUnique(DeviceID);
}

bool StepVrGlobal::GetDeviceTransform(SinglePlayer& OutData)
{
	OutData = GameLocalPlayer;
	return true;
}

bool StepVrGlobal::GetDeviceTransformImmediately(int32 DeviceID, FTransform& OutData)
{
	if (StepVrManager.IsValid())
	{
		StepVR::SingleNode Node = StepVrManager->GetFrame().GetSingleNode();

		UpdateDeviceState(&Node, DeviceID);
		if (auto Temp = GameLocalPlayer.Find(DeviceID))
		{
			Temp->GetTransform(OutData);
		}

		return true;
	}

	return false;
}

bool StepVrGlobal::GetRemoteDeviceTransform(uint32 GUID, SinglePlayer& OutData)
{
	if (auto Single = GameAllPlayer.Find(GUID))
	{
		OutData = *Single;
		return true;
	}

	return false;
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
	return StepVrManager.Get();
}


void StepVrGlobal::SetGameType(EStepGameType NewType, FString& NewServerIP)
{
	GameType = NewType;
	GameServerIP = NewServerIP;

	if (StepVrData.IsValid())
	{
		StepVrData->SetNewGameType(GameType, GameServerIP);
	}
}

EStepGameType StepVrGlobal::GetGameType()
{
	return GameType;
}
