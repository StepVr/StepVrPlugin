// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

public class StepVrPlugin : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }
    private string LibPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty")); }
    }
    public StepVrPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        OptimizeCode = CodeOptimization.InShippingBuildsOnly;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        //外部链接头文件目录
        PublicIncludePaths.AddRange(
			new string[] {
				"StepVrPlugin/Public",
				// ... add public include paths required here ...
			}
			);
				
		
        //本模块连接头文件路径
		PrivateIncludePaths.AddRange(
			new string[] {
				"StepVrPlugin/Private",
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
                "InputDevice",
                "HeadMountedDisplay",
                "Sockets",
                "Slate"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        bool IsLibrarySupport = false;
        string LibrariesPath;

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            IsLibrarySupport = true;

            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x32";
            LibrariesPath = Path.Combine(LibPath, "lib", PlatformString);

            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "StepVr.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "license.lib"));

            PublicDelayLoadDLLs.Add("StepVR.dll");
            PublicDelayLoadDLLs.Add("license.dll");
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(LibrariesPath, "StepVR.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(LibrariesPath, "license.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(LibrariesPath, "libcurl.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(LibrariesPath, "StepVr.lib")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(LibrariesPath, "license.lib")));
        }

        if (IsLibrarySupport)
        {
            PrivateIncludePaths.Add(Path.Combine(LibPath, "include"));
            PublicIncludePaths.Add(Path.Combine(LibPath, "Include"));
        }
    }
}
