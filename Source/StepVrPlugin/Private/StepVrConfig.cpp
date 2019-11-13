#include "StepVrConfig.h"

#include "Misc/Paths.h"
#include "Misc/ConfigCacheIni.h"

#include "CoreGlobals.h"

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
