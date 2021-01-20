#include "StepVrComponent.h"
#include "StepVrGlobal.h"
#include "StepVrDataRecord.h"




#include "HeadMountedDisplayFunctionLibrary.h"
#include "Net/UnrealNetwork.h"



class FStepComponentData : public FStepSaveData
{
public:

	void GetLine(FString& OutLine) override
	{
		OutLine = "";

		for (int32 i = 0; i<RecordData.Num(); i++)
		{
			OutLine.Append(FString::Printf(TEXT("%3.3f,%3.3f,%3.3f,"), RecordData[i].X, RecordData[i].Y, RecordData[i].Z));
		}

		OutLine.Append("\n");
	}

	TArray<FVector> RecordData;
};

FStepVrDataRecord<FStepComponentData> StepVrDataRecord;




static bool GIsResetHMD = false;
UStepVrComponent::UStepVrComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bAutoActivate = true;

	SetIsReplicatedByDefault(true);
}

void UStepVrComponent::ResetHMD()
{
	GIsResetHMD = false;
	ResetHMDAuto();
}

void UStepVrComponent::DeviceTransform(int32 DeviceID, FTransform& Trans)
{
#if SHOW_STATE
	SCOPE_CYCLE_COUNTER(Stat_StepVrComponetp_DeviceTransform);
#endif

	if (!bAlreadyInitializeLocal)
	{
		return;
	}

	if (bLocalControlled)
	{
		//STEPVR_GLOBAL->RefreshFrame(DeviceID, Trans);
		STEPVR_GLOBAL->GetDeviceTransform(DeviceID, Trans);
	}/*
	else
	{
		Trans = RemotePlayerData.GetDevice(DeviceID).GetTransform();
	}*/
}

void UStepVrComponent::SetPlayerGUID_Implementation(uint32 NewPlayerGUID)
{
	PlayerGUID = NewPlayerGUID;
}

void UStepVrComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UStepVrComponent, PlayerGUID);
}

void UStepVrComponent::RecordStart()
{
	if (bLocalControlled == false)
	{
		return;
	}

	if (StepVrDataRecord.IsRecord())
	{
		return;
	}

	StepVrDataRecord.CreateFile("StepVrComponent");
}

void UStepVrComponent::RecordStop()
{
	StepVrDataRecord.CloseFile();
}

void UStepVrComponent::RecordPushaData(TArray<FVector> LineData)
{
	FStepComponentData StepComponentData;
	StepComponentData.RecordData = LineData;

	StepVrDataRecord.AddData(StepComponentData);
	StepVrDataRecord.SaveLineData();
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
		Pawn->EnableInput(Pawn->GetController<APlayerController>());
		Pawn->InputComponent->BindKey(EKeys::R, EInputEvent::IE_Pressed, this, &UStepVrComponent::ResetHMD);
		UE_LOG(LogStepVrPlugin, Log, TEXT("ResetHMD"));
	}
}

void UStepVrComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
#if SHOW_STATE
	SCOPE_CYCLE_COUNTER(Stat_StepVrComponet_tick);
#endif

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bAlreadyInitializeLocal)
	{
		if (CheckControllState())
		{
			bAlreadyInitializeLocal = true;
			AfterinitializeLocalControlled();
		}
		return;
	}


	if (!bLocalControlled)
	{
		STEPVR_GLOBAL->GetRemoteDeviceFrame(PlayerGUID, RemotePlayerData);
	}
}

void UStepVrComponent::InitializeComponent()
{
	Super::InitializeComponent();
	ResetYaw = GameUseType == FGameUseType::UseType_VR ? 90.f : -90.f;
}

void UStepVrComponent::AfterinitializeLocalControlled()
{
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
	 * GUID
	 */
	SetPlayerGUID(STEPVR_GLOBAL->GetGUID());
}

void UStepVrComponent::ResetHMDFinal()
{
	FTransform TempData;
	if (STEPVR_GLOBAL->GetDeviceTransform(6, TempData))
	{
		float TempYaw = TempData.Rotator().Yaw + ResetYaw;
		UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(TempYaw);
		UE_LOG(LogStepVrPlugin, Log, TEXT("Reset HMD Yaw : %f"), TempYaw);
	}	
}


void UStepVrComponent::ResetOculusRealTime()
{
	//FRotator	S_QTemp;
	//FVector		S_VTemp;
	//UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(S_QTemp, S_VTemp);
	//
	//
	//HMDPitch = S_QTemp.Pitch;

	//double Maxrate;

	///*存储4帧数据*/
	//if (!bIsStart)
	//{
	//	Tnum = 0;
	//	Cnum = 0;
	//	bIsReset = false;
	//	bIsStart = true;
	//	bIsCorrect = false;
	//}
	//if (Tnum == 1)
	//{
	//	HMDYaw[0] = S_QTemp.Yaw;
	//	IMUYaw[0] = CurrentNodeState.FHead.Rotator().Yaw;
	//	Tnum++;
	//}
	//else if (Tnum > 1)
	//{
	//	for (int i = 1; i < Yawn; i++)
	//	{
	//		HMDYaw[Yawn - i] = HMDYaw[Yawn - 1 - i];
	//		IMUYaw[Yawn - i] = IMUYaw[Yawn - 1 - i];
	//	}
	//	HMDYaw[0] = S_QTemp.Yaw;
	//	IMUYaw[0] = CurrentNodeState.FHead.Rotator().Yaw;
	//	HMDrate[0] = HMDYaw[1] - HMDYaw[0];
	//	Maxrate = fabs(HMDrate[0]);
	//	float Maxyaw = IMUYaw[0];
	//	float Minyaw = IMUYaw[0];
	//	for (int i = 1; i < Yawn - 1; i++)
	//	{
	//		HMDrate[i] = HMDYaw[i + 1] - HMDYaw[i];
	//		Maxrate = fabs(HMDrate[i]) > Maxrate ? fabs(HMDrate[i]) : Maxrate;
	//		Maxrate = Maxrate > 180 ? 360 - Maxrate : Maxrate;
	//		Maxyaw = Maxyaw > IMUYaw[i] ? Maxyaw : IMUYaw[i];
	//		Minyaw = Minyaw < IMUYaw[i] ? Minyaw : IMUYaw[i];
	//	}
	//	double DecYaw = (Maxyaw - Minyaw) > 180 ? 360 - (Maxyaw - Minyaw) : (Maxyaw - Minyaw);
	//	if (Maxrate < 1 && fabs(DecYaw) < 2)
	//	{
	//		bIsReset = false;
	//		bIsCorrect = false;
	//	}
	//	else if (Cnum < 200)
	//	{
	//		bIsReset = true;
	//		bIsCorrect = false; 

	//		Cnum = 0;
	//	}
	//	else
	//	{
	//		bIsReset = false;
	//	}
	//}

	//if (!bIsReset)
	//{
	//	//校准
	//	if (fabs(HMDPitch) > 60)
	//	{
	//		return;
	//	}

	//	float Yaw = CurrentNodeState.FHead.Rotator().Yaw;
	//	float DevYaw;

	//	if (Tnum == 0)
	//	{
	//		NewYaw = Yaw + ResetYaw;
	//		NewYaw = NewYaw > 180 ? NewYaw - 360 : NewYaw;
	//		UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(NewYaw);
	//		Tnum = 1;
	//	}
	//	else
	//	{
	//		float TempIMUYaw = Yaw + ResetYaw;
	//		TempIMUYaw = TempIMUYaw > 180 ? TempIMUYaw - 360 : TempIMUYaw;

	//		DevYaw = TempIMUYaw - HMDYaw[0];
	//		DevYaw = DevYaw > 180 ? DevYaw - 360 : DevYaw;
	//		DevYaw = DevYaw < -180 ? DevYaw + 360 : DevYaw;
	//		if (!bIsCorrect && Cnum < 200)
	//		{
	//			if (fabs(DevYaw) > 5)
	//			{
	//				Cnum = fabs(DevYaw) / 0.01;
	//				bIsCorrect = true;
	//			}
	//			else if (Cnum < 200)
	//			{
	//				return;
	//			}
	//		}

	//		DevYaw = DevYaw > 0 ? 0.01 : -0.01;
	//		NewYaw = NewYaw + DevYaw;
	//		Cnum--;
	//		s_bIsResetOculus = false;
	//	}

	//	if (!s_bIsResetOculus)
	//	{
	//		//UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(NewYaw);
	//		FQuat BaseOrientation;
	//		BaseOrientation = (NewYaw != 0.0f) ? FRotator(0, -NewYaw, 0).Quaternion() : FQuat::Identity;
	//		GEngine->XRSystem->SetBaseOrientation(BaseOrientation);
	//		//UE_LOG(LogStepVrPlugin, Warning, TEXT("Stepvr ResetHMD Yaw:%f,%f,%f"), Yaw, HMDYaw[0], NewYaw);

	//		s_bIsResetOculus = true;
	//	}
	//}
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


bool UStepVrComponent::CheckControllState()
{
	APawn* LocalPawn = Cast<APawn>(GetOwner());
	if (LocalPawn == nullptr)
	{
		return false;
	}

	bLocalControlled = LocalPawn->IsLocallyControlled();
	return true;
}


//void UStepVrComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	//DOREPLIFETIME(UStepVrComponent, PlayerIP);
//}