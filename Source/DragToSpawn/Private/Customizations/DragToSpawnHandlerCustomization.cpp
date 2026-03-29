// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#include "DragToSpawnHandlerCustomization.h"
#include "DragToSpawnHandler.h"
#include "DragToSpawnSettings.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "ISettingsModule.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SHyperlink.h"
#include "Widgets/Text/STextBlock.h"
#include "DragToSpawnCompatibility.h"

#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 0, 0)
#include "Styling/AppStyle.h"
#else
#include "EditorStyleSet.h"
using FAppStyle = FEditorStyle;
#endif

#define LOCTEXT_NAMESPACE "DragToSpawnHandlerCustomization"

TSharedRef<IDetailCustomization> FDragToSpawnHandlerCustomization::MakeInstance()
{
	return MakeShareable(new FDragToSpawnHandlerCustomization());
}

void FDragToSpawnHandlerCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	CachedDetailBuilder = &DetailBuilder;

	TArray<TWeakObjectPtr<UObject>> SelectedObjects;
	DetailBuilder.GetObjectsBeingCustomized(SelectedObjects);
	for (const TWeakObjectPtr<UObject>& Object : SelectedObjects)
	{
		if (Object.IsValid() && Object->GetClass()->IsChildOf(UDragToSpawnHandler::StaticClass()))
		{
			HandlerClass = Object->GetClass();
			break;
		}
	}

	if (!HandlerClass)
	{
		return;
	}

	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("Drag to Spawn", FText::GetEmpty(), ECategoryPriority::Important);

	Category.AddCustomRow(LOCTEXT("RegistrationRow", "Registration"))
		.WholeRowContent()
		.VAlign(VAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(16.f, 8.f)
			[
				SNew(SButton)
				.ButtonStyle(FAppStyle::Get(), "FlatButton.Success")
				.HAlign(HAlign_Center)
				.OnClicked(this, &FDragToSpawnHandlerCustomization::OnRegisterClicked)
				.Visibility(this, &FDragToSpawnHandlerCustomization::GetRegisterVisibility)
				.ContentPadding(2)
				[
					SNew(STextBlock)
					.TextStyle(FAppStyle::Get(), "ContentBrowser.TopBar.Font")
					.Text(LOCTEXT("RegisterButton", "Register"))
				]
			]
			+ SHorizontalBox::Slot()
			.Padding(16.f, 8.f)
			[
				SNew(SButton)
				.ButtonStyle(FAppStyle::Get(), "FlatButton.Danger")
				.HAlign(HAlign_Center)
				.OnClicked(this, &FDragToSpawnHandlerCustomization::OnUnregisterClicked)
				.Visibility(this, &FDragToSpawnHandlerCustomization::GetUnregisterVisibility)
				.ContentPadding(2)
				[
					SNew(STextBlock)
					.TextStyle(FAppStyle::Get(), "ContentBrowser.TopBar.Font")
					.Text(LOCTEXT("UnregisterButton", "Unregister"))
				]
			]
		];

	Category.AddCustomRow(LOCTEXT("SettingsLinkRow", "Settings Link"))
		.WholeRowContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(16.f, 0.f, 0.f, 0.f)
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
				.OnNavigate(this, &FDragToSpawnHandlerCustomization::OpenPluginSettings)
			]
		];
}

bool FDragToSpawnHandlerCustomization::IsRegistered() const
{
	if (!HandlerClass)
	{
		return false;
	}

	const UDragToSpawnSettings* Settings = GetDefault<UDragToSpawnSettings>();
	if (!Settings)
	{
		return false;
	}

	for (const TSoftClassPtr<UDragToSpawnHandler>& Entry : Settings->Handlers)
	{
		if (Entry.Get() == HandlerClass || Entry.ToSoftObjectPath() == FSoftObjectPath(HandlerClass))
		{
			return true;
		}
	}

	return false;
}

void FDragToSpawnHandlerCustomization::Register()
{
	if (!HandlerClass || IsRegistered())
	{
		return;
	}

	UDragToSpawnSettings* Settings = GetMutableDefault<UDragToSpawnSettings>();
	Settings->Handlers.Add(TSoftClassPtr<UDragToSpawnHandler>(HandlerClass));
	Settings->SaveConfig();
}

void FDragToSpawnHandlerCustomization::Unregister()
{
	if (!HandlerClass)
	{
		return;
	}

	UDragToSpawnSettings* Settings = GetMutableDefault<UDragToSpawnSettings>();
	const FSoftObjectPath HandlerPath(HandlerClass);
	Settings->Handlers.RemoveAll([&HandlerPath](const TSoftClassPtr<UDragToSpawnHandler>& Entry)
	{
		return Entry.ToSoftObjectPath() == HandlerPath;
	});
	Settings->SaveConfig();
}

FReply FDragToSpawnHandlerCustomization::OnRegisterClicked()
{
	Register();
	if (CachedDetailBuilder)
	{
		CachedDetailBuilder->ForceRefreshDetails();
	}
	return FReply::Handled();
}

FReply FDragToSpawnHandlerCustomization::OnUnregisterClicked()
{
	Unregister();
	if (CachedDetailBuilder)
	{
		CachedDetailBuilder->ForceRefreshDetails();
	}
	return FReply::Handled();
}

EVisibility FDragToSpawnHandlerCustomization::GetRegisterVisibility() const
{
	return IsRegistered() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility FDragToSpawnHandlerCustomization::GetUnregisterVisibility() const
{
	return IsRegistered() ? EVisibility::Visible : EVisibility::Collapsed;
}

void FDragToSpawnHandlerCustomization::OpenPluginSettings() const
{
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer("Project", "Plugins", "DragToSpawn");
}

#undef LOCTEXT_NAMESPACE
