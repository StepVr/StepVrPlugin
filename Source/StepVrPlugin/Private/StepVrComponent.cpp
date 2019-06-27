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



static bool GIsResetOculus = false;
UStepVrComponent::UStepVrComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	bReplicates = true;
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

void UStepVrComponent::ResetHMDDirection()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn == nullptr)
	{
		return;
	}

	Pawn->SetActorRotation(FRotator::ZeroRotator);

	if (GEngine->XRSystem.IsValid())
	{
		FName HMDName = GEngine->XRSystem->GetSystemName();

		FName Oculus = TEXT("OculusHMD");
		FName Windows = TEXT("WindowsMixedRealityHMD");
		if (HMDName.IsEqual(Oculus))
		{
			ResetOculusRif();
		}
		else if(HMDName.IsEqual(Windows))
		{
			ResetOculusRif();
		}
	}
	

	GIsResetOculus = true;
}

void UStepVrComponent::BeginPlay()
{
	Super::BeginPlay();

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
 	else
 	{
 		TickSimulate();
 	}
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
	uint32 Addr = FStepVrServer::GetLocalAddress();
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


void UStepVrComponent::ResetHMDRealTime()
{
	if (ResetHMDType != FResetHMDType::ResetHMD_RealTime)
	{
		return;
	}

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

void UStepVrComponent::TickSimulate()
{
	if (!STEPVR_SERVER_IsValid) 
	{ 
		return; 
	}

	if (!IsValidPlayerAddr())
	{
		return;
	}

	TMap<int32, FTransform> GetData;
	STEPVR_SERVER->StepVrGetData(PlayerAddr, GetData);

	//更新每个玩家的Device
	for (auto DevID : NeedUpdateDevices)
	{
		FTransform& TempPtr = GetDeviceDataPtr(DevID);
		FTransform* TempData = GetData.Find(DevID);
		if (TempData)
		{
			TempPtr = *TempData;
		}
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
		* 更新公用Device
		*/
		TMap<int32, FTransform> GlobalDevices;
		for (auto DevID : GNeedUpdateGlobalDevices)
		{
			FTransform TempData;
			UStepVrBPLibrary::SVGetDeviceStateWithID(DevID, TempData);
			GlobalDevices.Add(DevID, TempData);
		}

		/**
		 * 更新连接状态
		 */
		do 
		{
			UWorld* CurWorld = GetWorld();
			if (CurWorld == nullptr)
			{
				break;
			}

			UNetDriver* Driver = CurWorld->NetDriver;
			if (Driver == nullptr)
			{
				STEPVR_SERVER->SetGameModeType(EStandAlone);
				break;
			}

			//客户端
			UNetConnection* ServerConnection = Driver->ServerConnection;
			if (ServerConnection && (!ServerIP.Equals(ServerConnection->URL.Host)))
			{
				ServerIP = ServerConnection->URL.Host;
				STEPVR_SERVER->UpdateClientState(ServerIP);
				break;
			}

			//服务器
			TArray<class UNetConnection*> ClientConnections = Driver->ClientConnections;
			if (RemotAddrIP.Num() != ClientConnections.Num())
			{
				RemotAddrIP.Empty(ClientConnections.Num());
				for (UNetConnection* Temp : ClientConnections)
				{
					if (Temp)
					{
						RemotAddrIP.Add(Temp->URL.Host);
					}
				}

				STEPVR_SERVER->UpdateServerState(RemotAddrIP);
			}
		} while (0);
		
		/**
		 * 同步定位数据
		 */
		TMap<int32, FTransform> SendData;
		for (auto DevID : NeedUpdateDevices)
		{
			FTransform& TempPtr = GetDeviceDataPtr(DevID);
			SendData.Add(DevID, TempPtr);
		}
		STEPVR_SERVER->StepVrSendData(PlayerAddr, SendData, GlobalDevices);
	}

	//Reset RealTime
	ResetHMDRealTime();
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

void UStepVrComponent::SetPlayerAddrOnServer_Implementation(const uint32 InAddr)
{
	PlayerAddr = InAddr;
}
bool UStepVrComponent::SetPlayerAddrOnServer_Validate(const uint32 InAddr)
{
	return true;
}
void UStepVrComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStepVrComponent, PlayerAddr);
}