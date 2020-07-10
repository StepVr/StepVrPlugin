// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "LocalDefine.h"
#include "StepVrGlobal.h"
#include "StepVrConfig.h"

#include "InputCoreTypes.h "
#include "CoreGlobals.h"

const uint8 SButton_Release = (int32)FMath::Pow(2, 0);
const uint8 SButton_Press = (int32)FMath::Pow(2, 1);
const uint8 SButton_Repeat = (int32)FMath::Pow(2, 2);
struct FStepVrButtonState
{
	uint8  KeyID = 0;
	int32  DeviceID = 0;
	uint8  PressedState = 0;
	double NextRepeatTime = 0;

	FName key;
	EStepDeviceKeyType ActionState;
};


struct FStepVrStateController
{
public:
	FStepVrStateController()
	{
		UStepSetting* StepConfig = UStepSetting::Instance();
		for (auto& KeyInfo : StepConfig->KeyAction)
		{
			AddActions(KeyInfo.KeyID, KeyInfo.KeyName,KeyInfo.DeviceID, KeyInfo.KeyType);
		}
	}

	void AddActions(uint8 InKeyId, FName KeyName, int32 DeviceID, EStepDeviceKeyType ActionState)
	{
		FStepVrButtonState ButtonState;

		ButtonState.PressedState = SButton_Release;
		ButtonState.DeviceID = DeviceID;
		ButtonState.KeyID = InKeyId;
		ButtonState.key = KeyName;
		ButtonState.ActionState = ActionState;
		Devices.Add(ButtonState);
	}

	//设备状态
	TArray<FStepVrButtonState> Devices;
};