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
#include "plugin.hpp"

Plugin* pluginInstance;

// init!
void init(Plugin* p) {
	pluginInstance = p;
	p->addModel(modelV100_Scanner);
    p->addModel(modelV101_Dual_Envelope);
    p->addModel(modelV102_Output_Mixer);
    p->addModel(modelV103_Reverb_Delay);
    p->addModel(modelV104_Four_Vs);
    p->addModel(modelV105_Quad_CV_Proc);
    p->addModel(modelV107_Dual_Slew);
    p->addModel(modelV201_Tri_Comparator);
    p->addModel(modelV202_Levelmeter);
    p->addModel(modelV203_Test_Osc);
    p->addModel(modelV218_SH_Clock_Noise);
}
