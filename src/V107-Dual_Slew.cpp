/*
 * Dintree V107 Dual Slew
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
#include "dsp_utils.h"
#include "utils/KAComponents.h"
#include "utils/MenuHelper.h"

struct V107_Dual_Slew : Module {
	enum ParamIds {
		POT_SLEW1,
		POT_SLEW2,
		NUM_PARAMS
	};
	enum InputIds {
		IN1,
		IN2,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1,
		OUT2,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

    #define RT_TASK_RATE 100.0

    dsp::ClockDivider task_timer;
    float hist1;
    float hist2;
    float slew1_a0;
    float slew2_a0;
    float AUDIO_FS;

	V107_Dual_Slew() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(POT_SLEW1, 0.f, 1.f, 0.f, "SLEW 1");
		configParam(POT_SLEW2, 0.f, 1.f, 0.f, "SLEW 2");
        configInput(IN1, "IN 1");
        configInput(IN2, "IN 2");
        configOutput(OUT1, "OUT 1");
        configOutput(OUT2, "OUT 2");
        // reset stuff
        hist1 = 0.0;
        hist2 = 0.0;
        onReset();
        onSampleRateChange();
        setParams();
	}

    // process a sample
	void process(const ProcessArgs& args) override {
        float tempf;
        // state
        if(task_timer.process()) {
            setParams();
        }
        DSP_UTILS_F1LP(inputs[IN1].getVoltage(), tempf, slew1_a0, hist1);
        outputs[OUT1].setVoltage(tempf);
        DSP_UTILS_F1LP(inputs[IN2].getVoltage(), tempf, slew2_a0, hist2);
        outputs[OUT2].setVoltage(tempf);
	}

    // samplerate changed
    void onSampleRateChange(void) override {
        AUDIO_FS = APP->engine->getSampleRate();
        task_timer.setDivision((int)(AUDIO_FS / RT_TASK_RATE));
    }

    // set params based on input
    void setParams(void) {
        float tempf;
        tempf = 1.0 - params[POT_SLEW1].getValue() + 0.00001;
        tempf *= tempf;
        DSP_UTILS_F1SC(tempf * 10.0, slew1_a0);
        tempf = 1.0 - params[POT_SLEW2].getValue() + 0.00001;
        tempf *= tempf;
        DSP_UTILS_F1SC(tempf * 10.0, slew2_a0);
    }
};

struct V107_Dual_SlewWidget : ModuleWidget {
	V107_Dual_SlewWidget(V107_Dual_Slew* module) {
		setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/V107-Dual_Slew.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(12.706, 24.275)), module, V107_Dual_Slew::POT_SLEW1));
		addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(12.706, 47.241)), module, V107_Dual_Slew::POT_SLEW2));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.706, 64.872)), module, V107_Dual_Slew::IN1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.706, 80.324)), module, V107_Dual_Slew::IN2));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.684, 95.776)), module, V107_Dual_Slew::OUT1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.684, 111.228)), module, V107_Dual_Slew::OUT2));
	}
};

Model* modelV107_Dual_Slew = createModel<V107_Dual_Slew, V107_Dual_SlewWidget>("V107-Dual_Slew");
