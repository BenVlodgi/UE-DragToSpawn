// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FDragToSpawnModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
