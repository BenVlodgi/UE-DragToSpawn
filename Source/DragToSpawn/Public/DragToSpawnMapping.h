// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "DragToSpawnMapping.generated.h"

/**
 * Mapping for an asset to drag, which spawns an actor and sets a property to the dragged asset.
 */
USTRUCT()
struct DRAGTOSPAWN_API FDragToSpawnMapping
{
	GENERATED_BODY()

	/** Asset type that triggers this mapping. Subclasses also match. */
	UPROPERTY(Config, EditAnywhere, Category = "DragToSpawn", meta = (AllowAbstract = true, DisallowCreateNew, NoClear))
	TSoftClassPtr<UObject> AssetClass;

	/** Actor class placed in the level on drop. */
	UPROPERTY(Config, EditAnywhere, Category = "DragToSpawn", meta = (DisallowCreateNew, NoClear))
	TSoftClassPtr<AActor> ActorClassToSpawn;

	/**
	 * Property on the spawned actor to receive the asset.
	 *
	 * Supports UObject*, TObjectPtr, TSubclassOf, TSoftObjectPtr, and TSoftClassPtr.
	 * Object properties get the asset instance (or CDO for Blueprints).
	 * Class properties get the asset's UClass.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "DragToSpawn")
	FName PropertyName;
};
