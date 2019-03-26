// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "StepVrBPLibrary.h"
#include "Engine.h"

#include "License/SDKLic.h"
#include "LocalDefine.h"


UStepVrBPLibrary::UStepVrBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

bool UStepVrBPLibrary::SVCheckGameLic(FString gameId){
	FSDKLic _lic;
	if (!_lic.CheckLicIsValid(gameId))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("StepVR", "StepVR", "{0}"), FText::FromString(TEXT("Lic Invalid!"))));
		if (IsValid(GWorld))
		{
			APlayerController* _player = UGameplayStatics::GetPlayerController(GWorld, 0);
			if (IsValid(_player))
			{
				_player->ConsoleCommand("quit");
			}
		}
		return false;
	}
	return true;
}
bool UStepVrBPLibrary::SVGetDeviceStateWithID(StepVR::SingleNode* InSingleNode, int32 EquipId, FTransform& Transform)
{
	if (!InSingleNode->IsHardWareLink(EquipId))
	{
		return false;
	}

	static StepVR::Vector3f vec3;
	vec3 = InSingleNode->GetPosition(SDKNODEID(EquipId));
	vec3 = StepVR::StepVR_EnginAdaptor::toUserPosition(vec3);
	if (FMath::Abs(vec3.x) < 50 &&
		FMath::Abs(vec3.y) < 50 &&
		FMath::Abs(vec3.z) < 5)
	{
		Transform.SetLocation(FVector(vec3.x * 100, vec3.y * 100, vec3.z * 100));
	}
	else
	{
		return false;
	}


	static StepVR::Vector4f vec4;
	vec4= InSingleNode->GetQuaternion(SDKNODEID(EquipId));
	if (EquipId == 6)
	{
		vec4 = StepVR::StepVR_EnginAdaptor::toUserQuat(vec4);
		Transform.SetRotation(FQuat(vec4.y*-1, vec4.x, vec4.z, vec4.w));
	}
	else
	{
		vec4 = StepVR::StepVR_EnginAdaptor::toUserQuat(vec4);
		Transform.SetRotation(FQuat(vec4.x, vec4.y, vec4.z, vec4.w));
	}

	S_mStepVrDeviceState.FindOrAdd(EquipId) = Transform;
	return true;
}


bool UStepVrBPLibrary::SVGetDeviceStateWithID(int32 DeviceID, FTransform& Transform)
{
	FTransform* _trans = S_mStepVrDeviceState.Find(DeviceID);
	if (_trans)
	{
		Transform = *_trans;
		return true;
	}

	return false;
}
void UStepVrBPLibrary::ConvertCoordinateToUE(FTransform& InOutData)
{
	FVector TempLocation = InOutData.GetLocation();
	FQuat TempRotation = InOutData.GetRotation();

	StepVR::Vector3f vec3(TempLocation.X, TempLocation.Y, TempLocation.Z);
	StepVR::Vector4f vec4(TempRotation.W, TempRotation.X, TempRotation.Y, TempRotation.Z);
	vec3 = StepVR::StepVR_EnginAdaptor::toUserPosition(vec3);
	vec4 = StepVR::StepVR_EnginAdaptor::toUserQuat(vec4);

	//InOutData.SetLocation(FVector(vec3.x, vec3.y, vec3.z));
	//InOutData.SetRotation(FQuat(vec4.x, vec4.y, vec4.z, vec4.w));
}
