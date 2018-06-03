// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "License/SDKLic.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "IPluginManager.h"
#include "LocalDefine.h"


/** get gameid */
typedef bool(*_getGameID)(char *buff1, char *buff2,int len);
/** get current cpuid */
typedef bool(*_getCurrentCpuID)(char *buff, int len);
/** get regist cpuid */
typedef bool(*_getRegistCpuID)(char *buff1, char *buff2, int len);


bool FSDKLic::CheckLicIsValid(FString gameID)
{
	bool _flag = false;

	/** Load DLL */
	FString _platform = PLATFORM_WIN64 ? "x64" : "x32";
	FString _dllPath = IPluginManager::Get().FindPlugin(StepVrPluginName)->GetBaseDir() + "/ThirdParty/lib/" + _platform;
	FPlatformProcess::PushDllDirectory(*_dllPath);
	DllHandle = FPlatformProcess::GetDllHandle(*(_dllPath + "/license.dll"));

	do 
	{
		/** Load DLL Method */
		_getGameID _dllGetGameID = (_getGameID)FPlatformProcess::GetDllExport(DllHandle, TEXT("GetGameId"));
		_getCurrentCpuID _dllGetCpuID = (_getCurrentCpuID)FPlatformProcess::GetDllExport(DllHandle, TEXT("GetId"));
		_getRegistCpuID _dllGetRegistCpuID = (_getRegistCpuID)FPlatformProcess::GetDllExport(DllHandle, TEXT("GetPcId"));

		char _registGamID[4];
		char _registCPUID[128];
		char _curCPUID[128];

		bool _f1 = _dllGetGameID(TCHAR_TO_ANSI(*FPaths::ProjectDir()), _registGamID, 4);
		int32 _igameID = FCString::Atoi(ANSI_TO_TCHAR(_registGamID));
		int32 _iregistGameID = FCString::Atoi(*gameID);
		if (_igameID != _iregistGameID)
		{
			break;
		}

		if (_dllGetRegistCpuID(TCHAR_TO_ANSI(*FPaths::ProjectDir()),_registCPUID,128)&&
			_dllGetCpuID(_curCPUID,128)&&
			FString(ANSI_TO_TCHAR(_curCPUID)).Equals(FString(ANSI_TO_TCHAR(_registCPUID))))
		{
			UE_LOG(LogStepVrPlugin, Warning, TEXT("check Lic Success!"));
		}
		else
		{
			UE_LOG(LogStepVrPlugin, Warning, TEXT("Lic Invalid!"));
			break;
		}

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
