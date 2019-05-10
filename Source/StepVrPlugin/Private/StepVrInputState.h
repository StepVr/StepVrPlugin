// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "LocalDefine.h"



/** The Device Have Key */
enum class EStepVrDeviceId
{
	DLeft,
	Dright,
	DGun,


	DTotalCount
};

//Register KeyName
struct FStepVRCapacitiveKey
{
	FKey StepVR_GunBtn_A_Trigger;
	FKey StepVR_GunBtn_B_Trigger;
	FKey StepVR_GunBtn_C_Trigger;

	FKey StepVR_LeftBtn_A_Trigger;
	FKey StepVR_LeftBtn_B_Trigger;
	FKey StepVR_LeftBtn_C_Trigger;

	FKey StepVR_RightBtn_A_Trigger;
	FKey StepVR_RightBtn_B_Trigger;
	FKey StepVR_RightBtn_C_Trigger;

	FStepVRCapacitiveKey() :
		StepVR_GunBtn_A_Trigger("StepVR_GunBtnA_Press"),
		StepVR_GunBtn_B_Trigger("StepVR_GunBtnB_Press"),
		StepVR_GunBtn_C_Trigger("StepVR_GunBtnC_Press"),
		StepVR_LeftBtn_A_Trigger("StepVR_LeftA_Press"),
		StepVR_LeftBtn_B_Trigger("StepVR_LeftB_Press"),
		StepVR_LeftBtn_C_Trigger("StepVR_LeftC_Press"),
		StepVR_RightBtn_A_Trigger("StepVR_RightA_Press"),
		StepVR_RightBtn_B_Trigger("StepVR_RightB_Press"),
		StepVR_RightBtn_C_Trigger("StepVR_RightC_Press")
	{

	}
};

const uint8 SButton_Release = (int32)FMath::Pow(2, 0);
const uint8 SButton_Press = (int32)FMath::Pow(2, 1);
const uint8 SButton_Repeat = (int32)FMath::Pow(2, 2);
struct FStepVrButtonState
{
	FName key;

	uint8  PressedState;

	double NextRepeatTime;

	FStepVrButtonState()
		:key(NAME_None),
		PressedState(SButton_Release),
		NextRepeatTime(0.0)
	{

	}
};

struct FStepVrDeviceState
{
	StepVR::SingleNode::NodeID EquipId;

	TArray<FStepVrButtonState> TBtnKey;
};

struct FStepVrStateController
{
	FStepVrDeviceState Devices[(int32)EStepVrDeviceId::DTotalCount];
	FStepVRCapacitiveKey MyKey;
	FStepVrStateController()
	{

		//Init
		FStepVrButtonState btn;

		Devices[(int32)EStepVrDeviceId::DLeft].EquipId = SDKNODEID((int32)StepVrDeviceID::DLeftController);
		btn.key = MyKey.StepVR_LeftBtn_A_Trigger.GetFName();
		Devices[(int32)EStepVrDeviceId::DLeft].TBtnKey.Add(btn);
		btn.key = MyKey.StepVR_LeftBtn_B_Trigger.GetFName();
		Devices[(int32)EStepVrDeviceId::DLeft].TBtnKey.Add(btn);
		btn.key = MyKey.StepVR_LeftBtn_C_Trigger.GetFName();
		Devices[(int32)EStepVrDeviceId::DLeft].TBtnKey.Add(btn);

		Devices[(int32)EStepVrDeviceId::Dright].EquipId = SDKNODEID((int32)StepVrDeviceID::DRightController);
		btn.key = MyKey.StepVR_RightBtn_A_Trigger.GetFName();
		Devices[(int32)EStepVrDeviceId::Dright].TBtnKey.Add(btn);
		btn.key = MyKey.StepVR_RightBtn_B_Trigger.GetFName();
		Devices[(int32)EStepVrDeviceId::Dright].TBtnKey.Add(btn);
		btn.key = MyKey.StepVR_RightBtn_C_Trigger.GetFName();
		Devices[(int32)EStepVrDeviceId::Dright].TBtnKey.Add(btn);

		Devices[(int32)EStepVrDeviceId::DGun].EquipId = SDKNODEID((int32)StepVrDeviceID::DGun);
		btn.key = MyKey.StepVR_GunBtn_A_Trigger.GetFName();
		Devices[(int32)EStepVrDeviceId::DGun].TBtnKey.Add(btn);
		btn.key = MyKey.StepVR_GunBtn_B_Trigger.GetFName();
		Devices[(int32)EStepVrDeviceId::DGun].TBtnKey.Add(btn);
		btn.key = MyKey.StepVR_GunBtn_C_Trigger.GetFName();
		Devices[(int32)EStepVrDeviceId::DGun].TBtnKey.Add(btn);
	}
};