// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActorFactories/ActorFactory.h"

#include "DragToSpawnCompatibility.h"
#include "DragToSpawnActorFactory.generated.h"

class UDragToSpawnHandler;
struct FDragToSpawnMapping;

/**
 * Actor factory that intercepts content browser drag-and-drop and delegates to registered handlers or asset mappings.
 */
UCLASS()
class DRAGTOSPAWN_API UDragToSpawnActorFactory : public UActorFactory
{
	GENERATED_BODY()

public:
	UDragToSpawnActorFactory();

	virtual bool CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) override;
#if ENGINE_MAJOR_VERSION >= 5
	virtual UClass* GetDefaultActorClass(const FAssetData& AssetData) override;
#endif

protected:
	virtual AActor* SpawnActor(UObject* InAsset, ULevel* InLevel, const FTransform& InTransform, const FActorSpawnParameters& InSpawnParams) override;
	virtual void PostSpawnActor(UObject* Asset, AActor* NewActor) override;

private:
	/** The active handler instance for the current CreateActor call. */
	TObjectPtr<UDragToSpawnHandler> ActiveHandler;

	/** The resolved asset (CDO for Objects) passed to handler events. */
	TObjectPtr<UObject> ActiveAsset;

	UClass* FindHandlerClassForAssetClass(UClass* InAssetClass, UObject* Asset = nullptr) const;
	const FDragToSpawnMapping* FindAssetMappingForAssetClass(UClass* InAssetClass) const;
	static void EmplaceAssetInProperty(AActor* Actor, UObject* Asset, UClass* AssetClass, FName PropertyName);
};
