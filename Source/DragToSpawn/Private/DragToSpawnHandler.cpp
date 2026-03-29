// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#include "DragToSpawnHandler.h"

bool UDragToSpawnHandler::CanHandleAsset_Implementation(UObject* Asset) const
{
	return true;
}

void UDragToSpawnHandler::DragStart_Implementation(UObject* Asset, FVector Location, FRotator Rotation) {}

void UDragToSpawnHandler::ActorSpawnedPreConstruction_Implementation(UObject* Asset, AActor* SpawnedActor) {}

void UDragToSpawnHandler::ActorSpawnedPostConstruction_Implementation(UObject* Asset, AActor* SpawnedActor) {}

void UDragToSpawnHandler::DropFinished_Implementation(UObject* Asset, AActor* SpawnedActor) {}
