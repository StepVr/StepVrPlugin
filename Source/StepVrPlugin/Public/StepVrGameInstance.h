// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "StepVrGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class STEPVRPLUGIN_API UStepVrGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	/******************************命令行**********************************/
	//Net发送间隔
	UFUNCTION(exec)
	void StepServerSendInterval(float Interval);
	
	UFUNCTION(exec)
	void StepFrameForecastInterval(float Interval);
	
	UFUNCTION(exec)
	void StepFrameForecast(int32 IsForecast);
	
	UFUNCTION(exec)
	void StepFrameLerp(int32 IsLerp);
	
	UFUNCTION(exec)
	void StepFrameLerpAlpha(float Alpha);


	/************************************************************************/

	/*******************************录制数据********************************/
	//接收数据
	UFUNCTION(exec)
	void StepServerReceive_Start();
	UFUNCTION(exec)
	void StepServerReceive_Stop();

	//发送数据
	UFUNCTION(exec)
	void StepServerSend_Start();
	UFUNCTION(exec)
	void StepServerSend_Stop();

	//录制头显，定位数据
	UFUNCTION(exec)
	void StepCamera_Start();
	UFUNCTION(exec)
	void StepCamera_Stop();

};
