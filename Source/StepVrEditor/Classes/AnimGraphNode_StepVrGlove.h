// Copyright (C) 2006-2016, IKinema Ltd. All rights reserved.
#pragma once

#include "AnimNode_StepVrGlove.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimGraphNode_StepVrGlove.generated.h"

UCLASS(MinimalAPI)
class UAnimGraphNode_StepVrGlove : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Settings)
	FAnimNode_StepVrGlove Node;

protected:
	// UEdGraphNode interface  
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	// End of UEdGraphNode interface  

	virtual void ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog) override;

	virtual FText GetControllerDescription() const override;
 
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }

};


