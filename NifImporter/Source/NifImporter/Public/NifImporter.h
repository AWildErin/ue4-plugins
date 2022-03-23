// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FNifImporterModule : public IModuleInterface
{
public:

	// Begin IModelInterface Interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// End IModelInterface Interface
};