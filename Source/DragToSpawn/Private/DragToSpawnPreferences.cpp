// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#include "DragToSpawnPreferences.h"

#define LOCTEXT_NAMESPACE "DragToSpawn"

FText UDragToSpawnPreferences::GetSectionText() const
{
	return LOCTEXT("PreferencesSection", "Drag to Spawn");
}

FText UDragToSpawnPreferences::GetSectionDescription() const
{
	return LOCTEXT("PreferencesDescription", "Developer preferences for Drag to Spawn.");
}

#undef LOCTEXT_NAMESPACE
