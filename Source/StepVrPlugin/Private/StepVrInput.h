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

	/**
	 * IInputDevice Delegate
	 */
	virtual void Tick(float DeltaTime) override;
	/** Poll for controller state and send events if needed */
	virtual void SendControllerEvents() override;
	/** Set which MessageHandler will get the events from SendControllerEvents. */
	virtual void SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override;
	/** Exec handler to allow console commands to be passed through for debugging */
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
	//IForceFeedbackSystem pass through functions
	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values) override;
	virtual bool IsGamepadAttached() const override;

	/**
	 * FXRMotionControllerBase Delegate
	 */
	// Original GetControllerOrientationAndPosition signature for backwards compatibility 
	virtual bool GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition, float WorldToMetersScale) const;
	// Original GetControllerTrackingStatus signature for backwards compatibility
	virtual ETrackingStatus GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const;

	/**
	 * IMotionController Delegate
	 */
	virtual FName GetMotionControllerDeviceTypeName() const override;

private:
	bool GetOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition) const;
	void RegisterDeviceKey();
	void RegisterMotionPair();

	/**
	 * 初始化数据
	 */
	void Initialize();

protected:
	TSharedRef<FGenericApplicationMessageHandler> MessageHandler;

	FStepVrStateController	StateController;

	double					m_fBtnRepeatTime;

	void*					m_HandleDLL;
	MotionSet				m_MotionPair;
};