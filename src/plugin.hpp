#pragma once
#include <rack.hpp>

using namespace rack;

// plugin definitions
extern Plugin* pluginInstance;

// model definitions
extern Model* modelV100_Scanner;
extern Model* modelV101_Dual_Envelope;
extern Model* modelV102_Output_Mixer;
extern Model* modelV103_Reverb_Delay;
extern Model* modelV104_Four_Vs;

// defaults for modules that users can store
struct ModuleDefaults {
    int darkTheme;
};

// load module defaults for Dintree modules
void loadDefaults(struct ModuleDefaults *def);

// save module defaults for Dintree modules
void saveDefaults(struct ModuleDefaults *def);

#include "components/DintreeComponents.hpp"
