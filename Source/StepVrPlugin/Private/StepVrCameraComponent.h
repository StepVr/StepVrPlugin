// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Camera/CameraComponent.h"
#include "StepVrCameraComponent.generated.h"



UCLASS(meta = (BlueprintSpawnableComponent), ClassGroup = (StepVR,Camera))
class STEPVRPLUGIN_API UStepVrCameraComponent : public UCameraComponent
{
	GENERATED_UCLASS_BODY()
		
public:
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

protected:
	virtual void BeginPlay() override;
	void RecaclCameraData(float DeltaTime, FMinimalViewInfo& DesiredView);

	bool IsLocalControlled = false;
};
