// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#include "DragToSpawn.h"
#include "DragToSpawnHandler.h"
#include "DragToSpawnSettings.h"
#include "DragToSpawnPreferences.h"
#include "Customizations/DragToSpawnHandlerCustomization.h"
#include "Customizations/DragToSpawnSettingsCustomization.h"
#include "Customizations/DragToSpawnPreferencesCustomization.h"
#include "Customizations/DragToSpawnMappingCustomization.h"

#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"

static const FName PropertyEditorModuleName("PropertyEditor");

void FDragToSpawnModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(PropertyEditorModuleName);
	PropertyModule.RegisterCustomClassLayout(UDragToSpawnHandler::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FDragToSpawnHandlerCustomization::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UDragToSpawnSettings::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FDragToSpawnSettingsCustomization::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UDragToSpawnPreferences::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FDragToSpawnPreferencesCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("DragToSpawnMapping", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDragToSpawnMappingCustomization::MakeInstance));
	PropertyModule.NotifyCustomizationModuleChanged();
}

void FDragToSpawnModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded(PropertyEditorModuleName))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditorModuleName);
		PropertyModule.UnregisterCustomClassLayout(UDragToSpawnHandler::StaticClass()->GetFName());
		PropertyModule.UnregisterCustomClassLayout(UDragToSpawnSettings::StaticClass()->GetFName());
		PropertyModule.UnregisterCustomClassLayout(UDragToSpawnPreferences::StaticClass()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout("DragToSpawnMapping");
	}
}

IMPLEMENT_MODULE(FDragToSpawnModule, DragToSpawn)
