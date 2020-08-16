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

void UStepVrGameInstance::StepStartRecord_Online()
{
	ExecCommand(ECommandState::Stat_RecordOnline, 1);
}

void UStepVrGameInstance::StepStopRecord_Online()
{
	ExecCommand(ECommandState::Stat_RecordOnline, 0);
}

void UStepVrGameInstance::StepStartRecord_Camera()
{
	ExecCommand(ECommandState::Stat_RecordCamera, 1);
}

void UStepVrGameInstance::StepStopRecord_Camera()
{
	ExecCommand(ECommandState::Stat_RecordCamera, 0);
}

