// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "License/SDKLic.h"
#include "Runtime/Core/Public/Misc/Paths.h"
//#include "IPluginManager.h"
#include "LocalDefine.h"


/** get gameid */
typedef bool(*_getGameID)(char *buff1, char *buff2,int len);
/** get regist cpuid */
typedef bool(*_getRegistCpuID)(char *buff1, char *buff2, int len);
/** get Compare */
typedef bool(*_getCompare)(char *buff1);

bool FSDKLic::CheckLicIsValid(FString gameID)
{
	bool _flag = false;

	/** Load DLL */
	FString _platform = PLATFORM_WIN64 ? "x64" : "x32";
	FString _dllPath = FPaths::ProjectPluginsDir() + TEXT("StepVrPlugin/ThirdParty/lib/") + _platform;
	FPlatformProcess::PushDllDirectory(*_dllPath);
	DllHandle = FPlatformProcess::GetDllHandle(*(_dllPath + "/license.dll"));

	do 
	{
		if (DllHandle == nullptr)
		{
			break;
		}

		/** Load DLL Method */
		_getGameID _dllGetGameID = (_getGameID)FPlatformProcess::GetDllExport(DllHandle, TEXT("GetGameId"));
		_getCompare _dllGetCompare = (_getCompare)FPlatformProcess::GetDllExport(DllHandle, TEXT("Compare"));
		_getRegistCpuID _dllGetRegistCpuID = (_getRegistCpuID)FPlatformProcess::GetDllExport(DllHandle, TEXT("GetPcId"));

		char _registGamID[4];

		//传bin路径
		bool _f1 = _dllGetGameID(TCHAR_TO_ANSI(*FPaths::ProjectDir()), _registGamID, 4);
		int32 _igameID = FCString::Atoi(ANSI_TO_TCHAR(_registGamID));

		//传入GameID进行匹配
		int32 _iregistGameID = FCString::Atoi(*gameID);
		if (_igameID != _iregistGameID)
		{
			break;
		}

		char _registCPUID[128];
		if (!_dllGetRegistCpuID(TCHAR_TO_ANSI(*FPaths::ProjectDir()), _registCPUID, 128))
		{
			break;
		}

		if (!_dllGetCompare(_registCPUID))
		{
			UE_LOG(LogStepVrPlugin, Warning, TEXT("Lic Invalid!"));
			break;
			
		}


		UE_LOG(LogStepVrPlugin, Warning, TEXT("check Lic Success!"));
		_flag = true;
	} while (0);

	UE_LOG(LogStepVrPlugin, Warning, TEXT("path = %s"), *FPaths::ProjectDir());
	
	/** Free DLL */
	FPlatformProcess::PopDllDirectory(*_dllPath);
	if (DllHandle)
	{
		FPlatformProcess::FreeDllHandle(DllHandle);
		DllHandle = nullptr;
	}

	return _flag;
}
