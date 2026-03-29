// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "DragToSpawnHandler.generated.h"

/**
 * Base class for defining what happens when an asset is dragged into the viewport.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class DRAGTOSPAWN_API UDragToSpawnHandler : public UObject
{
	GENERATED_BODY()

public:
	/** Asset types this handler responds to. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DragToSpawn", meta = (NoClear))
	TArray<TSoftClassPtr<UObject>> SupportedAssetClasses;

	/** Actor class that spawns in the level on drop. Leave empty to skip spawning. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DragToSpawn", meta = (NoClear))
	TSoftClassPtr<AActor> ActorClassToSpawn;

	/**
	 * Extra validation before the spawn sequence begins. Return false to cancel.
	 * This is called *statically* on the CDO.
	 * 
	 * @param Asset The asset being dragged (CDO for Objects).
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "DragToSpawn")
	bool CanHandleAsset(UObject* Asset) const;
	virtual bool CanHandleAsset_Implementation(UObject* Asset) const;

	/**
	 * Fires when dragging starts before the actor is created.
	 * 
	 * @param Asset The asset being dragged (CDO for Objects).
	 * @param Location World position of the drop point.
	 * @param Rotation World rotation at the drop point.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "DragToSpawn")
	void DragStart(UObject* Asset, FVector Location, FRotator Rotation);
	virtual void DragStart_Implementation(UObject* Asset, FVector Location, FRotator Rotation);

	/**
	 * Fires after the actor is spawned (SpawnActor), before the construction script runs. Set variables here.
	 * 
	 * @param Asset The asset being dragged (CDO for Objects).
	 * @param SpawnedActor The newly created actor with deferred construction.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "DragToSpawn")
	void ActorSpawnedPreConstruction(UObject* Asset, AActor* SpawnedActor);
	virtual void ActorSpawnedPreConstruction_Implementation(UObject* Asset, AActor* SpawnedActor);

	/**
	 * Fires after the actor's construction script runs (FinishSpawning).
	 * 
	 * @param Asset The asset being dragged (CDO for Objects).
	 * @param SpawnedActor The fully constructed actor.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "DragToSpawn")
	void ActorSpawnedPostConstruction(UObject* Asset, AActor* SpawnedActor);
	virtual void ActorSpawnedPostConstruction_Implementation(UObject* Asset, AActor* SpawnedActor);

	/**
	 * Fires once the user releases the dragged asset.
	 * 
	 * @param Asset The asset being dragged (CDO for Objects).
	 * @param SpawnedActor The spawned actor, or null if spawning was skipped.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "DragToSpawn")
	void DropFinished(UObject* Asset, AActor* SpawnedActor);
	virtual void DropFinished_Implementation(UObject* Asset, AActor* SpawnedActor);
};
