#pragma once


#include "cpuid.h"


class FSDKLic
{
public:
	bool CheckLicIsValid(FString gameID);
private:
	void* DllHandle;
};
