#include "StepVrComponent.h"
#include "StepVrInput.h"
#include "StepVrGlobal.h"

#include "StepVrConfig.h"
#include "StepVrCameraComponent.h"


#include "Kismet/GameplayStatics.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "IHeadMountedDisplay.h"
#include "IXRTrackingSystem.h"
#include "IXRSystemAssets.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Engine/NetDriver.h"
#include "Engine/NetConnection.h"





static bool GIsResetHMD = false;
UStepVrComponent::UStepVrComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	
	PlayerIP = TEXT("");

	SetIsReplicatedByDefault(true);
}

void UStepVrComponent::ResetHMD()
{
	GIsResetHMD = false;
	ResetHMDAuto();
}

FString UStepVrComponent::GetLocalIP()
{
	//return FStepVrServer::GetLocalAddressStr();
	return "";
}

void UStepVrComponent::OnRep_PlayerIP()
{
	//PlayerID = GetTypeHash(PlayerIP);
}

void UStepVrComponent::SetPlayerAddrOnServer_Implementation(const FString& LocalIP)
{
	PlayerIP = LocalIP;
	OnRep_PlayerIP();
}

bool UStepVrComponent::SetPlayerAddrOnServer_Validate(const FString& LocalIP)
{
	return true;
}

void UStepVrComponent::DeviceTransform(int32 DeviceID, FTransform& Trans)
{
#if SHOW_STATE
	SCOPE_CYCLE_COUNTER(stat_DeviceTransform_tick);
#endif
	
	//获取同步数据
	FTransform* CacheData = LastDeviceData.Find(DeviceID);
	if (CacheData == nullptr)
	{
		LastDeviceData.Add(DeviceID, FTransform());
		CacheData = LastDeviceData.Find(DeviceID);
	}

	if (IsLocalControlled())
	{
		auto Temp = GLocalDevicesRT.Find(DeviceID);
		if (Temp)
		{
			Trans = *Temp;
		}
		else
		{
			if (GNeedUpdateDevices.Find(DeviceID) == INDEX_NONE)
			{
				GNeedUpdateDevices.Add(DeviceID);
			}
		}
	}
	else
	{
		if (STEPVE_GLOBAL_IsValid)
		{
			if (GStepFrames && STEPVR_GLOBAL->IsValidPlayerAddr())
			{
				GStepFrames->GetLastReplicateDeviceData(STEPVR_GLOBAL->PlayerID, DeviceID, *CacheData);
				Trans = *CacheData;
			}
		}
	}
}

void UStepVrComponent::SetGameType(FGameType type, FString ServerIP)
{
	if (STEPVE_GLOBAL_IsValid)
	{
		//STEPVR_GLOBAL->SetGameModeTypeGlobal((EGameModeType)type);
		if (type == FGameType::GameClient)
		{
			//STEPVR_GLOBAL->UpdateServerIP(ServerIP);
		}
	}
}

void UStepVrComponent::ResetHMDDirection()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn == nullptr)
	{
		return;
	}

	Pawn->SetActorRotation(FRotator::ZeroRotator);

    ResetHMDFinal();

	GIsResetHMD = true;
}

void UStepVrComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UStepVrComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


void UStepVrComponent::RegistInputComponent()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn && Pawn->InputComponent)
	{
		Pawn->InputComponent->BindKey(EKeys::R, EInputEvent::IE_Pressed, this, &UStepVrComponent::ResetHMD);
	}
}

void UStepVrComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
#if SHOW_STATE
	SCOPE_CYCLE_COUNTER(stat_Componment_tick);
#endif

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsLocalControlled())
	{
		AfterinitializeLocalControlled();

		TickLocal();
	}

}

void UStepVrComponent::InitializeComponent()
{
	Super::InitializeComponent();
	ResetYaw = GameUseType == FGameUseType::UseType_VR ? 90.f : -90.f;
}

void UStepVrComponent::AfterinitializeLocalControlled()
{
	if (bAlreadyInitializeLocal)
	{
		return;
	}
	bAlreadyInitializeLocal = true;

	/**
	 * Stepcamera 配置
	 */
	auto StepCamera = Cast<UStepVrCameraComponent>(GetOwner()->GetComponentByClass(UStepVrCameraComponent::StaticClass()));
	if (StepCamera)
	{
		int32 CameraID = GameUseType == FGameUseType::UseType_Cave ? 198 : 6;
		StepCamera->SetCameraInfo(CameraID);
	}

	/**
	* 注册需要更新定位的标准件
	*/
	if (!STEPVE_GLOBAL_IsValid)
	{
		return;
	}
	for (auto DevID : STEPVR_GLOBAL->NeedUpdateDevices)
	{
		if (DevID != StepVrDeviceID::DHMD)
		{
			GNeedUpdateDevices.AddUnique(DevID);
		}
	}
	
	/**
	 * 编辑器模式重新校准
	 */
	if (GetWorld()->IsEditorWorld())
	{
		GIsResetHMD = false;
	}

	/**
	 * 校准头盔，注册校准事件
	 */
	ResetHMDAuto();
	RegistInputComponent();

	/**
	 * 同步定位数据
	 */
	FString Addr = GetLocalIP();
	SetPlayerAddrOnServer(Addr);
}

void UStepVrComponent::ResetHMDFinal()
{
	FTransform TempData;
	//UStepVrBPLibrary::SVGetDeviceStateWithID(StepVrDeviceID::DHead, TempData);
	float TempYaw = TempData.Rotator().Yaw + ResetYaw;

	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(TempYaw);
	UE_LOG(LogStepVrPlugin, Log, TEXT("Reset HMD Yaw : %f"), TempYaw);
}


void UStepVrComponent::ResetOculusRealTime()
{
	FRotator	S_QTemp;
	FVector		S_VTemp;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(S_QTemp, S_VTemp);
	
	
	HMDPitch = S_QTemp.Pitch;

	double Maxrate;

	/*存储4帧数据*/
	if (!bIsStart)
	{
		Tnum = 0;
		Cnum = 0;
		bIsReset = false;
		bIsStart = true;
		bIsCorrect = false;
	}
	if (Tnum == 1)
	{
		HMDYaw[0] = S_QTemp.Yaw;
		IMUYaw[0] = CurrentNodeState.FHead.Rotator().Yaw;
		Tnum++;
	}
	else if (Tnum > 1)
	{
		for (int i = 1; i < Yawn; i++)
		{
			HMDYaw[Yawn - i] = HMDYaw[Yawn - 1 - i];
			IMUYaw[Yawn - i] = IMUYaw[Yawn - 1 - i];
		}
		HMDYaw[0] = S_QTemp.Yaw;
		IMUYaw[0] = CurrentNodeState.FHead.Rotator().Yaw;
		HMDrate[0] = HMDYaw[1] - HMDYaw[0];
		Maxrate = fabs(HMDrate[0]);
		float Maxyaw = IMUYaw[0];
		float Minyaw = IMUYaw[0];
		for (int i = 1; i < Yawn - 1; i++)
		{
			HMDrate[i] = HMDYaw[i + 1] - HMDYaw[i];
			Maxrate = fabs(HMDrate[i]) > Maxrate ? fabs(HMDrate[i]) : Maxrate;
			Maxrate = Maxrate > 180 ? 360 - Maxrate : Maxrate;
			Maxyaw = Maxyaw > IMUYaw[i] ? Maxyaw : IMUYaw[i];
			Minyaw = Minyaw < IMUYaw[i] ? Minyaw : IMUYaw[i];
		}
		double DecYaw = (Maxyaw - Minyaw) > 180 ? 360 - (Maxyaw - Minyaw) : (Maxyaw - Minyaw);
		if (Maxrate < 1 && fabs(DecYaw) < 2)
		{
			bIsReset = false;
			bIsCorrect = false;
		}
		else if (Cnum < 200)
		{
			bIsReset = true;
			bIsCorrect = false; 

			Cnum = 0;
		}
		else
		{
			bIsReset = false;
		}
	}

	if (!bIsReset)
	{
		//校准
		if (fabs(HMDPitch) > 60)
		{
			return;
		}

		float Yaw = CurrentNodeState.FHead.Rotator().Yaw;
		float DevYaw;

		if (Tnum == 0)
		{
			NewYaw = Yaw + ResetYaw;
			NewYaw = NewYaw > 180 ? NewYaw - 360 : NewYaw;
			UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(NewYaw);
			Tnum = 1;
		}
		else
		{
			float TempIMUYaw = Yaw + ResetYaw;
			TempIMUYaw = TempIMUYaw > 180 ? TempIMUYaw - 360 : TempIMUYaw;

			DevYaw = TempIMUYaw - HMDYaw[0];
			DevYaw = DevYaw > 180 ? DevYaw - 360 : DevYaw;
			DevYaw = DevYaw < -180 ? DevYaw + 360 : DevYaw;
			if (!bIsCorrect && Cnum < 200)
			{
				if (fabs(DevYaw) > 5)
				{
					Cnum = fabs(DevYaw) / 0.01;
					bIsCorrect = true;
				}
				else if (Cnum < 200)
				{
					return;
				}
			}

			DevYaw = DevYaw > 0 ? 0.01 : -0.01;
			NewYaw = NewYaw + DevYaw;
			Cnum--;
			s_bIsResetOculus = false;
		}

		if (!s_bIsResetOculus)
		{
			//UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(NewYaw);
			FQuat BaseOrientation;
			BaseOrientation = (NewYaw != 0.0f) ? FRotator(0, -NewYaw, 0).Quaternion() : FQuat::Identity;
			GEngine->XRSystem->SetBaseOrientation(BaseOrientation);
			//UE_LOG(LogStepVrPlugin, Warning, TEXT("Stepvr ResetHMD Yaw:%f,%f,%f"), Yaw, HMDYaw[0], NewYaw);

			s_bIsResetOculus = true;
		}
	}
}


void UStepVrComponent::ResetHMDAuto()
{
	if (GIsResetHMD)
	{
		return;
	}

	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		ResetHMDDirection();
	}
}

void UStepVrComponent::TickLocal()
{
	if (!STEPVE_GLOBAL_IsValid)
	{
		return;
	}

	//更新每个玩家的Device
	for (auto DevID : STEPVR_GLOBAL->NeedUpdateDevices)
	{
		FTransform& TempPtr = GetDeviceDataPtr(DevID);
		STEPVR_GLOBAL->SVGetDeviceStateWithID(DevID, TempPtr);
	}
}



FTransform& UStepVrComponent::GetDeviceDataPtr(int32 DeviceID)
{
	switch (DeviceID)
	{
		case StepVrDeviceID::DHead:
		{
			return CurrentNodeState.FHead;
		}
		case StepVrDeviceID::DGun:
		{
			return CurrentNodeState.FGun;
		}
		case StepVrDeviceID::DLeftController:
		{
			return CurrentNodeState.FDLeftController;
		}
		case StepVrDeviceID::DRightController:
		{
			return CurrentNodeState.FRightController;
		}
		case StepVrDeviceID::DHMD:
		{
			return CurrentNodeState.FHeadForHMD;
		}
	}

	static FTransform GTransform;
	return GTransform;
}

bool UStepVrComponent::IsLocalControlled()
{
	APawn* LocalPawn = Cast<APawn>(GetOwner());
	if (LocalPawn == nullptr)
	{
		return false;
	}

	return LocalPawn->IsLocallyControlled();
}


void UStepVrComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStepVrComponent, PlayerIP);
}