/*
 * Dintree V104 Four Vs
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
#include "utils/KAComponents.h"
#include "utils/MenuHelper.h"

struct V104_Four_Vs : Module {
    enum ParamIds {
        POT_VOLTAGE1,
        POT_VOLTAGE2,
        POT_VOLTAGE3,
        POT_VOLTAGE4,
        ON1_SW,
        ON2_SW,
        NUM_PARAMS
    };
    enum InputIds {
        NUM_INPUTS
    };
    enum OutputIds {
        OUT1,
        OUT2,
        OUT3,
        OUT4,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    // settings
    #define RT_TASK_RATE 1000.0  // Hz

    // state
    dsp::ClockDivider task_timer;
    float voltage1;
    float voltage2;
    float voltage3;
    float voltage4;

    V104_Four_Vs() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(POT_VOLTAGE1, -5.0f, 5.0f, 0.f, "VOLTAGE 1");
        configParam(POT_VOLTAGE2, -5.0f, 5.0f, 0.f, "VOLTAGE 2");
        configParam(POT_VOLTAGE3, -5.0f, 5.0f, 0.f, "VOLTAGE 3");
        configParam(POT_VOLTAGE4, -5.0f, 5.0f, 0.f, "VOLTAGE 4");

        // reset stuff
        onReset();
        onSampleRateChange();
    }

    // process a sample
    void process(const ProcessArgs& args) override {
        // state
        if(task_timer.process()) {
            setParams();
        }
        outputs[OUT1].setVoltage(voltage1);
        outputs[OUT2].setVoltage(voltage2);
        outputs[OUT3].setVoltage(voltage3);
        outputs[OUT4].setVoltage(voltage4);
    }

    // samplerate changed
    void onSampleRateChange(void) override {
        task_timer.setDivision((int)(APP->engine->getSampleRate() / RT_TASK_RATE));
    }

    // module initialize
    void onReset(void) override {
        params[POT_VOLTAGE1].setValue(0.0);
        params[POT_VOLTAGE2].setValue(0.0);
        params[POT_VOLTAGE3].setValue(0.0);
        params[POT_VOLTAGE4].setValue(0.0);
        params[ON1_SW].setValue(1.0);
        params[ON2_SW].setValue(1.0);
    }

    // set params based on input
    void setParams(void) {
        if(params[ON1_SW].getValue() > 0.5) {
            voltage1 = params[POT_VOLTAGE1].getValue();
        }
        else {
            voltage1 = 0.0;
        }
        if(params[ON2_SW].getValue() > 0.5) {
            voltage2 = params[POT_VOLTAGE2].getValue();
        }
        else {
            voltage2 = 0.0;
        }
        voltage3 = params[POT_VOLTAGE3].getValue();
        voltage4 = params[POT_VOLTAGE4].getValue();
    }
};

struct V104_Four_VsWidget : ModuleWidget {
    V104_Four_VsWidget(V104_Four_Vs* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/V104-Four_Vs.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(27.601, 24.233)), module, V104_Four_Vs::POT_VOLTAGE1));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(27.58, 50.882)), module, V104_Four_Vs::POT_VOLTAGE2));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(27.602, 77.594)), module, V104_Four_Vs::POT_VOLTAGE3));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(27.602, 104.222)), module, V104_Four_Vs::POT_VOLTAGE4));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.362, 70.884)), module, V104_Four_Vs::OUT1));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.362, 84.219)), module, V104_Four_Vs::OUT2));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.362, 97.661)), module, V104_Four_Vs::OUT3));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.362, 110.996)), module, V104_Four_Vs::OUT4));

        addParam(createParamCentered<KilpatrickToggle2P>(mm2px(Vec(12.345, 24.233)), module, V104_Four_Vs::ON1_SW));
        addParam(createParamCentered<KilpatrickToggle2P>(mm2px(Vec(12.345, 50.903)), module, V104_Four_Vs::ON2_SW));
    }
};

Model* modelV104_Four_Vs = createModel<V104_Four_Vs, V104_Four_VsWidget>("V104-Four_Vs");
