#pragma once

#include "Runtime/Core/Public/Containers/UnrealString.h"
#include "cpuid.h"



class FSDKLic
{
public:
	bool CheckLicIsValid(FString gameID);
private:
	void* DllHandle;
};
