/*
 * Dintree V100 Scanner
 *
 * Written by: Andrew Kilpatrick
 * Copyright 2020: Andrew Kilpatrick
 *
 * This file is part of Dintree-VCV.
 *
 * Dintree-VCV is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Dintree-VCV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Dintree-VCV.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This code is is licensed under GPL with the additional requirement
 * that commercial hardware versions of this module, code or concept
 * not be made without express written permission from Andrew Kilpatrick.
 *
 */
#include "componentlibrary.hpp"

using namespace rack;

// SPDT switch
struct DintreeToggleSPDT : SvgSwitch {
    DintreeToggleSPDT() {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/toggle-down.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/toggle-up.svg")));
    }
};

// black knob with red
struct DintreeKnobBlackRed : app::SvgKnob {
    DintreeKnobBlackRed() {
        minAngle = -0.8334 * M_PI;
        maxAngle = 0.8334 * M_PI;
        shadow->opacity = 0;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/knob-black_red.svg")));
    }
};
