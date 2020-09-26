#include "Auxiliary/StepVrAuxiliaryCollection.h"
#include "Auxiliary/StepVrAuxiliaryUDP.h"
#include "StepVrGlobal.h"
#include "LocalDefine.h"



FStepVrAuxiliaryCollection::~FStepVrAuxiliaryCollection()
{
	
}

void FStepVrAuxiliaryCollection::StartCollection()
{
	if (!StepVrAuxiliaryUDP.IsValid())
	{
		StepVrAuxiliaryUDP = MakeShared<FStepVrAuxiliaryUDP>();
		StepVrAuxiliaryUDP->StartServer();
	}
}

void FStepVrAuxiliaryCollection::StopCollection()
{

}


void FStepVrAuxiliaryCollection::EngineBeginFrame()
{
	if (NeedCollection())
	{
		FDeviceFrame& DeviceFrame = STEPVR_GLOBAL->GetDeviceFrame();
		
		FAuxiliaryDevice AuxiliaryDevice;

		//头信息
		auto HeadFrame = DeviceFrame.GetDevice(StepVrDeviceID::DHead);
		AuxiliaryDevice.Head = HeadFrame.GetTransform().GetLocation();
		AuxiliaryDevice.bHead = HeadFrame.ISLink();

		//枪信息
		auto GunFrame = DeviceFrame.GetDevice(StepVrDeviceID::DGun);
		AuxiliaryDevice.Gun = GunFrame.GetTransform().GetLocation();
		AuxiliaryDevice.bGun = GunFrame.ISLink();

		//偏移


		//帧号
		AuxiliaryDevice.TotalFrames = GFrameCounter;
		if (GWorld)
		{
			AuxiliaryDevice.FrameTimes = GWorld->GetDeltaSeconds();
		}

		StepVrAuxiliaryUDP->SendDeviceData(AuxiliaryDevice);
	}
}

bool FStepVrAuxiliaryCollection::NeedCollection()
{
	return StepVrAuxiliaryUDP.IsValid() && (StepVrAuxiliaryUDP->GetSendType() == EType_Connect);
}
