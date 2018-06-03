// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Camera/CameraComponent.h"
#include "StepVrCameraComponent.generated.h"



UCLASS(meta = (BlueprintSpawnableComponent), ClassGroup = (StepVR,Camera))
class STEPVRPLUGIN_API UStepVrCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
		
public:
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;
private:
	void RecaclCameraData(float DeltaTime, FMinimalViewInfo& DesiredView);
};
