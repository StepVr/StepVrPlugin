// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

class FStepVrAuxiliaryUDP;

class FStepVrAuxiliaryCollection
{
public:
	~FStepVrAuxiliaryCollection();
	void StartCollection();
	void StopCollection();
	void EngineBeginFrame();

protected:
	bool NeedCollection();

	TSharedPtr<FStepVrAuxiliaryUDP> StepVrAuxiliaryUDP;
private:
};
