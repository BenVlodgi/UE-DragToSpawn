// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EngineVersionComparison.h"

// UE_VERSION_NEWER_THAN_OR_EQUAL was added in UE 5.6. Define it for older engines.
#ifndef UE_VERSION_NEWER_THAN_OR_EQUAL
// Version comparison macro that is defined to true if the UE version is newer or equal than MajorVer.MinorVer.PatchVer 
// and false otherwise
// (a typical use is for backward compatible code)
#define UE_VERSION_NEWER_THAN_OR_EQUAL(MajorVersion, MinorVersion, PatchVersion) \
	!UE_VERSION_OLDER_THAN(MajorVersion, MinorVersion, PatchVersion)
#endif

// TObjectPtr was introduced in UE 5.0. On UE4, alias to raw pointer.
// Note: this only works for non-UPROPERTY members. UE4 UHT hardcodes its
// list of known template types and rejects TObjectPtr in reflected properties.
#if ENGINE_MAJOR_VERSION < 5
template <typename T>
using TObjectPtr = T*;
#endif
