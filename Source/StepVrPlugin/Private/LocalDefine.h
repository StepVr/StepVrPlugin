// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "StepVr.h"

#include "CoreMinimal.h"
#include "Logging/LogMacros.h"

#include "LocalDefine.generated.h"

#define StepVrPluginName	"StepVrPlugin"


/*----------------------------------------------------------------------------
	StepVr Node Convert
----------------------------------------------------------------------------*/
#define SDKNODEID(_ID_)	((StepVR::SingleNode::NodeID)(_ID_))
#define SDKKEYID(_ID_)	((StepVR::SingleNode::KeyID)(_ID_))


/*----------------------------------------------------------------------------
	Support Platform
----------------------------------------------------------------------------*/
#define STEPVRSUPPORTPLATFORMS PLATFORM_WINDOWS
#define PLATFORM_WIN64 (PLATFORM_WINDOWS&&PLATFORM_64BITS)
#define PLATFORM_WIN32 (PLATFORM_WINDOWS&&PLATFORM_32BITS)


/*----------------------------------------------------------------------------
	Log category for the StepVr module.
----------------------------------------------------------------------------*/
DECLARE_LOG_CATEGORY_EXTERN(LogStepVrPlugin, Log, All);

#define SAFE_DELETE_NULL(_Point_)	if (_Point_!=nullptr) { delete _Point_;_Point_ = nullptr; }
#define CHECK_BREAK(_Flag_)			if(_Flag_){break;}


/*----------------------------------------------------------------------------
	Enum  & Struct
----------------------------------------------------------------------------*/
UENUM()
namespace StepVrDeviceID {
	enum Type {
		DLeftController		= 0x01,
		DRightController	= 0x02,
		DGun				= 0x04,
		//相机
		DCamera				= 0x05,
		DHead				= 0x06,
		//景深
		DDepth				= 0x07,
		//导演监视器
		DDirMon				= 0x08,

		DBack				= 0x0a,
		DLeftHand			= 0x0b,
		DRightHand			= 0x0c,
		DLeftFoot			= 0x0d,
		DRightFoot			= 0x0e,

		DHMD			= 0x1f,
	};
}

//标准件
extern TMap<int32, FTransform>	GLocalDevicesRT;

//需要获取定位的设备ID
extern TArray<int32>	GNeedUpdateDevices;