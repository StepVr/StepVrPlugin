// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Camera/CameraComponent.h"
#include "StepVrCameraComponent.generated.h"

class FArchive;


UCLASS(meta = (BlueprintSpawnableComponent), ClassGroup = (StepVR,Camera))
class STEPVRPLUGIN_API UStepVrCameraComponent : public UCameraComponent
{
	GENERATED_UCLASS_BODY()
		
public:
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;
	
	//设置Camera类型
	void SetCameraInfo(int32 CameraID);

	virtual void BeginDestroy() override;

	virtual void BeginPlay() override;

	void ExecCommands(FString& Commands);

protected:
	void RecaclCameraData(float DeltaTime, FMinimalViewInfo& DesiredView);

	void RecordHMDData(FTransform& Head, FMinimalViewInfo& CameraInfo);

	int32	iCameraID = 6;

	//Delegate
	FDelegateHandle HandleCommand;

	bool					IsStartRecord;
	FArchive*				HandleFile;
};
