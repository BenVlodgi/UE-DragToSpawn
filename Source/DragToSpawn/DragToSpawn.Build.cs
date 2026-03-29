// Copyright 2026 Dream Seed LLC. All Rights Reserved.

using UnrealBuildTool;

public class DragToSpawn : ModuleRules
{
	public DragToSpawn(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"UnrealEd",
			"DeveloperSettings",
			"InputCore",
			"PropertyEditor",
			"Slate",
			"SlateCore",
			"Settings",
		});

		// UE 5.0 and earlier: IPropertyTypeCustomizationUtils uses FEditorStyle.
		if (Target.Version.MajorVersion < 5 || (Target.Version.MajorVersion == 5 && Target.Version.MinorVersion < 1))
		{
			PrivateDependencyModuleNames.Add("EditorStyle");
		}
	}
}
