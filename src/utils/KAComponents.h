/*
 * Kilpatrick Audio GUI Elements
 *
 * Written by: Andrew Kilpatrick
 * Copyright 2020: Andrew Kilpatrick
 *
 */
#ifndef KACOMPONENTS_H
#define KACOMPONENTS_H

#include "componentlibrary.hpp"
#include "../plugin.hpp"

using namespace rack;

// SPDT switch
struct KilpatrickToggle2P : SvgSwitch {
    KilpatrickToggle2P() {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/toggle-down.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/toggle-up.svg")));
    }
};

// SP3T switch
struct KilpatrickToggle3P : SvgSwitch {
    KilpatrickToggle3P() {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/toggle-down.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/toggle-middle.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/toggle-up.svg")));
    }

    // randomize the position of the switch
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

// knob with grey grey
struct KilpatrickKnobGreyGrey : app::SvgKnob {
    KilpatrickKnobGreyGrey() {
        minAngle = -0.8334 * M_PI;
        maxAngle = 0.8334 * M_PI;
        shadow->opacity = 0;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/knob-grey_grey.svg")));
    }
};

// black knob with red
struct KilpatrickKnobBlackRed : KilpatrickKnobGreyGrey {
    KilpatrickKnobBlackRed() {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/knob-black_red.svg")));
    }
};

#endif
