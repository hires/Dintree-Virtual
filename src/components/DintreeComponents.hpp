/*
 * Dintree GUI Components
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
#include "componentlibrary.hpp"

using namespace rack;

// SPDT switch
struct DintreeToggle2P : SvgSwitch {
    DintreeToggle2P() {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/toggle-down.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/toggle-up.svg")));
    }
};

// SP3T switch
struct DintreeToggle3P : SvgSwitch {

    DintreeToggle3P() {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/toggle-down.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/toggle-middle.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/toggle-up.svg")));
    }

	// handle the manually entered values
	void onChange(const event::Change &e) override {
		SvgSwitch::onChange(e);
    }

    void randomize() override {
        SvgSwitch::randomize();
        if(paramQuantity->getValue() > 1.33f) {
            paramQuantity->setValue(2.0f);
        }
        else if(paramQuantity->getValue() > 0.67f) {
            paramQuantity->setValue(1.0f);
        }
        else {
            paramQuantity->setValue(0.0f);
        }
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
