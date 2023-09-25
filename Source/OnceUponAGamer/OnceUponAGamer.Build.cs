// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OnceUponAGamer : ModuleRules
{
	public OnceUponAGamer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" ,"GameplayTasks","NavigationSystem", "PhysicsCore","DeveloperSettings" });
	}
}
