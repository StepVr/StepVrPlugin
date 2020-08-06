
// Fill out your copyright notice in the Description page of Project Settings.
#include "StepVrCameraComponent.h"
#include "Engine.h"

#include "StepVrGlobal.h"
#include "LocalDefine.h"

UStepVrCameraComponent::UStepVrCameraComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}
void UStepVrCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	Super::GetCameraView(DeltaTime, DesiredView);

	do 
	{
		APawn* Pawn = Cast<APawn>(GetOwner());
		if (Pawn == nullptr)
		{
			break;
		}

		if (!Pawn->IsLocallyControlled())
		{
			break;
		}

		if (!STEPVR_FRAME_IsValid)
		{
			break;
		}

		RecaclCameraData(DeltaTime, DesiredView);
	} while (0);
}

void UStepVrCameraComponent::SetCameraInfo(int32 CameraID)
{
	iCameraID = CameraID;
}

void UStepVrCameraComponent::BeginDestroy()
{
	Super::BeginDestroy();
	//HandleCommand = GStepCommand.AddUObject(this,&UStepVrCameraComponent::ExecCommands);
}

void UStepVrCameraComponent::BeginPlay()
{
	Super::BeginPlay();
	//GStepCommand.Remove(HandleCommand);
}

void UStepVrCameraComponent::ExecCommands(FString& Commands)
{
	if (Commands.Equals(TEXT("StartHMDState")))
	{
		if (IsStartRecord)
		{
			return;
		}

		IsStartRecord = true;

		//创建文件
		FString FileName;
		FileName.Append("HDM-Head-").AppendInt(FPlatformTime::Seconds());
		FileName.Append(".csv");


		FString FilePath = FPaths::ProjectSavedDir();
		FilePath.Append(*FileName);
		HandleFile = IFileManager::Get().CreateFileWriter(*FilePath);
	}
	if (Commands.Equals(TEXT("StopHMDState")))
	{
		if (IsStartRecord == false)
		{
			return;
		}

		IsStartRecord = false;

		//写入磁盘
		if (HandleFile)
		{
			HandleFile->Flush();
			delete HandleFile;
			HandleFile = nullptr;
		}
	}
}

void UStepVrCameraComponent::RecaclCameraData(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	FTransform _StepvrHead;
	StepVR::SingleNode Node = STEPVR_FRAME->GetFrame().GetSingleNode();
	
	if (!STEPVR_GLOBAL_IsValid)
	{
		return;
	}
	STEPVR_GLOBAL->SVGetDeviceState(&Node, iCameraID, _StepvrHead);

	//Command录制数据
	if (IsStartRecord)
	{
		RecordHMDData(_StepvrHead, DesiredView);
	}

	//重新计算位置姿态
	SetRelativeLocation(_StepvrHead.GetLocation());
	
	//Cave眼镜，需要设置姿态
	if (iCameraID == 198)
	{
		SetRelativeRotation(_StepvrHead.Rotator());
	}

	DesiredView.Location = GetComponentToWorld().GetLocation();
}

void UStepVrCameraComponent::RecordHMDData(FTransform& Head, FMinimalViewInfo& CameraInfo)
{
	FVector HeadVec = Head.GetLocation();
	//FVector( Roll, Pitch, Yaw )
	FVector HeadRo  = Head.GetRotation().Euler();
	FString Str = FString::Format(TEXT("{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12}\n"), 
		{
			HeadVec.X,
			HeadVec.Y,
			HeadVec.Z,
			HeadRo.X,
			HeadRo.Y,
			HeadRo.Z,
			CameraInfo.Location.X,
			CameraInfo.Location.Y,
			CameraInfo.Location.Z,
			CameraInfo.Rotation.Roll,
			CameraInfo.Rotation.Pitch,
			CameraInfo.Rotation.Yaw,
		});

	FTCHARToUTF8 UTF8String(*Str);
	HandleFile->Serialize((UTF8CHAR*)UTF8String.Get(), UTF8String.Length() * sizeof(UTF8CHAR));
}
