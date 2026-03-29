// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

/** Details panel for UDragToSpawnHandler. Adds Register/Unregister buttons. */
class FDragToSpawnHandlerCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	UClass* HandlerClass = nullptr;
	IDetailLayoutBuilder* CachedDetailBuilder = nullptr;

	bool IsRegistered() const;
	void Register();
	void Unregister();

	FReply OnRegisterClicked();
	FReply OnUnregisterClicked();
	EVisibility GetRegisterVisibility() const;
	EVisibility GetUnregisterVisibility() const;

	void OpenPluginSettings() const;
};
