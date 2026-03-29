// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#include "DragToSpawnSettingsCustomization.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "ISettingsModule.h"
#include "Widgets/Input/SHyperlink.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "DragToSpawnSettingsCustomization"

TSharedRef<IDetailCustomization> FDragToSpawnSettingsCustomization::MakeInstance()
{
	return MakeShareable(new FDragToSpawnSettingsCustomization());
}

void FDragToSpawnSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("Drag to Spawn", FText::GetEmpty(), ECategoryPriority::Important);

	Category.AddCustomRow(LOCTEXT("PreferencesLinkRow", "Preferences Link"))
		.WholeRowContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("PreferencesPrefix", "Go to "))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SHyperlink)
				.Text(LOCTEXT("OpenPreferences", "Developer Preferences"))
				.ToolTipText(LOCTEXT("OpenPreferencesTooltip", "Editor Preferences | Plugins | Drag to Spawn"))
				.OnNavigate_Lambda([]()
				{
					FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer("Editor", "Plugins", "DragToSpawn");
				})
			]
		];

}

#undef LOCTEXT_NAMESPACE
