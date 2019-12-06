// Fill out your copyright notice in the Description page of Project Settings.


#include "StepVrGameInstance.h"
#include "StepVrGlobal.h"



void UStepVrGameInstance::StepServerSendInterval(float Interval)
{
	GStepServerSendInterval = Interval;
}

void UStepVrGameInstance::StepFrameForecastInterval(float Interval)
{
	GStepFrameForecastInterval = Interval;
}

void UStepVrGameInstance::StepFrameForecast(int32 IsForecast)
{
	GStepFrameForecast = IsForecast != 0 ? true : false;
}

void UStepVrGameInstance::StepFrameLerp(int32 IsLerp)
{
	GStepFrameLerp = IsLerp != 0 ? true : false;
}

void UStepVrGameInstance::StepFrameLerpAlpha(float Alpha)
{
	GStepFrameLerpAlpha = Alpha;
}

void UStepVrGameInstance::StepStartServerState()
{
	ExecCommand(TEXT("StartServerState"));
}

void UStepVrGameInstance::StepStopServerState()
{
	ExecCommand(TEXT("StopServerState"));
}

void UStepVrGameInstance::StepStartHMDState()
{
	ExecCommand(TEXT("StartHMDState"));
}

void UStepVrGameInstance::StepStopHMDState()
{
	ExecCommand(TEXT("StopHMDState"));
}

void UStepVrGameInstance::ExecCommand(const FString& Command)
{
	if (GStepCommand.IsBound())
	{
		FString ExecStr(*Command);
		GStepCommand.Broadcast(ExecStr);
	}
}
