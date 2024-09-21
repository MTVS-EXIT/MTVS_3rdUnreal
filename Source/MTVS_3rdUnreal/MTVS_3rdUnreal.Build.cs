// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MTVS_3rdUnreal : ModuleRules
{
	public MTVS_3rdUnreal(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "UMG", 
															"OnlineSubsystem", "OnlineSubsystemSteam", "OnlineSubsystemUtils",
                                                            "HTTP", "Json", "JsonUtilities", "GameplayTasks", "AudioCapture", "AudioMixer", "OpenCVHelper", "OpenCV"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
