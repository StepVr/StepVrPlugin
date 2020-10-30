// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Camera/CameraComponent.h"
#include "StepVrCameraComponent.generated.h"



UCLASS(meta = (BlueprintSpawnableComponent), ClassGroup = (StepVR,Camera))
class UStepVrCameraComponent : public UCameraComponent
{
	GENERATED_UCLASS_BODY()
		
public:
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;
	
	//设置Camera类型
	void SetCameraInfo(int32 CameraID , bool IsLocal);

protected:
	void RecaclCameraData(float DeltaTime, FMinimalViewInfo& DesiredView);

	int32	iCameraID = 6;
	bool	bLocalControlled = false;
};
