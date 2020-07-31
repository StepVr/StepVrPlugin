#include "StepVrGlobal.h"
#include "Engine.h"
#include "StepVrInput.h"
#include "StepVrConfig.h"
#include "StepVrData.h"

#include "LocalDefine.h"
#include "StepVrPlugin.h"


#include "Kismet/GameplayStatics.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "IHeadMountedDisplay.h"
#include "IXRTrackingSystem.h"
#include "IXRSystemAssets.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Engine/NetDriver.h"
#include "Engine/NetConnection.h"

/************************************************************************/
/* Global Data                                                                     */
/************************************************************************/
			
TSharedPtr<StepVrGlobal> StepVrGlobal::SingletonInstance = nullptr;



/************************************************************************/
/* Global Class                                                                     */
/************************************************************************/
StepVrGlobal::StepVrGlobal()
{
	GScaleTransform = 1.f;
	GNeedUpdateDevices = { StepVrDeviceID::DHead };
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

bool StepVrGlobal::GlobalIsValid()
{
	return SingletonInstance.IsValid();
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


	//同步ID
	UStepSetting* Config = UStepSetting::Instance();
	if (Config)
	{
		ReplicateID = Config->ReplicateDeviceID;
	}

	/**
	 * 组要更新的设备ID
	 */
	NeedUpdateDevices.Add(StepVrDeviceID::DLeftController);
	NeedUpdateDevices.Add(StepVrDeviceID::DRightController);
	NeedUpdateDevices.Add(StepVrDeviceID::DGun);
	NeedUpdateDevices.Add(StepVrDeviceID::DHead);
	NeedUpdateDevices.Add(StepVrDeviceID::DHMD);


	/**
	 * 注册开始帧，刷新数据
	 */
	EngineBeginFrameHandle = FCoreDelegates::OnBeginFrame.AddRaw(this, &StepVrGlobal::EngineBeginFrame);
}

bool StepVrGlobal::SDKIsValid()
{
	return StepVrManager.IsValid();
}

void StepVrGlobal::LoadServer()
{
	//创建服务器数据对象

	//StepVrServerData = MakeShareable(new FStepVrData());


	//创建接收数据
	//StepVrReplicateData = MakeShareable(new FStepFrames());
	//GStepFrames = StepVrReplicateData.Get();
	
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
		if (!Success)
		{
			Message = "Failed to connect server";
		}
	} while (0);

	if (Success)
	{
		UE_LOG(LogStepVrPlugin, Warning, TEXT("StepvrSDK Satrt Success"));
	}
	else
	{
		//CloseSDK();

		UE_LOG(LogStepVrPlugin, Warning, TEXT("StepvrSDK Satrt Failed,Message:%s"), *Message);
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("StepVR", "StepVR", "{0}"), FText::FromString(Message)));
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
	/**
	* 更新定位数据
	*/
	if (StepVrManager.IsValid())
	{
		StepVR::SingleNode Node = StepVrManager->GetFrame().GetSingleNode();

		for (auto DevID : GNeedUpdateDevices)
		{
			FTransform TempData;
			SVGetDeviceState(&Node, DevID, TempData);
		}
	}

	UE_LOG(LogTemp,Log,TEXT("PLatform Time : %f"), FPlatformTime::Seconds());

	/**
	 * 更新同步数据
	 */
	if (STEPVR_Data_IsValid )
	{
		STEPVR_Data->SynchronizationStepVrData();

	}


	if (!STEPVR_FRAME_IsValid)
	{
		return;
	}

	/**
	* 同步定位数据
	*/
	if (STEPVR_Data_IsValid)
	{
		TMap<int32, FTransform> SendData;
		FTransform TempPtr;
		for (auto DevID : ReplicateID)
		{
			SVGetDeviceStateWithID(DevID, TempPtr);

			SendData.Add(DevID, TempPtr);
		}
		if (IsValidPlayerAddr())
		{
			STEPVR_Data->ReceiveStepVrData(GetPlayerAddr(), SendData);
		}
	}

}

void StepVrGlobal::SVGetDeviceState(StepVR::SingleNode* InSingleNode, int32 EquipId, FTransform& Transform)
{
	if (!InSingleNode->IsHardWareLink(EquipId))
	{
		return;
	}

	static StepVR::Vector3f vec3;
	vec3 = InSingleNode->GetPosition(SDKNODEID(EquipId));
	vec3 = StepVR::StepVR_EnginAdaptor::toUserPosition(vec3);
	Transform.SetLocation(FVector(vec3.x * 100, vec3.y * 100, vec3.z * 100));

	static StepVR::Vector4f vec4;
	vec4 = InSingleNode->GetQuaternion(SDKNODEID(EquipId));
	if (EquipId == 6)
	{
		vec4 = StepVR::StepVR_EnginAdaptor::toUserQuat(vec4);
		Transform.SetRotation(FQuat(vec4.y*-1, vec4.x, vec4.z, vec4.w));

		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected())
		{
			FRotator	S_QTemp;
			FVector		S_VTemp;
			UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(S_QTemp, S_VTemp);
			GLocalDevicesRT.FindOrAdd(StepVrDeviceID::DHMD) = FTransform(
				S_QTemp.Quaternion(),
				Transform.GetLocation() - S_VTemp);
		}
	}
	else
	{
		vec4 = StepVR::StepVR_EnginAdaptor::toUserQuat(vec4);
		Transform.SetRotation(FQuat(vec4.x, vec4.y, vec4.z, vec4.w));
	}

	{
		//数据进行缩放
		FVector ScaleLocaltion = Transform.GetLocation() * GScaleTransform;
		Transform.SetLocation(ScaleLocaltion);
	}

	GLocalDevicesRT.FindOrAdd(EquipId) = Transform;
}

void StepVrGlobal::SVGetDeviceStateWithID(int32 DeviceID, FTransform& Transform)
{
	FTransform* _trans = GLocalDevicesRT.Find(DeviceID);
	if (_trans)
	{
		Transform = *_trans;
		return;
	}

	if (GNeedUpdateDevices.Find(DeviceID) == INDEX_NONE)
	{
		GNeedUpdateDevices.Add(DeviceID);
	}
}

AllDevicesData StepVrGlobal::GetAllDevicesData()
{
	return GLocalDevicesRT;
}

void StepVrGlobal::SetScaleTransform(float ScaleTransform)
{
	GScaleTransform = ScaleTransform;
}

bool StepVrGlobal::IsValidPlayerAddr()
{
	return PlayerID > 0;
}

uint32 StepVrGlobal::GetPlayerAddr()
{
	return PlayerID;
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

FString StepVrGlobal::GetLocalAddressStr()
{
	if (STEPVR_Data_IsValid)
	{

		return STEPVR_Data->GetLocalAddressStr();
	}
	return "";

}

StepVR::Manager* StepVrGlobal::GetStepVrManager()
{
	return StepVrManager.IsValid() ? StepVrManager.Get() : nullptr;
}

FStepFrames* StepVrGlobal::GetStepVrReplicateFrame()
{
	return StepVrReplicateData.IsValid() ? StepVrReplicateData.Get() : nullptr;
}



void StepVrGlobal::GetLastReplicateDeviceData(uint32 lPlayerID, int32 DeviceID, FTransform& Data)
{
	//最新数据
	if (STEPVR_Data_IsValid)
	{
		auto TempPlayer = STEPVR_Data->AllPlayerInfo.Find(lPlayerID);
		if (TempPlayer == nullptr)
		{
			return;
		}

		auto NewDeviceData = (*TempPlayer).CurrnetDeviceData.Find(DeviceID);
		if (NewDeviceData == nullptr)
		{
			return;
		}

		Data = *NewDeviceData;
	}
	
}



/****************************FStepFrams***********************************/
/*                                                                      */
/************************************************************************/
FStepFrames::FStepFrames() :
	IndexCurHead(0),
	IndexHeadContain(0),
	NewFrame(nullptr)
{

}

FStepAllPlayerFrame* FStepFrames::GetHeadContainer()
{
	IndexHeadContain = (IndexCurHead + 1) % StepFramsMax;
	return &CacheFrames[IndexHeadContain];
}

void FStepFrames::FlushHeadContain()
{
	IndexCurHead = IndexHeadContain;
	NewFrame = &CacheFrames[IndexCurHead];
}


/************************************************************************/
/* Time                                                                     */
/************************************************************************/
#include "Windows/AllowWindowsPlatformTypes.h"  
#include <chrono>
using namespace std;
using namespace std::chrono;


class StepTimeWin : public StepTime
{
public:
	StepTimeWin() :
		m_begin(high_resolution_clock::now())
	{

	}
	virtual ~StepTimeWin()
	{
	}

	virtual void ResetTime() override
	{
		//m_begin = high_resolution_clock::now();
	}


	virtual double IntervalAndReset() override
	{
		double __LastTime = Interval_MS();
		double __Interval = __LastTime - LastTime;

		LastTime = __LastTime;
		return __Interval;
	}


	virtual double Interval_MS() override
	{
		return duration_cast<chrono::microseconds>(high_resolution_clock::now() - m_begin).count() / 1000.f;
	}
	virtual int64 Interval_Micro() override
	{
		return duration_cast<chrono::microseconds>(high_resolution_clock::now() - m_begin).count();
	}

protected:
	time_point<high_resolution_clock> m_begin;

	double LastTime = 0.f;
};

TSharedPtr<StepTime> StepTime::GetTime()
{
	return MakeShareable(new StepTimeWin());
}


#include "Windows/HideWindowsPlatformTypes.h"  