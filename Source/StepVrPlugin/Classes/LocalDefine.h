// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "StepVr.h"

#include "CoreMinimal.h"
#include "Logging/LogMacros.h"

#include "LocalDefine.generated.h"

#define StepVrPluginName	"StepVrPlugin"

DECLARE_LOG_CATEGORY_EXTERN(LogStepVrPlugin, Log, All);

//Stat stepvr
#define SHOW_STATE	true
DECLARE_STATS_GROUP(TEXT("StepVR"), STATGROUP_StepVR, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("StepVrComponet tick"), Stat_StepVrComponet_tick, STATGROUP_StepVR);
DECLARE_CYCLE_STAT(TEXT("StepVrComponet DeviceTransform"), Stat_StepVrComponetp_DeviceTransform, STATGROUP_StepVR);
DECLARE_CYCLE_STAT(TEXT("StepVrInput ControllerEvent"), Stat_StepVrInput_ControllerEvent, STATGROUP_StepVR);
DECLARE_CYCLE_STAT(TEXT("StepVrGlobal UpdateFrame"), Stat_StepVrGlobal_UpdateFrame, STATGROUP_StepVR);

//DECLARE_STATS_GROUP(TEXT("stepvrserver"), STATGROUP_STEPVRSERVER, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("StepVrData Receive"), Stat_StepVrData_Receive, STATGROUP_StepVR);
DECLARE_CYCLE_STAT(TEXT("StepVrData Send"), Stat_StepVrData_Send, STATGROUP_StepVR);
DECLARE_DWORD_COUNTER_STAT(TEXT("StepVrData RemoteNums"), Statr_StepVrData_RemoteNums, STATGROUP_StepVR);
DECLARE_DWORD_COUNTER_STAT(TEXT("StepVrData Receive Interval (ms)"), Stat_StepVrData_Receive_Interval, STATGROUP_StepVR);

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

		DHMD				= 0x1f,
	};
}