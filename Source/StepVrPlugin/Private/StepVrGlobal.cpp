#include "StepVrGlobal.h"
#include "LocalDefine.h"
#include "StepVrConfig.h"
#include "StepVrServerInterface.h"
#include "Auxiliary/StepVrAuxiliaryCollection.h"


#include "Engine/Engine.h"
#include "Misc/MessageDialog.h"





TSharedPtr<StepVrGlobal> StepVrGlobal::SingletonInstance = nullptr;
bool _AlreadyCreate = false;


StepVrGlobal::StepVrGlobal()
{
	NeedUpdateDeviceID = {
				StepVrDeviceID::DHead,
				StepVrDeviceID::DGun
				};
}

StepVrGlobal* StepVrGlobal::GetInstance()
{
	if ((SingletonInstance.IsValid() == false) &&
		(_AlreadyCreate == false))
	{
		_AlreadyCreate = true;
		SingletonInstance = MakeShareable(new StepVrGlobal());
		SingletonInstance->StartSDK();
	}

	if (SingletonInstance.IsValid())
	{
		return SingletonInstance.Get();
	}

	return nullptr;
}

bool StepVrGlobal::CheckValidInstance()
{
	return SingletonInstance.IsValid();
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

	//辅助软件
	StepVrAuxiliaryCollection = MakeShared<FStepVrAuxiliaryCollection>();
	StepVrAuxiliaryCollection->StartCollection();

	//当前机器唯一标识
	FGuid NewGUID = FGuid::NewGuid();
	GameGUID = GetTypeHash(NewGUID);

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
		StepVrData->Init();
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

		StepVrManager = MakeShared<StepVR::Manager>();
		StepVR::StepVR_EnginAdaptor::MapCoordinate(StepVR::Vector3f(0, 0, 1), StepVR::Vector3f(-1, 0, 0), StepVR::Vector3f(0, 1, 0));
		StepVR::StepVR_EnginAdaptor::setEulerOrder(StepVR::EulerOrder_ZYX);
		
		Success = (StepVrManager->Start() == 0);
		if (Success)
		{
			StepVrManagerComplieTime = StepVrManager->GetServerCompileTime();
			StepVrManagerVersion = StepVrManager->GetServerVersion();
		}
	} while (0);

	if (Success)
	{
		UE_LOG(LogStepVrPlugin, Log, TEXT("MMAP ComplieTime : %s"), *StepVrManagerComplieTime);
		UE_LOG(LogStepVrPlugin, Log, TEXT("MMAP Version : %s"), *StepVrManagerVersion);
	}
	else
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
	SCOPE_CYCLE_COUNTER(Stat_StepVrGlobal_UpdateFrame);
#endif

	RefreshFrame(GameDevicesFrame);
	
	if (StepVrData.IsValid())
	{
		//同步数据到Game
		StepVrData->SynchronizationToLocal(GameAllPlayerLastTicks, GameAllPlayer);
	}

	if (StepVrAuxiliaryCollection.IsValid())
	{
		StepVrAuxiliaryCollection->EngineBeginFrame();
	}
}

void StepVrGlobal::DataLerp(FDeviceData& inputData, FDeviceData& outputData)
{
	//FTransform TempData;

	////第一帧接收到数据，同步两个时间戳
	//if (isFirst)
	//{
	//	//记录对齐时候的两个时间戳
	//	FirstTime1 = inputData.GetMMAPTicks();
	//	FirstTime2 = inputData.TemporaryTimestamp;

	//	//输出时间戳均为0
	//	outputData.Transform = inputData.Transform;
	//	outputData.TemporaryTimestamp = 0;

	//	//缓存时间戳
	//	OutPutDataBuff = outputData;
	//	return;
	//}

	//if ((inputData.TemporaryTimestamp - FirstTime2) < DelatTime)
	//{
	//	//接收到的数据的时间小于延迟时间，输出仍为第一帧，用于产生延迟
	//	outputData.Transform = inputData.Transform;
	//	outputData.TemporaryTimestamp = 0;

	//	OutPutDataBuff = outputData;
	//}
	//else
	//{
	//	int64 OutTime = inputData.TemporaryTimestamp - FirstTime2 - DelatTime;  //经过延迟输出的时间戳
	//	int64 InTime = inputData.GetMMAPTicks() - FirstTime1;					//接收到的网络数据的时间戳

	//	//根据上一帧输出数据的时间戳，本次输出应有的时间戳，以及接收到网络数据的时间戳计算插值比例（因为延迟，接收到的时间戳肯定大于上一帧输出时间戳）
	//	double k = (OutTime - OutPutDataBuff.GetMMAPTicks()) / (InTime - OutPutDataBuff.GetMMAPTicks());
	//	
	//	//插值
	//	FVector Plerp = OutPutDataBuff.Transform.GetLocation() + k * (inputData.Transform.GetLocation() - outputData.Transform.GetLocation());

	//	outputData.Transform.SetLocation(Plerp);
	//	outputData.TemporaryTimestamp = OutTime;

	//	OutPutDataBuff = outputData;
	//}
}

void StepVrGlobal::SetScaleTransform(const FVector& NewScale)
{
	ScaleTransform = NewScale;
}

FVector StepVrGlobal::GetScaleTransform()
{
	return ScaleTransform;
}

void StepVrGlobal::SetOffsetTransform(const FVector& NewOffset)
{
	OffsetTransform = NewOffset;
}

FVector StepVrGlobal::GetOffsetTransform()
{
	return OffsetTransform;
}

int32 StepVrGlobal::SetKartMaxSpeed(int32 speed)
{
	int32 success = -1;

	if (StepVrManager.IsValid())
	{
		success = StepVrManager->SetKartMaxSpeed(speed);
	}
	return success;
}

int32 StepVrGlobal::SetKartBrake(bool bSet)
{
	int32 success = -1;

	if (StepVrManager.IsValid())
	{
		success = StepVrManager->SetKartBrake(bSet);
	}
	return success;
}

int32 StepVrGlobal::SetKartEnableReverse(bool bSet)
{
	int32 success = -1;

	if (StepVrManager.IsValid())
	{
		success = StepVrManager->SetKartEnableReverse(bSet);
	}
	return success;
}

int32 StepVrGlobal::SetKartForward(bool bForward)
{
	int32 success = -1;

	if (StepVrManager.IsValid())
	{
		success = StepVrManager->SetKartForward(bForward);
	}
	return success;
}

FDeviceFrame& StepVrGlobal::GetDeviceFrame()
{
	return GameDevicesFrame;
}

bool StepVrGlobal::GetDeviceTransform(int32 DeviceID, FTransform& OutData)
{
	if (GameDevicesFrame.HasDevice(DeviceID))
	{
		OutData = GameDevicesFrame.GetDeviceRef(DeviceID).GetTransform();
		return true;
	}

	NeedUpdateDeviceID.AddUnique(DeviceID);
	return false;
}

bool StepVrGlobal::GetRemoteDeviceFrame(uint32 GUID, FDeviceFrame& OutData)
{
	if (auto Single = GameAllPlayer.Find(GUID))
	{
		OutData = *Single;
		return true;
	}

	return false;
}


FStepCommandDelegate& StepVrGlobal::GetCommandDelegate()
{
	return CommandDelegate;
}

void StepVrGlobal::ExecCommand(ECommandState NewCommand, int32 Values)
{
	if (CommandDelegate.IsBound())
	{
		CommandDelegate.Broadcast(NewCommand, Values);
	}
}

void StepVrGlobal::ExecCommand(ECommandState NewCommand, const FString& Values)
{
	if (CommandDelegateStr.IsBound())
	{
		CommandDelegateStr.Broadcast(NewCommand, Values);
	}
}

FStepCommandDelegateStr& StepVrGlobal::GetCommandDelegateStr()
{
	return CommandDelegateStr;
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


FString StepVrGlobal::GetManagerCompileTime()
{
	return StepVrManagerComplieTime;
}

FString StepVrGlobal::GetManagerCompileVersion()
{
	return StepVrManagerVersion;
}

void StepVrGlobal::SetGameType(EStepGameType NewType, FString& NewServerIP)
{
	GameType = NewType;
	GameServerIP = NewServerIP;

	if (StepVrData.IsValid())
	{
		StepVrData->SetNewGameInfo(GameType, GameGUID, GameServerIP);
	}
}

EStepGameType StepVrGlobal::GetGameType()
{
	return GameType;
}

void StepVrGlobal::SetRecordPCIP(const FString& PCIP)
{
	if (StepVrData.IsValid())
	{
		StepVrData->SetNeedRecordIP(PCIP);
	}
}

void StepVrGlobal::RefreshFrame(FDeviceFrame& outFrame)
{
	static StepVR::Vector3f vec3;
	static StepVR::Vector4f vec4;
	static FTransform       Transform;

	FScopeLock Lock(&StepVrManagerCritical);

	//更新本机数据
	if (StepVrManager.IsValid())
	{
		StepVR::SingleNode InSingleNode = StepVrManager->GetFrame().GetSingleNode();

		for (uint8 DevID : NeedUpdateDeviceID)
		{
			FDeviceData& OutDevice = outFrame.GetDeviceRef(DevID);

			bool isLink = InSingleNode.IsHardWareLink(DevID);
			OutDevice.SetLink(isLink);

			if (!isLink)
			{
				continue;
			}

			//加速度角速度
			vec3 = InSingleNode.GetSpeedVec(SDKNODEID(DevID));
			OutDevice.SetSpeed(FVector(vec3.x, vec3.y, vec3.z));
			vec3 = InSingleNode.GetSpeedAcc(SDKNODEID(DevID));
			OutDevice.SetAcceleration(FVector(vec3.x, vec3.y, vec3.z));
			vec3 = InSingleNode.GetSpeedGyro(SDKNODEID(DevID));
			OutDevice.SetPalstance(FVector(vec3.x, vec3.y, vec3.z));

			//定位
			vec3 = InSingleNode.GetPosition(SDKNODEID(DevID));
			vec3.x = vec3.x - (OffsetTransform.X/100);
			vec3.y = vec3.y - (OffsetTransform.Y/100);
			vec3.z = vec3.z - (OffsetTransform.Z/100);

			vec3 = StepVR::StepVR_EnginAdaptor::toUserPosition(vec3);

			//定位缩放
			Transform.SetLocation(FVector(vec3.x, vec3.y, vec3.z) * 100 * ScaleTransform);

			vec4 = InSingleNode.GetQuaternion(SDKNODEID(DevID));
			vec4 = StepVR::StepVR_EnginAdaptor::toUserQuat(vec4);
			
			//头部姿态单独处理
			if (DevID == 6)
			{
				Transform.SetRotation(FQuat(vec4.y * -1, vec4.x, vec4.z, vec4.w));
			}
			else
			{
				Transform.SetRotation(FQuat(vec4.x, vec4.y, vec4.z, vec4.w));
			}

			OutDevice.SetTransform(Transform);
		}
	}
}
