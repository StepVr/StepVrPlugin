#include "StepVrComponent.h"
#include "StepVrBPLibrary.h"
#include "StepVrInput.h"
#include "StepVrGlobal.h"
#include "StepVrServerModule.h"

#include "Kismet/GameplayStatics.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "IHeadMountedDisplay.h"
#include "IXRTrackingSystem.h"
#include "IXRSystemAssets.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Engine/NetDriver.h"
#include "Engine/NetConnection.h"
#include "StepVrConfig.h"



static bool GIsResetOculus = false;
UStepVrComponent::UStepVrComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	bReplicates = true;

	PlayerIP = TEXT("");
}

void UStepVrComponent::ResetHMD()
{
	GIsResetOculus = false;
	ResetHMDAuto();
}
void UStepVrComponent::ToggleResetType()
{
	switch (ResetHMDType)
	{
		case FResetHMDType::ResetHMD_RealTime:
		{
			ResetHMDType = FResetHMDType::ResetHMD_BeginPlay;
			ResetHMD();
		}
			break;
		case FResetHMDType::ResetHMD_BeginPlay:
		{
			ResetHMDType = FResetHMDType::ResetHMD_RealTime;
		}
			break;
	}
}

void UStepVrComponent::DeviceTransform(int32 DeviceID, FTransform& Trans)
{
	if (bIsLocalControll)
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
		auto TempPlayer = GReplicateDevicesRT.Find(PlayerAddr);
		if (TempPlayer)
		{
			auto TempDevice = (*TempPlayer).Find(DeviceID);
			if (TempDevice)
			{
				Trans = *TempDevice;
			}
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

	switch (HMDType)
	{
	case FHMDType::HMD_Oculus:
	{
		ResetOculusRif();
	}
		break;
	case FHMDType::HMD_Windows:
		break;
	case FHMDType::HMD_InValid:
		break;
	}

	GIsResetOculus = true;
}

void UStepVrComponent::BeginPlay()
{
	Super::BeginPlay();

	//同步ID
	UStepSetting* Config = StepVrGlobal::GetInstance()->GetStepSetting();
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
}

void UStepVrComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


void UStepVrComponent::RegistInputComponent()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn == nullptr)
	{
		return;
	}

	if (Pawn->InputComponent)
	{
		Pawn->InputComponent->BindKey(EKeys::R, EInputEvent::IE_Pressed, this, &UStepVrComponent::ResetHMD);
		Pawn->InputComponent->BindKey(EKeys::T, EInputEvent::IE_Pressed, this, &UStepVrComponent::ToggleResetType);
	}
}

void UStepVrComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	INC_DWORD_STAT(StepVrComponent_TickComponent_Count);
	SCOPE_CYCLE_COUNTER(StepVrComponent_TickComponent_State);

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (!bInitializeLocal)
	{
		InitializeLocalControlled();
		return;
	}

 	if (bIsLocalControll/* || TickType == ELevelTick::LEVELTICK_ViewportsOnly*/)
 	{
 		TickLocal();
 	}
	/*	else
	   {
		   TickSimulate();
	   }*/
}

void UStepVrComponent::InitializeComponent()
{
	Super::InitializeComponent();
	ResetYaw = GameUseType == FGameUseType::UseType_Normal ? 90.f : -90.f;
}

void UStepVrComponent::AfterinitializeLocalControlled()
{
	if (!bIsLocalControll)
	{
		return;
	}

	/**
	* 注册需要更新定位的标准件
	*/
	for (auto DevID : NeedUpdateDevices)
	{
		if (DevID != StepVrDeviceID::DHMD)
		{
			GNeedUpdateDevices.AddUnique(DevID);
		}
	}

	/**
	 * 检测头盔
	 */
	do 
	{
		if (!GEngine->XRSystem.IsValid())
		{
			break;
		}

		FName HMDName = GEngine->XRSystem->GetSystemName();
		if (HMDName.IsEqual("OculusHMD"))
		{
			HMDType = FHMDType::HMD_Oculus;
			break;
		}

		if (HMDName.IsEqual("WindowsMixedRealityHMD"))
		{
			HMDType = FHMDType::HMD_Windows;
			break;
		}
	} while (0);
	
	/**
	 * 编辑器模式重新校准
	 */
	if (GetWorld()->IsEditorWorld())
	{
		GIsResetOculus = false;
	}

	/**
	 * 校准头盔，注册校准事件
	 */
	ResetHMDAuto();
	RegistInputComponent();

	/**
	 * 同步定位数据
	 */
	FString Addr = FStepVrServer::GetLocalAddressStr();
	SetPlayerAddrOnServer(Addr);
}

void UStepVrComponent::ResetOculusRif()
{
	FTransform TempData;
	UStepVrBPLibrary::SVGetDeviceStateWithID(StepVrDeviceID::DHead, TempData);
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
	if (GIsResetOculus)
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
	if (!STEPVR_FRAME_IsValid)
	{
		return;
	}

	//更新每个玩家的Device
	for (auto DevID : NeedUpdateDevices)
	{
		FTransform& TempPtr = GetDeviceDataPtr(DevID);
		UStepVrBPLibrary::SVGetDeviceStateWithID(DevID, TempPtr);
	}


	if (STEPVR_SERVER_IsValid)
	{
		/**
		 * 同步定位数据
		 */
		TMap<int32, FTransform> SendData;
		FTransform TempPtr;
		for (auto DevID : ReplicateID)
		{
			UStepVrBPLibrary::SVGetDeviceStateWithID(DevID,TempPtr);
			SendData.Add(DevID, TempPtr);
		}
		STEPVR_SERVER->StepVrSendData(PlayerAddr, SendData);
	}

	/**
	 * 实时校准
	 */
	if (ResetHMDType == FResetHMDType::ResetHMD_RealTime)
	{
		switch (HMDType)
		{
		case  FHMDType::HMD_Oculus:
		{
			ResetOculusRealTime();
		}
		break;
		case  FHMDType::HMD_Windows:
		{

		}
		break;
		}
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

bool UStepVrComponent::IsValidPlayerAddr()
{
	return PlayerAddr > 1;
}

uint32 UStepVrComponent::GetPlayerAddr()
{
	return PlayerAddr;
}

bool UStepVrComponent::IsInitialization()
{
	return bInitializeLocal;
}

bool UStepVrComponent::IsLocalControlled()
{
	return bIsLocalControll;
}

bool UStepVrComponent::InitializeLocalControlled()
{
	do 
	{
		AController* ControllerLocal = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (ControllerLocal == nullptr)
		{
			break;
		}

		APawn* LocalPawn = ControllerLocal->GetPawn();
		if (LocalPawn == nullptr)
		{
			break;
		}

		APawn* Pawn = Cast<APawn>(GetOwner());
		if (Pawn == nullptr)
		{
			//基类非Pawn下的组件无效
			bInitializeLocal = true;
			break;
		}

		bIsLocalControll = LocalPawn == Pawn;
		bInitializeLocal = true;

		AfterinitializeLocalControlled();
	} while (0);

	return bInitializeLocal;
}

void UStepVrComponent::SetPlayerAddrOnServer_Implementation(const FString& LocalIP)
{
	PlayerIP = LocalIP;
	PlayerAddr = GetTypeHash(LocalIP);
}
bool UStepVrComponent::SetPlayerAddrOnServer_Validate(const FString& LocalIP)
{
	return true;
}
void UStepVrComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStepVrComponent, PlayerAddr);
	DOREPLIFETIME(UStepVrComponent, PlayerIP);
}