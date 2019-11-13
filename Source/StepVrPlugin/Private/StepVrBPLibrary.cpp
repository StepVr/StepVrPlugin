// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "StepVrBPLibrary.h"
#include "License/SDKLic.h"
#include "LocalDefine.h"
#include "StepVrGlobal.h"
#include "StepVrServerModule.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "IHeadMountedDisplay.h"
#include "Engine.h"



UStepVrBPLibrary::UStepVrBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

bool UStepVrBPLibrary::SVCheckGameLic(FString gameId){
	FSDKLic _lic;
	if (!_lic.CheckLicIsValid(gameId))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("StepVR", "StepVR", "{0}"), FText::FromString(TEXT("Lic Invalid!"))));
		FPlatformMisc::RequestExit(0);
		return false;
	}
	return true;
}
void UStepVrBPLibrary::SVGetDeviceState(StepVR::SingleNode* InSingleNode, int32 EquipId, FTransform& Transform)
{
	if (!InSingleNode->IsHardWareLink(EquipId))
	{
		return;
	}

	static StepVR::Vector3f vec3;
	vec3 = InSingleNode->GetPosition(SDKNODEID(EquipId));
	vec3 = StepVR::StepVR_EnginAdaptor::toUserPosition(vec3);
	Transform.SetLocation(FVector(vec3.x * 100, vec3.y * 100, vec3.z * 100));

	static StepVR::Vector4f vec4;
	vec4= InSingleNode->GetQuaternion(SDKNODEID(EquipId));
	if (EquipId == 6)
	{
		vec4 = StepVR::StepVR_EnginAdaptor::toUserQuat(vec4);
		Transform.SetRotation(FQuat(vec4.y*-1, vec4.x, vec4.z, vec4.w));

		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected())
		{
			FRotator	S_QTemp;
			FVector		S_VTemp;
			UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(S_QTemp, S_VTemp);
			GLocalDevicesRT.FindOrAdd(StepVrDeviceID::DHMD) = FTransform(
				S_QTemp.Quaternion(), 
				Transform.GetLocation() - S_VTemp);
		}
	}
	else
	{
		vec4 = StepVR::StepVR_EnginAdaptor::toUserQuat(vec4);
		Transform.SetRotation(FQuat(vec4.x, vec4.y, vec4.z, vec4.w));
	}

	GLocalDevicesRT.FindOrAdd(EquipId) = Transform;
}


void UStepVrBPLibrary::SVGetDeviceStateWithID(int32 DeviceID, FTransform& Transform)
{
	FTransform* _trans = GLocalDevicesRT.Find(DeviceID);
	if (_trans)
	{
		Transform = *_trans;
		return;
	}

	if (GNeedUpdateDevices.Find(DeviceID) == INDEX_NONE)
	{
		GNeedUpdateDevices.Add(DeviceID);
	}
}

FTransform UStepVrBPLibrary::Convert2UETransform(float Vx, float Vy, float Vz, float Qw, float Qx, float Qy, float Qz)
{
	FTransform Transform;
	StepVR::Vector3f vec3;
	vec3.x = Vx;
	vec3.y = Vy;
	vec3.z = Vz;
	vec3 = StepVR::StepVR_EnginAdaptor::toUserPosition(vec3);
	Transform.SetLocation(FVector(vec3.x * 100, vec3.y * 100, vec3.z * 100));

	StepVR::Vector4f vec4;
	vec4.w = Qw;
	vec4.x = Qx;
	vec4.y = Qy;
	vec4.z = Qz;
	vec4 = StepVR::StepVR_EnginAdaptor::toUserQuat(vec4);
	Transform.SetRotation(FQuat(vec4.x, vec4.y, vec4.z, vec4.w));

	return Transform;
}

void UStepVrBPLibrary::SetGameType(FGameType type, FString ServerIP)
{
	if (STEPVR_SERVER_IsValid)
	{
		STEPVR_SERVER->SetGameModeType((EGameModeType)type);
		if (type == FGameType::GameClient)
		{
			STEPVR_SERVER->UpdateServerIP(ServerIP);
		}
	}
}
