#include "StepVrDataInterface.h"


/************************************************************************/
/* Time                                                                 */
/************************************************************************/
#include "Windows/AllowWindowsPlatformTypes.h"  
#if defined(__MSWINDOWS__) && _MSC_VER < 1900
#define STEP_NEEDS_64_BIT_CLOCK
#include <cmath>
#include <windows.h>
#endif

#if !defined(STEP_NEEDS_64_BIT_CLOCK)
#include <chrono>
#endif
#include "Windows/HideWindowsPlatformTypes.h"  


class FStepTimerInstance : public FStepTimer
{
public:
	~FStepTimerInstance(){}

#if defined(STEP_NEEDS_64_BIT_CLOCK)
	FStepTimerInstance():
		mFrequency(0),
		mInitialTicks(0),
		mInvFrequency(0.0)
	{
		LARGE_INTEGER frequency = { 1 }, counter = { 0 };
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&counter);
		mFrequency = static_cast<int64_t>(frequency.QuadPart);
		mInitialTicks = static_cast<int64_t>(counter.QuadPart);
		mInvFrequency = 1.0 / static_cast<double>(mFrequency);
	}

	int64 GetNanoseconds() override
	{
		int64_t ticks = GetTicks();
		double seconds = mInvFrequency * static_cast<double>(ticks);
		int64_t nanoseconds = static_cast<int64_t>(std::ceil(1000000000.0 * seconds));
		return nanoseconds;
	}

	int64 GetMicroseconds() override
	{
		int64_t ticks = GetTicks();
		double seconds = mInvFrequency * static_cast<double>(ticks);
		int64_t microseconds = static_cast<int64_t>(std::ceil(1000000.0 * seconds));
		return microseconds;
	}

	int64 GetMilliseconds() override
	{
		int64_t ticks = GetTicks();
		double seconds = mInvFrequency * static_cast<double>(ticks);
		int64_t milliseconds = static_cast<int64_t>(std::ceil(1000.0 * seconds));
		return milliseconds;
	}

	double GetSeconds() override
	{
		int64_t ticks = GetTicks();
		double seconds = mInvFrequency * static_cast<double>(ticks);
		return seconds;
	}

	void Reset() override
	{
		LARGE_INTEGER counter = { 0 };
		QueryPerformanceCounter(&counter);
		mInitialTicks = static_cast<int64_t>(counter.QuadPart);
	}

	int64 FStepTimerInstance::GetTicks()
	{
		LARGE_INTEGER counter = { 0 };
		QueryPerformanceCounter(&counter);
		return static_cast<int64_t>(counter.QuadPart) - mInitialTicks;
	}

private:
	int64 GetTicks();

	int64	mFrequency;
	int64	mInitialTicks;
	double	mInvFrequency;

#else
	FStepTimerInstance()
	{
		Reset();
	}

	int64 GetNanoseconds() override
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::nanoseconds>(
			currentTime - mInitialTime).count();
	}


	int64 GetMicroseconds() override
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(
			currentTime - mInitialTime).count();
	}


	int64 GetMilliseconds() override
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			currentTime - mInitialTime).count();
	}


	double GetSeconds() override
	{
		int64_t msecs = GetMilliseconds();
		return static_cast<double>(msecs) / 1000.0;
	}

	void Reset() override
	{
		mInitialTime = std::chrono::high_resolution_clock::now();
	}
private:
	std::chrono::high_resolution_clock::time_point mInitialTime;
#endif

};


TSharedPtr<FStepTimer> FStepTimer::GetTimer()
{
	return MakeShareable(new FStepTimerInstance());
}







/************************************************************************/
/*                         FDeviceFrame                                 */
/************************************************************************/
FDeviceFrame::FDeviceFrame():
	MMAPTicks(0)
{
}

int64 FDeviceFrame::GetInterval()
{
	return (FDateTime::Now().GetTicks() - MMAPTicks) / ETimespan::TicksPerMillisecond;
}

int64 FDeviceFrame::GetMMAPTicks()
{
	return MMAPTicks;
}

void FDeviceFrame::SetTransform(FTransform& InData)
{
	MMAPTicks = FDateTime::Now().GetTicks();
	Transform = InData;
}

void FDeviceFrame::GetTransform(FTransform& OutData) const
{
	OutData = Transform;
}

void FDeviceFrame::SetSpeed(FVector Indata)
{
	Speed = Indata;
}

void FDeviceFrame::SetAcceleration(FVector InData)
{
	Acceleration = InData;
}

void FDeviceFrame::SetPalstance(FVector InData)
{
	Palstance = InData;
}
