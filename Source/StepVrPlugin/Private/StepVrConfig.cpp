#include "StepVrConfig.h"
#include "Paths.h"
#include "CoreGlobals.h"
#include "Misc/ConfigCacheIni.h"

UStepSetting::UStepSetting()
{

}

UStepSetting* UStepSetting::Instance()
{
	UStepSetting* Config = UStepSetting::StaticClass()->GetDefaultObject<UStepSetting>();
	
	if (!Config->IsInit)
	{
		Config->ReLoadConfig();
	}

	return Config;
}


void UStepSetting::ReLoadConfig()
{
	FString StepConfigPath = FPaths::ProjectPluginsDir() + TEXT("StepVrPlugin/Config/StepConfig.ini");
	GConfig->LoadFile(StepConfigPath); 

	LoadConfig(UStepSetting::StaticClass(),*StepConfigPath);

	IsInit = true;
}
