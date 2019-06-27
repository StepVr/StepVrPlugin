#include "LocalDefine.h"

DEFINE_LOG_CATEGORY(LogStepVrPlugin);

/* StepVrDevice All Data*/
TMap<int32, FTransform>	GLocalDevicesRT;
TArray<int32>	GNeedUpdateDevices = { StepVrDeviceID::DHead };
TArray<int32>	GNeedUpdateGlobalDevices = {};