// Fill out your copyright notice in the Description page of Project Settings.
#include "StepVrCameraComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Pawn.h"
#include "StepVrBPLibrary.h"
#include "StepVrGlobal.h"
#include "LocalDefine.h"



void UStepVrCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	Super::GetCameraView(DeltaTime, DesiredView);

	if (StepVrGlobal::Get()->SDKIsValid())
	{
		RecaclCameraData(DeltaTime, DesiredView);
	}
}

void UStepVrCameraComponent::RecaclCameraData(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	auto _pawn = Cast<APawn>(GetOwner());
	if (!IsValid(_pawn)) { return; }
	if (!_pawn->IsLocallyControlled()) { return; }

	/** Get HeadTransform */
	FTransform _StepvrHead;
	StepVR::Frame tmp = STEPVR_FRAME->GetFrame();
	UStepVrBPLibrary::SVGetDeviceStateWithID(&tmp, StepVrDeviceID::DHead, _StepvrHead);

	SetRelativeLocation(_StepvrHead.GetLocation());
	FTransform BaseCamToWorld = GetComponentToWorld();
	if (bUseAdditiveOffset)
	{
		FTransform OffsetCamToBaseCam = AdditiveOffset;
		FTransform OffsetCamToWorld = OffsetCamToBaseCam * BaseCamToWorld;
		DesiredView.Location = OffsetCamToWorld.GetLocation();
	}
	else
	{
		DesiredView.Location = BaseCamToWorld.GetLocation();
	}

	DesiredView.Rotation = _StepvrHead.GetRotation().Rotator();
}
