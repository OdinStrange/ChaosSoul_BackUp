// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ChaosSoul : ModuleRules
{
	public ChaosSoul(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ChaosSoul",
			"ChaosSoul/Variant_Platforming",
			"ChaosSoul/Variant_Platforming/Animation",
			"ChaosSoul/Variant_Combat",
			"ChaosSoul/Variant_Combat/AI",
			"ChaosSoul/Variant_Combat/Animation",
			"ChaosSoul/Variant_Combat/Gameplay",
			"ChaosSoul/Variant_Combat/Interfaces",
			"ChaosSoul/Variant_Combat/UI",
			"ChaosSoul/Variant_SideScrolling",
			"ChaosSoul/Variant_SideScrolling/AI",
			"ChaosSoul/Variant_SideScrolling/Gameplay",
			"ChaosSoul/Variant_SideScrolling/Interfaces",
			"ChaosSoul/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
