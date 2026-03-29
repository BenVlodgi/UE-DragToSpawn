// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#include "DragToSpawnActorFactory.h"
#include "DragToSpawnHandler.h"
#include "DragToSpawnPreferences.h"
#include "DragToSpawnSettings.h"

#include "AssetRegistry/AssetData.h"
#include "Blueprint/BlueprintSupport.h"
#include "Engine/Blueprint.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UnrealType.h"
#include "Engine/Level.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Templates/Casts.h"
#include "DragToSpawnCompatibility.h"

#define LOCTEXT_NAMESPACE "DragToSpawn"

DEFINE_LOG_CATEGORY_STATIC(LogDragToSpawn, Log, All);

/**
 * For Blueprint assets, resolves the generated class (preserving the full BP parent chain) or falls back to the native parent.
 * For non-Blueprints, returns GetClass().
 */
static UClass* ResolveLogicalAssetClass(const FAssetData& AssetData)
{
	UClass* AssetClass = AssetData.GetClass();
	if (!AssetClass)
	{
		return nullptr;
	}

	if (!AssetClass->IsChildOf(UBlueprint::StaticClass()))
	{
		return AssetClass;
	}

	const FString GeneratedClassPathString = AssetData.GetTagValueRef<FString>(FBlueprintTags::GeneratedClassPath);
	if (!GeneratedClassPathString.IsEmpty())
	{
		FString CleanPath = GeneratedClassPathString;
		ConstructorHelpers::StripObjectClass(CleanPath);
		if (UClass* GeneratedClass = FindObject<UClass>(nullptr, *CleanPath))
		{
			return GeneratedClass;
		}
		if (UClass* GeneratedClass = LoadObject<UClass>(nullptr, *CleanPath))
		{
			return GeneratedClass;
		}
	}

	const FString NativeParentClassPathString = AssetData.GetTagValueRef<FString>(FBlueprintTags::NativeParentClassPath);
	if (!NativeParentClassPathString.IsEmpty())
	{
		FString CleanPath = NativeParentClassPathString;
		ConstructorHelpers::StripObjectClass(CleanPath);
		if (UClass* NativeParent = FindObject<UClass>(nullptr, *CleanPath))
		{
			return NativeParent;
		}
		if (UClass* NativeParent = LoadObject<UClass>(nullptr, *CleanPath))
		{
			return NativeParent;
		}
	}

	return AssetClass;
}

/**
 * Like ResolveLogicalAssetClass, but works on a loaded UObject instead of FAssetData.
 */
static UClass* ResolveLogicalClassForObject(UObject* InObject)
{
	if (!InObject)
	{
		return nullptr;
	}

	if (UBlueprint* Blueprint = Cast<UBlueprint>(InObject))
	{
		return Blueprint->GeneratedClass ? static_cast<UClass*>(Blueprint->GeneratedClass) : InObject->GetClass();
	}

	return InObject->GetClass();
}

/**
 * For Blueprint assets, returns the generated class CDO. For everything else, returns the object as-is.
 */
static UObject* ResolveAssetForHandler(UObject* InAsset)
{
	if (UBlueprint* Blueprint = Cast<UBlueprint>(InAsset))
	{
		if (Blueprint->GeneratedClass)
		{
			return Blueprint->GeneratedClass->GetDefaultObject();
		}
	}

	return InAsset;
}

UDragToSpawnActorFactory::UDragToSpawnActorFactory()
{
	DisplayName = LOCTEXT("FactoryDisplayName", "DragToSpawn Actor");
	NewActorClass = AActor::StaticClass();
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 5, 0)
	bShouldAutoRegister = true;
#endif
	MenuPriority = 200;
}

bool UDragToSpawnActorFactory::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
	const UDragToSpawnPreferences* Preferences = GetDefault<UDragToSpawnPreferences>();
	if (!Preferences || !Preferences->bPluginEnabled)
	{
		return false;
	}

	UClass* LogicalClass = ResolveLogicalAssetClass(AssetData);
	if (!LogicalClass)
	{
		return false;
	}

	if (FindHandlerClassForAssetClass(LogicalClass))
	{
		return true;
	}

	if (FindAssetMappingForAssetClass(LogicalClass))
	{
		return true;
	}

	return false;
}

#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 0, 0)
UClass* UDragToSpawnActorFactory::GetDefaultActorClass(const FAssetData& AssetData)
{
	UClass* LogicalClass = ResolveLogicalAssetClass(AssetData);
	if (!LogicalClass)
	{
		return Super::GetDefaultActorClass(AssetData);
	}

	// Check handlers first.
	if (UClass* HandlerClass = FindHandlerClassForAssetClass(LogicalClass))
	{
		const UDragToSpawnHandler* HandlerCDO = GetDefault<UDragToSpawnHandler>(HandlerClass);
		if (HandlerCDO && !HandlerCDO->ActorClassToSpawn.IsNull())
		{
			UClass* ActorClass = HandlerCDO->ActorClassToSpawn.LoadSynchronous();
			return ActorClass ? ActorClass : AActor::StaticClass();
		}

		return AActor::StaticClass();
	}

	// Then asset mappings.
	if (const FDragToSpawnMapping* Mapping = FindAssetMappingForAssetClass(LogicalClass))
	{
		if (!Mapping->ActorClassToSpawn.IsNull())
		{
			UClass* ActorClass = Mapping->ActorClassToSpawn.LoadSynchronous();
			return ActorClass ? ActorClass : AActor::StaticClass();
		}

		return AActor::StaticClass();
	}

	return Super::GetDefaultActorClass(AssetData);
}
#endif

AActor* UDragToSpawnActorFactory::SpawnActor(UObject* InAsset, ULevel* InLevel, const FTransform& InTransform, const FActorSpawnParameters& InSpawnParams)
{
	const UDragToSpawnPreferences* Preferences = GetDefault<UDragToSpawnPreferences>();
	if (!Preferences || !Preferences->bPluginEnabled || !InAsset)
	{
		return nullptr;
	}

	UClass* LogicalClass = ResolveLogicalClassForObject(InAsset);
	UObject* AssetForHandler = ResolveAssetForHandler(InAsset);

	// Try handlers first.
	if (UClass* HandlerClass = FindHandlerClassForAssetClass(LogicalClass, AssetForHandler))
	{
		UDragToSpawnHandler* Handler = NewObject<UDragToSpawnHandler>(GetTransientPackage(), HandlerClass, NAME_None, RF_Transient);

		const FVector Location = InTransform.GetLocation();
		const FRotator Rotation = InTransform.GetRotation().Rotator();
		UClass* ActorClass = Handler->ActorClassToSpawn.LoadSynchronous();

		Handler->DragStart(AssetForHandler, Location, Rotation);

		if (!ActorClass)
		{
			Handler->DropFinished(AssetForHandler, nullptr);
			return nullptr;
		}

		ULevel* LocalLevel = ValidateSpawnActorLevel(InLevel, InSpawnParams);
		if (!LocalLevel)
		{
			UE_LOG(LogDragToSpawn, Warning, TEXT("SpawnActor: could not resolve spawn level for %s"), *AssetForHandler->GetName());
			Handler->DropFinished(AssetForHandler, nullptr);
			return nullptr;
		}

		FActorSpawnParameters SpawnParams = InSpawnParams;
		SpawnParams.bDeferConstruction = true;
		SpawnParams.OverrideLevel = LocalLevel;

		AActor* SpawnedActor = LocalLevel->GetWorld()->SpawnActor<AActor>(ActorClass, InTransform, SpawnParams);
		if (!SpawnedActor)
		{
			UE_LOG(LogDragToSpawn, Warning, TEXT("SpawnActor: SpawnActor failed for class %s"), *ActorClass->GetName());
			Handler->DropFinished(AssetForHandler, nullptr);
			return nullptr;
		}

		Handler->ActorSpawnedPreConstruction(AssetForHandler, SpawnedActor);
		SpawnedActor->FinishSpawning(InTransform, /*bIsDefaultTransform=*/ false);

		// Store for PostSpawnActor, which the base CreateActor calls later.
		// Root the handler to protect against GC triggered from Blueprint callbacks.
		Handler->AddToRoot();
		ActiveHandler = Handler;
		ActiveAsset = AssetForHandler;

		return SpawnedActor;
	}

	// Try asset mappings...
	const FDragToSpawnMapping* Mapping = FindAssetMappingForAssetClass(LogicalClass);
	if (!Mapping)
	{
		UE_LOG(LogDragToSpawn, Warning, TEXT("SpawnActor: no handler or mapping found for %s"), *InAsset->GetName());
		return nullptr;
	}

	UClass* ActorClass = Mapping->ActorClassToSpawn.LoadSynchronous();
	if (!ActorClass)
	{
		return nullptr;
	}

	ULevel* LocalLevel = ValidateSpawnActorLevel(InLevel, InSpawnParams);
	if (!LocalLevel)
	{
		UE_LOG(LogDragToSpawn, Warning, TEXT("SpawnActor: could not resolve spawn level for %s"), *AssetForHandler->GetName());
		return nullptr;
	}

	FActorSpawnParameters SpawnParams = InSpawnParams;
	SpawnParams.bDeferConstruction = true;
	SpawnParams.OverrideLevel = LocalLevel;

	AActor* SpawnedActor = LocalLevel->GetWorld()->SpawnActor<AActor>(ActorClass, InTransform, SpawnParams);
	if (!SpawnedActor)
	{
		UE_LOG(LogDragToSpawn, Warning, TEXT("SpawnActor: SpawnActor failed for class %s"), *ActorClass->GetName());
		return nullptr;
	}

	if (!Mapping->PropertyName.IsNone())
	{
		EmplaceAssetInProperty(SpawnedActor, AssetForHandler, LogicalClass, Mapping->PropertyName);
	}

	SpawnedActor->FinishSpawning(InTransform, /*bIsDefaultTransform=*/ false);

	return SpawnedActor;
}

void UDragToSpawnActorFactory::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	Super::PostSpawnActor(Asset, NewActor);

	if (ActiveHandler)
	{
		ActiveHandler->ActorSpawnedPostConstruction(ActiveAsset, NewActor);
		ActiveHandler->DropFinished(ActiveAsset, NewActor);
		ActiveHandler->RemoveFromRoot();
		ActiveHandler = nullptr;
		ActiveAsset = nullptr;
	}
}

UClass* UDragToSpawnActorFactory::FindHandlerClassForAssetClass(UClass* InAssetClass, UObject* Asset) const
{
	const UDragToSpawnSettings* Settings = GetDefault<UDragToSpawnSettings>();
	if (!Settings || !InAssetClass)
	{
		return nullptr;
	}

	for (const TSoftClassPtr<UDragToSpawnHandler>& SoftHandlerClass : Settings->Handlers)
	{
		if (SoftHandlerClass.IsNull())
		{
			continue;
		}

		UClass* HandlerClass = SoftHandlerClass.LoadSynchronous();
		if (!HandlerClass)
		{
			continue;
		}

		const UDragToSpawnHandler* HandlerCDO = GetDefault<UDragToSpawnHandler>(HandlerClass);
		if (!HandlerCDO)
		{
			continue;
		}

		for (const TSoftClassPtr<UObject>& SoftSupportedClass : HandlerCDO->SupportedAssetClasses)
		{
			if (SoftSupportedClass.IsNull())
			{
				continue;
			}

			UClass* SupportedClass = SoftSupportedClass.LoadSynchronous();
			if (SupportedClass && InAssetClass->IsChildOf(SupportedClass))
			{
				if (Asset && !HandlerCDO->CanHandleAsset(Asset))
				{
					continue;
				}

				return HandlerClass;
			}
		}
	}

	return nullptr;
}

const FDragToSpawnMapping* UDragToSpawnActorFactory::FindAssetMappingForAssetClass(UClass* InAssetClass) const
{
	const UDragToSpawnSettings* Settings = GetDefault<UDragToSpawnSettings>();
	if (!Settings || !InAssetClass)
	{
		return nullptr;
	}

	for (const FDragToSpawnMapping& Mapping : Settings->AssetMappings)
	{
		if (Mapping.AssetClass.IsNull() || Mapping.ActorClassToSpawn.IsNull())
		{
			continue;
		}

		UClass* SupportedClass = Mapping.AssetClass.LoadSynchronous();
		if (SupportedClass && InAssetClass->IsChildOf(SupportedClass))
		{
			return &Mapping;
		}
	}

	return nullptr;
}

void UDragToSpawnActorFactory::EmplaceAssetInProperty(AActor* Actor, UObject* Asset, UClass* AssetClass, FName PropertyName)
{
	if (!Actor || !Asset || PropertyName.IsNone())
	{
		return;
	}

	FProperty* Property = Actor->GetClass()->FindPropertyByName(PropertyName);
	if (!Property)
	{
		UE_LOG(LogDragToSpawn, Warning, TEXT("EmplaceAssetInProperty: property '%s' not found on %s"), *PropertyName.ToString(), *Actor->GetClass()->GetName());
		return;
	}

	// TSoftClassPtr<T>
	if (FSoftClassProperty* SoftClassProperty = CastField<FSoftClassProperty>(Property))
	{
		void* ValuePointer = SoftClassProperty->ContainerPtrToValuePtr<void>(Actor);
		*static_cast<FSoftObjectPtr*>(ValuePointer) = FSoftObjectPtr(AssetClass);
		return;
	}

	// TSubclassOf<T> and UClass*
	if (FClassProperty* ClassProperty = CastField<FClassProperty>(Property))
	{
		ClassProperty->SetObjectPropertyValue_InContainer(Actor, AssetClass);
		return;
	}

	// TSoftObjectPtr<T>
	if (FSoftObjectProperty* SoftObjectProperty = CastField<FSoftObjectProperty>(Property))
	{
		void* ValuePointer = SoftObjectProperty->ContainerPtrToValuePtr<void>(Actor);
		*static_cast<FSoftObjectPtr*>(ValuePointer) = FSoftObjectPtr(Asset);
		return;
	}

	// TObjectPtr<T> and UObject*
	if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
	{
		ObjectProperty->SetObjectPropertyValue_InContainer(Actor, Asset);
		return;
	}

	UE_LOG(LogDragToSpawn, Warning, TEXT("EmplaceAssetInProperty: property '%s' on %s is not a supported type (object, class, soft object, or soft class)"), *PropertyName.ToString(), *Actor->GetClass()->GetName());
}

#undef LOCTEXT_NAMESPACE
