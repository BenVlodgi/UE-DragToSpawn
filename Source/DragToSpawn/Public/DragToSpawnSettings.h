// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "DragToSpawnHandler.h"
#include "DragToSpawnMapping.h"
#include "DragToSpawnSettings.generated.h"

/**
 * Project settings.
 */
UCLASS(Config = Game, DefaultConfig)
class DRAGTOSPAWN_API UDragToSpawnSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** Registered handlers, evaluated in order. First match wins. */
	UPROPERTY(Config, EditAnywhere, Category = "Drag to Spawn", meta = (NoClear))
	TArray<TSoftClassPtr<UDragToSpawnHandler>> Handlers;

	/** Map draggable type to the spawned actor, and specify the property on the actor to set the dragged asset. Evaluated after handlers. First match wins. */
	UPROPERTY(Config, EditAnywhere, Category = "Drag to Spawn")
	TArray<FDragToSpawnMapping> AssetMappings;

	virtual FName GetContainerName() const override { return TEXT("Project"); }
	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
	virtual FName GetSectionName() const override { return TEXT("DragToSpawn"); }
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
};
