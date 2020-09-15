// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Camera/CameraComponent.h"
#include "StepVrDataRecord.h"
#include "StepVrCameraComponent.generated.h"




class FStepCameraData : public FStepSaveData
{
public:
	FVector		Location;
	FRotator	Rotator;

	void SetTransform(FVector& inLocation, FRotator& inRotator)
	{
		Location = inLocation;
		Rotator = inRotator;

		//数据间隔
		{
			static int64 _CacheTicks;
			DealTicks(_CacheTicks);
		}
	}

	virtual void GetLine(FString& OutLine)
	{
		OutLine = FString::Format(TEXT("{0},{1},{2},{3},{4},{5},{6}\n"),
			{ 
				DataInterval,
				Location.X,
				Location.Y,
				Location.Z,
				Rotator.Yaw,
				Rotator.Pitch,
				Rotator.Roll
			});
	}

};



UCLASS(meta = (BlueprintSpawnableComponent), ClassGroup = (StepVR,Camera))
class STEPVRPLUGIN_API UStepVrCameraComponent : public UCameraComponent
{
	GENERATED_UCLASS_BODY()
	
public:
	
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	virtual void BeginDestroy() override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ChangeCameraData();

protected:
	void RecaclCameraData(float DeltaTime, FMinimalViewInfo& DesiredView);

	FStepVrDataRecord<FStepCameraData>	StepVrDataRecord;
	FDelegateHandle	CommandHandle;
	bool UseStepData = true;
};
