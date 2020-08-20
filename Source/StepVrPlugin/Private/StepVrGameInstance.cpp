// Fill out your copyright notice in the Description page of Project Settings.


#include "StepVrGameInstance.h"
#include "StepVrGlobal.h"


void ExecCommand(ECommandState NewCommand, int32 InData)
{
	STEPVR_GLOBAL->ExecCommand(NewCommand, InData);
}

void UStepVrGameInstance::StepServerSendInterval(float Interval)
{
	//GStepServerSendInterval = Interval;
}

void UStepVrGameInstance::StepFrameForecastInterval(float Interval)
{
	//GStepFrameForecastInterval = Interval;
}

void UStepVrGameInstance::StepFrameForecast(int32 IsForecast)
{
	//GStepFrameForecast = IsForecast != 0 ? true : false;
}

void UStepVrGameInstance::StepFrameLerp(int32 IsLerp)
{
	//GStepFrameLerp = IsLerp != 0 ? true : false;
}

void UStepVrGameInstance::StepFrameLerpAlpha(float Alpha)
{
	//GStepFrameLerpAlpha = Alpha;
}

void UStepVrGameInstance::StepServerReceive_Start()
{
	ExecCommand(ECommandState::Stat_ServerReceiveRecord, 1);
}

void UStepVrGameInstance::StepServerReceive_Stop()
{
	ExecCommand(ECommandState::Stat_ServerReceiveRecord, 0);
}

void UStepVrGameInstance::StepServerSend_Start()
{
	ExecCommand(ECommandState::Stat_ServerSendRecord, 1);
}

void UStepVrGameInstance::StepServerSend_Stop()
{
	ExecCommand(ECommandState::Stat_ServerSendRecord, 0);
}

void UStepVrGameInstance::StepCamera_Start()
{
	ExecCommand(ECommandState::Stat_CameraRecord, 1);
}

void UStepVrGameInstance::StepCamera_Stop()
{
	ExecCommand(ECommandState::Stat_CameraRecord, 0);
}

