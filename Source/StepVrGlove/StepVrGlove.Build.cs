// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

public class StepVrGlove : ModuleRules
{
    public StepVrGlove(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bFasterWithoutUnity = true;

        //�ⲿ����ͷ�ļ�Ŀ¼
        PublicIncludePaths.AddRange(
			new string[] {
				"StepVrGlove/Public",
				// ... add public include paths required here ...
			}
			);
				
		
        //��ģ������ͷ�ļ�·��
		PrivateIncludePaths.AddRange(
			new string[] {
				"StepVrGlove/Private",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Projects",
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "CoreUObject",
                "Engine",
                "Projects",
                "InputCore",
                "AnimGraphRuntime",
                "StepVrPlugin"
            }
			);
    }
}
