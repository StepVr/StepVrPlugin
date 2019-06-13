// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "LocalDefine.h"
#include "InputCoreTypes.h "


/** The Device Have Key */
enum class EStepVrDeviceId
{
	DLeft,
	Dright,
	DGun,


	DTotalCount
};

enum class EStepActionState
{
	State_Button,
	State_ValueX,
	State_ValueY,
};

//Register KeyName
struct FStepVRCapacitiveKey
{
	/**
	 * 枪
	 */
	FKey StepVR_GunBtn_A_Trigger;
	FKey StepVR_GunBtn_B_Trigger;
	FKey StepVR_GunBtn_C_Trigger;
	FKey StepVR_GunBtn_D_Trigger;
	FKey StepVR_Gun_ValueX;
	FKey StepVR_Gun_ValueY;

	/**
	 * 左手
	 */
	FKey StepVR_LeftBtn_A_Trigger;
	FKey StepVR_LeftBtn_B_Trigger;
	FKey StepVR_LeftBtn_C_Trigger;

	/**
	 * 右手
	 */
	FKey StepVR_RightBtn_A_Trigger;
	FKey StepVR_RightBtn_B_Trigger;
	FKey StepVR_RightBtn_C_Trigger;

	/**
	 * 导演监视器
	 */
	FKey StepVR_DirMon_A_Trigger;
	FKey StepVR_DirMon_B_Trigger;
	FKey StepVR_DirMon_C_Trigger;
	FKey StepVR_DirMon_ValueX;
	FKey StepVR_DirMon_ValueY;


	FStepVRCapacitiveKey() :
		StepVR_GunBtn_A_Trigger("StepVR_GunBtnA_Press"),
		StepVR_GunBtn_B_Trigger("StepVR_GunBtnB_Press"),
		StepVR_GunBtn_C_Trigger("StepVR_GunBtnC_Press"),
		StepVR_GunBtn_D_Trigger("StepVR_GunBtnD_Press"),
		StepVR_Gun_ValueX("StepVR_Gun_ValueX"),
		StepVR_Gun_ValueY("StepVR_Gun_ValueY"),
		StepVR_LeftBtn_A_Trigger("StepVR_LeftA_Press"),
		StepVR_LeftBtn_B_Trigger("StepVR_LeftB_Press"),
		StepVR_LeftBtn_C_Trigger("StepVR_LeftC_Press"),
		StepVR_RightBtn_A_Trigger("StepVR_RightA_Press"),
		StepVR_RightBtn_B_Trigger("StepVR_RightB_Press"),
		StepVR_RightBtn_C_Trigger("StepVR_RightC_Press"),
		StepVR_DirMon_A_Trigger("StepVR_DirMon_A_Trigger"),
		StepVR_DirMon_B_Trigger("StepVR_DirMon_B_Trigger"),
		StepVR_DirMon_C_Trigger("StepVR_DirMon_C_Trigger"),
		StepVR_DirMon_ValueX("StepVR_DirMon_ValueX"),
		StepVR_DirMon_ValueY("StepVR_DirMon_ValueY")
	{

	}
};

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
	EStepActionState ActionState;

	FStepVrButtonState()
	{
		key = TEXT("");
		ActionState = EStepActionState::State_Button;
	}
};

struct FStepVrDeviceState
{
	StepVR::SingleNode::NodeID EquipId;

	TArray<FStepVrButtonState> TBtnKey;
};

struct FStepVrStateController
{
	//设备状态
	TArray<FStepVrButtonState> Devices;

	void AddActions(uint8 InKeyId, FName KeyName, int32 DeviceID, EStepActionState ActionState = EStepActionState::State_Button)
	{
		FStepVrButtonState ButtonState;

		ButtonState.DeviceID = DeviceID;
		ButtonState.KeyID = InKeyId;
		ButtonState.key = KeyName;
		ButtonState.ActionState = ActionState;
		Devices.Add(ButtonState);
	}

	FStepVrStateController()
	{
		FStepVRCapacitiveKey CapacitiveKey;
		
		/**
		 * 左手柄 : 1
		 */
		{
			AddActions(StepVR::SingleNode::KeyA,
				CapacitiveKey.StepVR_LeftBtn_A_Trigger.GetFName(),
				StepVrDeviceID::DLeftController);
			AddActions(StepVR::SingleNode::KeyB,
				CapacitiveKey.StepVR_LeftBtn_B_Trigger.GetFName(),
				StepVrDeviceID::DLeftController);
			AddActions(StepVR::SingleNode::KeyC,
				CapacitiveKey.StepVR_LeftBtn_C_Trigger.GetFName(),
				StepVrDeviceID::DLeftController);
		}
		
		/**
		 * 右手柄 : 2
		 */
		{
			AddActions(StepVR::SingleNode::KeyA,
				CapacitiveKey.StepVR_RightBtn_A_Trigger.GetFName(),
				StepVrDeviceID::DRightController);
			AddActions(StepVR::SingleNode::KeyB,
				CapacitiveKey.StepVR_RightBtn_B_Trigger.GetFName(),
				StepVrDeviceID::DRightController);
			AddActions(StepVR::SingleNode::KeyC,
				CapacitiveKey.StepVR_RightBtn_C_Trigger.GetFName(),
				StepVrDeviceID::DRightController);
		}
		
		/**
		 * 枪 : 4
		 */
		{
			AddActions(StepVR::SingleNode::KeyA,
				CapacitiveKey.StepVR_GunBtn_A_Trigger.GetFName(),
				StepVrDeviceID::DGun);
			AddActions(StepVR::SingleNode::KeyB,
				CapacitiveKey.StepVR_GunBtn_B_Trigger.GetFName(),
				StepVrDeviceID::DGun);
			AddActions(StepVR::SingleNode::KeyC,
				CapacitiveKey.StepVR_GunBtn_C_Trigger.GetFName(),
				StepVrDeviceID::DGun);
			AddActions(StepVR::SingleNode::KeyD,
				CapacitiveKey.StepVR_GunBtn_D_Trigger.GetFName(),
				StepVrDeviceID::DGun);
			AddActions(StepVR::SingleNode::MAX_KEY,
				CapacitiveKey.StepVR_Gun_ValueX.GetFName(),
				StepVrDeviceID::DGun,
				EStepActionState::State_ValueX);
			AddActions(StepVR::SingleNode::MAX_KEY,
				CapacitiveKey.StepVR_Gun_ValueY.GetFName(),
				StepVrDeviceID::DGun,
				EStepActionState::State_ValueY);
		}
		
		/**
		 * 导演监视器 ID：8
		 */
		{
			AddActions(StepVR::SingleNode::KeyA,
				CapacitiveKey.StepVR_DirMon_A_Trigger.GetFName(),
				StepVrDeviceID::DDirMon);
			AddActions(StepVR::SingleNode::KeyF,
				CapacitiveKey.StepVR_DirMon_B_Trigger.GetFName(),
				StepVrDeviceID::DDirMon);
			AddActions(StepVR::SingleNode::KeyG,
				CapacitiveKey.StepVR_DirMon_C_Trigger.GetFName(),
				StepVrDeviceID::DDirMon);

			AddActions(StepVR::SingleNode::MAX_KEY,
				CapacitiveKey.StepVR_DirMon_ValueX.GetFName(),
				StepVrDeviceID::DDirMon, EStepActionState::State_ValueX);
			AddActions(StepVR::SingleNode::MAX_KEY,
				CapacitiveKey.StepVR_DirMon_ValueY.GetFName(),
				StepVrDeviceID::DDirMon, EStepActionState::State_ValueY);
		}
	}
};