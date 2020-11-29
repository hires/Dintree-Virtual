/*
 * Dintree Plugin Top
 *
 * Written by: Andrew Kilpatrick
 * Copyright 2020: Andrew Kilpatrick
 *
 * This file is part of Dintree-Virtual.
 *
 * Dintree-Virtual is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Dintree-Virtual is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Dintree-Virtual.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
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
extern Model* modelV105_Quad_CV_Proc;
extern Model* modelV107_Dual_Slew;
extern Model* modelV201_Tri_Comparator;
extern Model* modelV218_SH_Clock_Noise;

// defaults for modules that users can store
struct ModuleDefaults {
    int darkTheme;
};

// load module defaults for Dintree modules
void loadDefaults(struct ModuleDefaults *def);

// save module defaults for Dintree modules
void saveDefaults(struct ModuleDefaults *def);

#include "components/DintreeComponents.hpp"
