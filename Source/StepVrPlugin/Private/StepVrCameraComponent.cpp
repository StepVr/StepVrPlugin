﻿
// Fill out your copyright notice in the Description page of Project Settings.
#include "StepVrCameraComponent.h"
#include "Engine.h"

#include "StepVrBPLibrary.h"
#include "StepVrGlobal.h"
#include "LocalDefine.h"

UStepVrCameraComponent::UStepVrCameraComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}
void UStepVrCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	Super::GetCameraView(DeltaTime, DesiredView);

	if (bLocalControlled && STEPVR_FRAME_IsValid)  
	{
		RecaclCameraData(DeltaTime, DesiredView);
	}
}

void UStepVrCameraComponent::SetCameraInfo(int32 CameraID, bool IsLocal)
{
	iCameraID = CameraID;
	bLocalControlled = IsLocal;
}

void UStepVrCameraComponent::RecaclCameraData(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	FTransform _StepvrHead;
	StepVR::SingleNode Node = STEPVR_FRAME->GetFrame().GetSingleNode();

	UStepVrBPLibrary::SVGetDeviceStateWithID(&Node, iCameraID, _StepvrHead);
	SetRelativeLocation(_StepvrHead.GetLocation());
	DesiredView.Location = GetComponentToWorld().GetLocation();
}
