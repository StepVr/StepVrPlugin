#include "StepVrConfig.h"
#include "Paths.h"
#include "CoreGlobals.h"

UStepSetting::UStepSetting()
{

}

UStepSetting::~UStepSetting()
{

}

void UStepSetting::ReLoadConfig()
{
	FString StepConfigPath = FPaths::ProjectPluginsDir() + TEXT("StepVrPlugin/Config/StepConfig.ini");
	GConfig->LoadFile(StepConfigPath);

	LoadConfig(UStepSetting::StaticClass(),*StepConfigPath);
}
