#include "LocalDefine.h"

DEFINE_LOG_CATEGORY(LogStepVrPlugin);

/* StepVrDevice All Data*/
TMap<int32, FTransform>	S_mStepVrDeviceState;
TArray<int32>	GNeedUpdateDevices;