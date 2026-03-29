// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#include "DragToSpawnPreferencesCustomization.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "ISettingsModule.h"
#include "Widgets/Input/SHyperlink.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "DragToSpawnPreferencesCustomization"

TSharedRef<IDetailCustomization> FDragToSpawnPreferencesCustomization::MakeInstance()
{
	return MakeShareable(new FDragToSpawnPreferencesCustomization());
}

void FDragToSpawnPreferencesCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("General", FText::GetEmpty(), ECategoryPriority::Important);

	Category.AddCustomRow(LOCTEXT("SettingsLinkRow", "Settings Link"))
		.WholeRowContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("SettingsPrefix", "Go to "))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SHyperlink)
				.Text(LOCTEXT("OpenSettings", "Project Settings"))
				.ToolTipText(LOCTEXT("OpenSettingsTooltip", "Project Settings | Plugins | Drag to Spawn"))
				.OnNavigate_Lambda([]()
				{
					FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer("Project", "Plugins", "DragToSpawn");
				})
			]
		];
}

#undef LOCTEXT_NAMESPACE
