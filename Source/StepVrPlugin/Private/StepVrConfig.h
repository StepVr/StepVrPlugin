// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Engine/DeveloperSettings.h"
#include "StepVrConfig.generated.h"

UENUM()
enum EStepDeviceKeyType
{
	State_Button,
	State_ValueX,
	State_ValueY,
};


UENUM()
enum EStepDeviceKeyID
{
	KeyA = 1,
	KeyB,
	KeyC,
	KeyD,
	KeyE,
	KeyF,
	KeyG,
	KeyH,
	ValueX,
	ValueY,
};


USTRUCT()
struct FStepDeviceAction
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 DeviceID;

	UPROPERTY()
	FName KeyName;

	UPROPERTY()
	TEnumAsByte<EStepDeviceKeyType> KeyType;

	UPROPERTY()
	TEnumAsByte<EStepDeviceKeyID> KeyID;
};



UCLASS(config=StepConfig, defaultconfig)
class UStepSetting : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UStepSetting();

	static UStepSetting* Instance();

	void ReLoadConfig();

	bool IsInit = false;

public:
	UPROPERTY(Config, VisibleAnywhere, Category = StepConfig)
	TArray<int32> ReplicateDeviceID;

	UPROPERTY(config, VisibleAnywhere, Category = StepConfig)
	TArray<FStepDeviceAction> KeyAction;
};
