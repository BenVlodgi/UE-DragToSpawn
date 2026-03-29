// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"

class SComboButton;

/** Displays FDragToSpawnMapping as a single row: [AssetClass] - - - > [ActorClassToSpawn] . [PropertyName dropdown] */
class FDragToSpawnMappingCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

private:
	/** Rebuild the list of compatible property names from the current ActorClassToSpawn and AssetClass. */
	void RebuildPropertyNameOptions();
	
	void OnPropertyNameSelected(TSharedPtr<FName> PropertyName);
	FText GetPropertyNameText() const;
	void OnPropertyNameTextCommitted(const FText& NewText, ETextCommit::Type CommitType);

	TSharedPtr<IPropertyHandle> AssetClassHandle;
	TSharedPtr<IPropertyHandle> ActorClassHandle;
	TSharedPtr<IPropertyHandle> PropertyNameHandle;

	TArray<TSharedPtr<FName>> PropertyNameOptions;
	TSharedPtr<SComboButton> PropertyNameComboButton;
};
