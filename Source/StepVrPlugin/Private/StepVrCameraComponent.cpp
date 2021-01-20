
// Fill out your copyright notice in the Description page of Project Settings.
#include "StepVrCameraComponent.h"
#include "StepVrGlobal.h"

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

		if (!UseStepData)
		{
			break;
		}

		RecaclCameraData(DeltaTime, DesiredView);
	} while (0);

	if (StepVrDataRecord.IsRecord())
	{
		FStepCameraData StepCameraData;
		StepCameraData.SetTransform(DesiredView.Location, DesiredView.Rotation);

		StepVrDataRecord.AddData(StepCameraData);
		StepVrDataRecord.SaveLineData();
	}
}


void UStepVrCameraComponent::BeginDestroy()
{
	Super::BeginDestroy();
	STEPVR_GLOBAL->GetCommandDelegate().Remove(CommandHandle);
}

void UStepVrCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	CommandHandle = STEPVR_GLOBAL->GetCommandDelegate().AddLambda([&](ECommandState NewState, int32 Values)
		{
			if (NewState != ECommandState::Stat_CameraRecord)
			{
				return;
			}

			if (Values != 0)
			{
				StepVrDataRecord.CreateFile("Camera");
			}
			else
			{
				StepVrDataRecord.CloseFile();
			}
		});
}

void UStepVrCameraComponent::ChangeCameraData()
{
	UseStepData = UseStepData ? false : true;
}

void UStepVrCameraComponent::RecaclCameraData(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	FTransform TempFrame;
	STEPVR_GLOBAL->RefreshFrame(6, TempFrame);
	//STEPVR_GLOBAL->GetDeviceTransform(6, TempFrame);

	//重新计算位置姿态
	SetRelativeLocation(TempFrame.GetLocation());
	
	DesiredView.Location = GetComponentToWorld().GetLocation();
}
