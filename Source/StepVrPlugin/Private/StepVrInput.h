// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "IInputDevice.h"
#include "XRMotionControllerBase.h"
#include "StepVrInputState.h"



typedef TMap<uint8, int32> MotionSet;

class STEPVRPLUGIN_API FStepVrInput : public IInputDevice ,public FXRMotionControllerBase/* ,public IHapticDevice*/
{
public:
	FStepVrInput(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler);
	~FStepVrInput();
	virtual void Tick(float DeltaTime) override;

#if (AFTER_ENGINEVERSION_411)
	virtual bool IsGamepadAttached() const override
	{
		return true;
	}
#endif

	/** Poll for controller state and send events if needed */
	virtual void SendControllerEvents() override;

	/** Set which MessageHandler will get the events from SendControllerEvents. */
	virtual void SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override;

	/** Exec handler to allow console commands to be passed through for debugging */
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

	/**
	* IForceFeedbackSystem pass through functions
	*/
	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values) override;


	/*----------------------------------------------------------------------------
		ImotionController Delegate
		PS:4.15 disable oculus inputPlugin,The engine will use oculus motionController before stepvr
	----------------------------------------------------------------------------*/

	virtual ETrackingStatus GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const override
	{
		return ETrackingStatus::Tracked;
	}

#if AFTER_ENGINEVERSION_415
	virtual bool GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition, float WorldToMetersScale) const override;
#else
	virtual bool GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition) const override;
#endif
	bool GetOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition) const;

#if AFTER_ENGINEVERSION_416
	static FName S_DeviceTypeName;
	virtual FName GetMotionControllerDeviceTypeName() const override;
#endif



	/*----------------------------------------------------------------------------
		IHapticDevice delegate 
	----------------------------------------------------------------------------*/
	//virtual void SetHapticFeedbackValues(int32 ControllerId, int32 Hand, const FHapticFeedbackValues& Values) override;

	/**
	* Determines the valid range of frequencies this haptic device supports, to limit input ranges from UHapticFeedbackEffects
	*/
	//virtual void GetHapticFrequencyRange(float& MinFrequency, float& MaxFrequency) const override;

	/**
	* Returns the scaling factor to map the amplitude of UHapticFeedbackEvents from [0.0, 1.0] to the actual range handled by the device
	*/
	//virtual float GetHapticAmplitudeScale() const override;
private:
	bool RegisterDeviceKey();
	void RegisterMotionPair();
protected:
	TSharedRef<FGenericApplicationMessageHandler> MessageHandler;

	FStepVrStateController	ButtonState;

	double					m_fBtnRepeatTime;

	void*					m_HandleDLL;
	MotionSet				m_MotionPair;
};