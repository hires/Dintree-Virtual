#pragma once
#include <rack.hpp>

using namespace rack;

// plugin definitions
extern Plugin* pluginInstance;

// model definitions
extern Model* modelV100_Scanner;

// defaults for modules that users can store
struct ModuleDefaults {
    int darkTheme;
};

// load module defaults for Dintree modules
void loadDefaults(struct ModuleDefaults *def);

// save module defaults for Dintree modules
void saveDefaults(struct ModuleDefaults *def);

#include "components/DintreeComponents.hpp"
