// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "StepVrBPLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "MessageDialog.h"
#include "LocalDefine.h"
#include "License/SDKLic.h"
#include "StepVrGlobal.h"



UStepVrBPLibrary::UStepVrBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

bool UStepVrBPLibrary::SVStepVRIsValid()
{
	return StepVrGlobal::Get()->SDKIsValid();
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
bool UStepVrBPLibrary::SVGetDeviceStateWithID(StepVR::Frame* Frame, int32 EquipId, FTransform& Transform)
{
	static StepVR::Vector3f vec3;
	vec3 = Frame->GetSingleNode().GetPosition(SDKNODEID(EquipId));
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
	vec4= Frame->GetSingleNode().GetQuaternion(SDKNODEID(EquipId));
	vec4 = StepVR::StepVR_EnginAdaptor::toUserQuat(vec4);
	Transform.SetRotation(FQuat(vec4.x, vec4.y, vec4.z, vec4.w));

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
