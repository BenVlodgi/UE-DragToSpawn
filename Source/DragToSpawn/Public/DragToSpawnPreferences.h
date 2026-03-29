// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "DragToSpawnPreferences.generated.h"

/**
 * Developer preferences.
 */
UCLASS(Config = EditorPerProjectUserSettings, DefaultConfig)
class DRAGTOSPAWN_API UDragToSpawnPreferences : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** Enables or disables drag-to-spawn for this developer. */
	UPROPERTY(Config, EditAnywhere, Category = "General")
	bool bPluginEnabled = true;

	virtual FName GetContainerName() const override { return TEXT("Editor"); }
	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
	virtual FName GetSectionName() const override { return TEXT("DragToSpawn"); }
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
};
