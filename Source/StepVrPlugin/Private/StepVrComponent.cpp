#include "StepVrComponent.h"
#include "Engine.h"

#include "StepVrBPLibrary.h"
#include "StepVrInput.h"
#include "StepVrGlobal.h"
#include "StepVrServerModule.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "IHeadMountedDisplay.h"
#include "IXRTrackingSystem.h"
#include "IXRSystemAssets.h"
#include "Net/UnrealNetwork.h"


bool UStepVrComponent::bGlobleIsReset = false;
UStepVrComponent::UStepVrComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	bReplicates = true;

	bIsLocalControll = false;
	bIsCheckLoal = false;
}

void UStepVrComponent::ResetHMD()
{
	bGlobleIsReset = false;
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
void UStepVrComponent::ResetControllPawnRotation()
{
	if (bGlobleIsReset)
	{
		return;
	}

	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn == nullptr)
	{
		return;
	}

	Pawn->SetActorRotation(FRotator::ZeroRotator);

	ResetOculusRif();

	bGlobleIsReset = true;
}

void UStepVrComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerBeginPlay();
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
	else
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UStepVrComponent::RegistInputComponent, 0.2f, false);
	}
}

void UStepVrComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//是否初始化contrller
	if (!IsLocalControlled())
	{
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

void UStepVrComponent::OwnerBeginPlay()
{
	if (!IsLocalControlled())
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UStepVrComponent::OwnerBeginPlay, 0.5f, false);
	}

	if (!bIsLocalControll)
	{
		return;
	}

	if (STEPVR_SERVER_IsValid)
	{
		uint32 Addr = STEPVR_SERVER->GetLocalAddress();
		SetPlayerAddrOnServer(Addr);
	}
	else
	{
		SetPlayerAddrOnServer(1);
	}

	if (ResetHMDType == FResetHMDType::ResetHMD_BeginPlay)
	{
		ResetHMDAuto();
	}

	RegistInputComponent();


}

void UStepVrComponent::ResetOculusRif()
{
	StepVR::SingleNode Node = STEPVR_FRAME->GetFrame().GetSingleNode();
	UStepVrBPLibrary::SVGetDeviceStateWithID(&Node, StepVrDeviceID::DHead, CurrentNodeState.FHead);

	float TempYaw = CurrentNodeState.FHead.Rotator().Yaw + ResetYaw;

	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(TempYaw);
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

			DevYaw = TempIMUYaw - HMDYaw[2];
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
			UE_LOG(LogStepVrPlugin, Warning, TEXT("Stepvr ResetHMD Yaw:%f,%f,%f"), Yaw, HMDYaw[0], NewYaw);

			s_bIsResetOculus = true;
		}
	}
}


void UStepVrComponent::ResetHMDAuto()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected() &&
		UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (GetWorld()->IsEditorWorld())
		{
			bGlobleIsReset = false;
		}
		ResetControllPawnRotation();
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UStepVrComponent::ResetHMDAuto, 0.2f, false);
	}
}


//void UStepVrComponent::UpdateTimer()
//{
//	StepVR::Frame tmp = STEPVR_FRAME->GetFrame();
//
//	UStepVrBPLibrary::SVGetDeviceStateWithID(&tmp, StepVrDeviceID::DHead, CurrentNodeState.FHead);
//
//	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected())
//	{
//		FRotator	S_QTemp;
//		FVector		S_VTemp;
//		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(S_QTemp, S_VTemp);
//
//		CurrentNodeState.FHeadForOculus.SetLocation(CurrentNodeState.FHead.GetLocation() - S_VTemp);
//		CurrentNodeState.FHeadForOculus.SetRotation(S_QTemp.Quaternion());
//
//		SetRelativeLocation(CurrentNodeState.FHeadForOculus.GetLocation());
//		MarkRenderTransformDirty();
//	}
//}

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

	FTransform* TempData = GetData.Find(StepVrDeviceID::DHead);
	if (TempData)
	{
		CurrentNodeState.FHead = *TempData;
	}
	TempData = GetData.Find(StepVrDeviceID::DOculusHead);
	if (TempData)
	{
		CurrentNodeState.FHeadForOculus = *TempData;
	}
	TempData = GetData.Find(StepVrDeviceID::DGun);
	if (TempData)
	{
		CurrentNodeState.FGun = *TempData;
	}
	TempData = GetData.Find(StepVrDeviceID::DLeftController);
	if (TempData)
	{
		CurrentNodeState.FDLeftController = *TempData;
	}
	TempData = GetData.Find(StepVrDeviceID::DRightController);
	if (TempData)
	{
		CurrentNodeState.FRightController = *TempData;
	}
}

void UStepVrComponent::TickLocal()
{
	if (!STEPVR_FRAME_IsValid) 
	{ 
		return; 
	}

	//更新标准件
	{
		//一帧数据
		StepVR::SingleNode Node = STEPVR_FRAME->GetFrame().GetSingleNode();

		UStepVrBPLibrary::SVGetDeviceStateWithID(&Node, StepVrDeviceID::DHead, CurrentNodeState.FHead);
		UStepVrBPLibrary::SVGetDeviceStateWithID(&Node, StepVrDeviceID::DGun, CurrentNodeState.FGun);
		UStepVrBPLibrary::SVGetDeviceStateWithID(&Node, StepVrDeviceID::DLeftController, CurrentNodeState.FDLeftController);
		UStepVrBPLibrary::SVGetDeviceStateWithID(&Node, StepVrDeviceID::DRightController, CurrentNodeState.FRightController);
		UStepVrBPLibrary::SVGetDeviceStateWithID(&Node, StepVrDeviceID::DLeftFoot, CurrentNodeState.FDLeftFoot);
		UStepVrBPLibrary::SVGetDeviceStateWithID(&Node, StepVrDeviceID::DRightFoot, CurrentNodeState.FRightFoot);

		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected())
		{
			FRotator	S_QTemp;
			FVector		S_VTemp;
			UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(S_QTemp, S_VTemp);

			CurrentNodeState.FHeadForOculus.SetLocation(CurrentNodeState.FHead.GetLocation() - S_VTemp);
			CurrentNodeState.FHeadForOculus.SetRotation(S_QTemp.Quaternion());
			S_mStepVrDeviceState.FindOrAdd(StepVrDeviceID::DOculusHead) = CurrentNodeState.FHeadForOculus;
		}
	}

	//同步数据
	{
		do 
		{
			if (!STEPVR_SERVER_IsValid)
			{
				break;
			}
			if (!IsValidPlayerAddr())
			{
				break;
			}
			TMap<int32, FTransform> SendData;
			SendData.Add(StepVrDeviceID::DHead, CurrentNodeState.FHead);
			SendData.Add(StepVrDeviceID::DOculusHead, CurrentNodeState.FHeadForOculus);
			SendData.Add(StepVrDeviceID::DGun, CurrentNodeState.FGun);
			SendData.Add(StepVrDeviceID::DLeftController, CurrentNodeState.FDLeftController);
			SendData.Add(StepVrDeviceID::DRightController, CurrentNodeState.FRightController);
			STEPVR_SERVER->StepVrSendData(PlayerAddr, SendData);
		} while (0);
	}

	//Reset RealTime
	ResetHMDRealTime();
}

bool UStepVrComponent::IsValidPlayerAddr()
{
	return PlayerAddr > 1;
}

bool UStepVrComponent::IsLocalControlled()
{
	if (bIsCheckLoal)
	{
		return bIsCheckLoal;
	}

	do 
	{
		APawn* Pawn = Cast<APawn>(GetOwner());
		if (Pawn == nullptr)
		{
			break;
		}

		AController* Controller = Pawn->GetController();
		if (Controller == nullptr)
		{
			break;
		}

		AController* ControllerLocal = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (ControllerLocal == nullptr)
		{
			break;
		}

		bIsLocalControll = ControllerLocal == Controller;
		bIsCheckLoal = true;

	} while (0);

	return bIsCheckLoal;
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