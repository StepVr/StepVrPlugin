// Copyright (C) 2006-2017, IKinema Ltd. All rights reserved.

#include "AnimGraphNode_StepVrGlove.h"
#include "CompilerResultsLog.h"
#include "AnimNodeEditModes.h"

FText UAnimGraphNode_StepVrGlove::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Stepvr Glove Modify");
}


void UAnimGraphNode_StepVrGlove::ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog)
{
	bool Flag = false;
	do 
	{
		if (Node.LeftBoneToModifys.Num() != 6)
		{
			break;
		}
		if (Node.RightBoneToModifys.Num() != 6)
		{
			break;
		}

		bool IsHaveUnBind = false;
		for (FBoneReference& Temp : Node.LeftBoneToModifys)
		{
			if (Temp.BoneName == NAME_None)
			{
				IsHaveUnBind = true;
				break;
			}
		}
		if (IsHaveUnBind)
		{
			break;
		}

		IsHaveUnBind = false;
		for (FBoneReference& Temp : Node.RightBoneToModifys)
		{
			if (Temp.BoneName == NAME_None)
			{
				IsHaveUnBind = true;
				break;
			}
		}
		if (IsHaveUnBind)
		{
			break;
		}

		Flag = true;
	} while (0);

	if (!Flag)
	{
		MessageLog.Warning(TEXT("@@ - LeftBoneToModifys | RightBoneToModifys 必须绑定6个手指关节."), this);
	}
}

FText UAnimGraphNode_StepVrGlove::GetControllerDescription() const
{
	return FText::FromString("Stepvr Glove 2222222222");
}

FText UAnimGraphNode_StepVrGlove::GetTooltipText() const
{
	return FText::FromString("Stepvr Glove Modify");
}