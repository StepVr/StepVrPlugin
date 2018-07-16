// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "../Launch/Resources/Version.h"
#include "InputCoreTypes.h"
#include "StepVr.h"


#define StepVrPluginName	"StepVrPlugin"


/*----------------------------------------------------------------------------
	Custom Version
----------------------------------------------------------------------------*/
#define AFTER_ENGINEVERSION_410 ((ENGINE_MAJOR_VERSION==4)&&(ENGINE_MINOR_VERSION>10))
#define AFTER_ENGINEVERSION_411 ((ENGINE_MAJOR_VERSION==4)&&(ENGINE_MINOR_VERSION>11))
#define AFTER_ENGINEVERSION_412 ((ENGINE_MAJOR_VERSION==4)&&(ENGINE_MINOR_VERSION>12))
#define AFTER_ENGINEVERSION_413 ((ENGINE_MAJOR_VERSION==4)&&(ENGINE_MINOR_VERSION>13))
#define AFTER_ENGINEVERSION_414 ((ENGINE_MAJOR_VERSION==4)&&(ENGINE_MINOR_VERSION>14))
#define AFTER_ENGINEVERSION_415 ((ENGINE_MAJOR_VERSION==4)&&(ENGINE_MINOR_VERSION>15))
#define AFTER_ENGINEVERSION_416 ((ENGINE_MAJOR_VERSION>=4)&&(ENGINE_MINOR_VERSION>16))
#define AFTER_ENGINEVERSION_417 ((ENGINE_MAJOR_VERSION>=4)&&(ENGINE_MINOR_VERSION>17))

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

	/**
	*   standard component 
	*/
	enum Type {
		DLeftController		= 0x01,
		DRightController	= 0x02,
		DGun				= 0x04,
		DHead				= 0x06,
		DBack				= 0x0a,
		DLeftHand			= 0x0b,
		DRightHand			= 0x0c,
		DLeftFoot			= 0x0d,
		DRightFoot			= 0x0e,

		DOculusHead			= 0x1f,
	};
}

//标准件
extern TMap<int32, FTransform>	S_mStepVrDeviceState;

//手套
extern TMap<int32, FRotator>	S_mStepVrGloveState;