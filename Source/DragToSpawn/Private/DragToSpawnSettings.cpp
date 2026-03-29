// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#include "DragToSpawnSettings.h"

#define LOCTEXT_NAMESPACE "DragToSpawn"

FText UDragToSpawnSettings::GetSectionText() const
{
	return LOCTEXT("SettingsSection", "Drag to Spawn");
}

FText UDragToSpawnSettings::GetSectionDescription() const
{
	return LOCTEXT("SettingsDescription", "Configure which handlers respond to content browser drag-and-drop.");
}

#undef LOCTEXT_NAMESPACE
