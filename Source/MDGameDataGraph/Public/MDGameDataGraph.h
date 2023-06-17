#pragma once

#include "Modules/ModuleManager.h"

class FMDGameDataGraphModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
