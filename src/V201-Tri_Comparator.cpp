/*
 * Dintree V201 Tri Comparator
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

// Dintree V201 Tri Comparator module
struct V201_Tri_Comparator : Module {
	enum ParamIds {
		OUT_RANGE_SW,
		NUM_PARAMS
	};
	enum InputIds {
		IN_A,
		IN_B,
		IN_C,
		NUM_INPUTS
	};
	enum OutputIds {
		F1_OUT,
		F2_OUT,
		F3_OUT,
		F4_OUT,
		F5_OUT,
		F6_OUT,
		F7_OUT,
		F8_OUT,
		F9_OUT,
		F10_OUT,
		F11_OUT,
		F12_OUT,
		F13_OUT,
		F14_OUT,
		F15_OUT,
		F16_OUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		F1_LED,
		F2_LED,
		F3_LED,
		F4_LED,
		F5_LED,
		F6_LED,
		F7_LED,
		F8_LED,
		F9_LED,
		F10_LED,
		F11_LED,
		F12_LED,
		F13_LED,
		F14_LED,
		F15_LED,
		F16_LED,
        ENUMS(IN_A_LED, 3),
        ENUMS(IN_B_LED, 3),
        ENUMS(IN_C_LED, 3),
		NUM_LIGHTS
	};

    #define BICOLOR_LED_SCALE 0.2f
    #define EQUAL_NEARNESS 0.01
    #define RT_TASK_RATE 100.0
    dsp::ClockDivider task_timer;
    float AUDIO_FS;
    float output_level;

	V201_Tri_Comparator() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(OUT_RANGE_SW, 0.f, 1.f, 0.f, "OUT RANGE");
        configInput(IN_A, "IN A");
        configInput(IN_B, "IN B");
        configInput(IN_C, "IN C");
        configOutput(F1_OUT, "OUT A=B=C");
        configOutput(F2_OUT, "OUT A=B<C");
        configOutput(F3_OUT, "OUT A=B>C");
        configOutput(F4_OUT, "OUT A=B!=C");
        configOutput(F5_OUT, "OUT A<B=C");
        configOutput(F6_OUT, "OUT A<B<C");
        configOutput(F7_OUT, "OUT A<B>C");
        configOutput(F8_OUT, "OUT A<B!=C");
        configOutput(F9_OUT, "OUT A>B=C");
        configOutput(F10_OUT, "OUT A>B<C");
        configOutput(F11_OUT, "OUT A>B>C");
        configOutput(F12_OUT, "OUT A>B!=C");
        configOutput(F13_OUT, "OUT A!=B=C");
        configOutput(F14_OUT, "OUT A!=B<C");
        configOutput(F15_OUT, "OUT A!=B>C");
        configOutput(F16_OUT, "OUT A!=B!=C");
        // reset stuff
        onReset();
        onSampleRateChange();
        setParams();
	}

    // process a sample
	void process(const ProcessArgs& args) override {
        bool aeb, bec;
        float ina, inb, inc;

        // state
        if(task_timer.process()) {
            setParams();
        }

        ina = inputs[IN_A].getVoltage();
        inb = inputs[IN_B].getVoltage();
        inc = inputs[IN_C].getVoltage();
        aeb = isNear(ina, inb, EQUAL_NEARNESS);
        bec = isNear(inb, inc, EQUAL_NEARNESS);

        // F1 - A=B=C
        if(aeb && bec) {
            outputs[F1_OUT].setVoltage(output_level);
            lights[F1_LED].setBrightness(1.0f);
        }
        else {
            outputs[F1_OUT].setVoltage(0.0f);
            lights[F1_LED].setBrightness(0.0f);
        }

        // F2 - A=B<C
        if(aeb && inb < inc) {
            outputs[F2_OUT].setVoltage(output_level);
            lights[F2_LED].setBrightness(1.0f);
        }
        else {
            outputs[F2_OUT].setVoltage(0.0f);
            lights[F2_LED].setBrightness(0.0f);
        }

        // F3 - A=B>C
        if(aeb && inb > inc) {
            outputs[F3_OUT].setVoltage(output_level);
            lights[F3_LED].setBrightness(1.0f);
        }
        else {
            outputs[F3_OUT].setVoltage(0.0f);
            lights[F3_LED].setBrightness(0.0f);
        }

        // F4 - A=B!=C
        if(aeb && !bec) {
            outputs[F4_OUT].setVoltage(output_level);
            lights[F4_LED].setBrightness(1.0f);
        }
        else {
            outputs[F4_OUT].setVoltage(0.0f);
            lights[F4_LED].setBrightness(0.0f);
        }

        // F5 - A<B=C
        if(ina < inb && bec) {
            outputs[F5_OUT].setVoltage(output_level);
            lights[F5_LED].setBrightness(1.0f);
        }
        else {
            outputs[F5_OUT].setVoltage(0.0f);
            lights[F5_LED].setBrightness(0.0f);
        }

        // F6 - A<B<C
        if(ina < inb && inb < inc) {
            outputs[F6_OUT].setVoltage(output_level);
            lights[F6_LED].setBrightness(1.0f);
        }
        else {
            outputs[F6_OUT].setVoltage(0.0f);
            lights[F6_LED].setBrightness(0.0f);
        }

        // F7 - A<B>C
        if(ina < inb && inb > inc) {
            outputs[F7_OUT].setVoltage(output_level);
            lights[F7_LED].setBrightness(1.0f);
        }
        else {
            outputs[F7_OUT].setVoltage(0.0f);
            lights[F7_LED].setBrightness(0.0f);
        }

        // F8 - A<B!=C
        if(ina < inb && !bec) {
            outputs[F8_OUT].setVoltage(output_level);
            lights[F8_LED].setBrightness(1.0f);
        }
        else {
            outputs[F8_OUT].setVoltage(0.0f);
            lights[F8_LED].setBrightness(0.0f);
        }

        // F9 - A>B=C
        if(ina > inb && bec) {
            outputs[F9_OUT].setVoltage(output_level);
            lights[F9_LED].setBrightness(1.0f);
        }
        else {
            outputs[F9_OUT].setVoltage(0.0f);
            lights[F9_LED].setBrightness(0.0f);
        }

        // F10 - A>B<C
        if(ina > inb && inb < inc) {
            outputs[F10_OUT].setVoltage(output_level);
            lights[F10_LED].setBrightness(1.0f);
        }
        else {
            outputs[F10_OUT].setVoltage(0.0f);
            lights[F10_LED].setBrightness(0.0f);
        }

        // F11 - A>B>C
        if(ina > inb && inb > inc) {
            outputs[F11_OUT].setVoltage(output_level);
            lights[F11_LED].setBrightness(1.0f);
        }
        else {
            outputs[F11_OUT].setVoltage(0.0f);
            lights[F11_LED].setBrightness(0.0f);
        }

        // F12 - A>B!=C
        if(ina > inb && !bec) {
            outputs[F12_OUT].setVoltage(output_level);
            lights[F12_LED].setBrightness(1.0f);
        }
        else {
            outputs[F12_OUT].setVoltage(0.0f);
            lights[F12_LED].setBrightness(0.0f);
        }

        // F13 - A!=B=C
        if(!aeb && bec) {
            outputs[F13_OUT].setVoltage(output_level);
            lights[F13_LED].setBrightness(1.0f);
        }
        else {
            outputs[F13_OUT].setVoltage(0.0f);
            lights[F13_LED].setBrightness(0.0f);
        }

        // F14 - A!=B<C
        if(!aeb && inb < inc) {
            outputs[F14_OUT].setVoltage(output_level);
            lights[F14_LED].setBrightness(1.0f);
        }
        else {
            outputs[F14_OUT].setVoltage(0.0f);
            lights[F14_LED].setBrightness(0.0f);
        }

        // F15 - A!=B>C
        if(!aeb && inb > inc) {
            outputs[F15_OUT].setVoltage(output_level);
            lights[F15_LED].setBrightness(1.0f);
        }
        else {
            outputs[F15_OUT].setVoltage(0.0f);
            lights[F15_LED].setBrightness(0.0f);
        }

        // F16 - A!=B!=C
        if(!aeb && !bec) {
            outputs[F16_OUT].setVoltage(output_level);
            lights[F16_LED].setBrightness(1.0f);
        }
        else {
            outputs[F16_OUT].setVoltage(0.0f);
            lights[F16_LED].setBrightness(0.0f);
        }

        // input A
        ina *= BICOLOR_LED_SCALE;
        inb *= BICOLOR_LED_SCALE;
        inc *= BICOLOR_LED_SCALE;
        lights[IN_A_LED + 0].setBrightness(clamp(-ina, 0.0f, 1.0f));  // red
        lights[IN_A_LED + 1].setBrightness(clamp(ina, 0.0f, 1.0f));  // green
        lights[IN_B_LED + 0].setBrightness(clamp(-inb, 0.0f, 1.0f));  // red
        lights[IN_B_LED + 1].setBrightness(clamp(inb, 0.0f, 1.0f));  // green
        lights[IN_C_LED + 0].setBrightness(clamp(-inc, 0.0f, 1.0f));  // red
        lights[IN_C_LED + 1].setBrightness(clamp(inc, 0.0f, 1.0f));  // green
	}

    // samplerate changed
    void onSampleRateChange(void) override {
        AUDIO_FS = APP->engine->getSampleRate();
        task_timer.setDivision((int)(AUDIO_FS / RT_TASK_RATE));
    }

    // module initialize
    void onReset(void) override {
        // disable blue LEDs
        lights[IN_A_LED + 2].setBrightness(0.0f);
        lights[IN_B_LED + 2].setBrightness(0.0f);
        lights[IN_C_LED + 2].setBrightness(0.0f);
    }

    // set params based on input
    void setParams(void) {
        if(params[OUT_RANGE_SW].getValue() > 0.5) {
            output_level = 10.0;
        }
        else {
            output_level = 5.0;
        }
    }
};

struct V201_Tri_ComparatorWidget : ModuleWidget {
	V201_Tri_ComparatorWidget(V201_Tri_Comparator* module) {
		setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/V201-Tri_Comparator.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<KilpatrickToggle2P>(mm2px(Vec(51.48, 112.5)), module, V201_Tri_Comparator::OUT_RANGE_SW));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.48, 112.5)), module, V201_Tri_Comparator::IN_A));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(23.48, 112.5)), module, V201_Tri_Comparator::IN_B));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.48, 112.5)), module, V201_Tri_Comparator::IN_C));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.48, 24.5)), module, V201_Tri_Comparator::F1_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.48, 24.5)), module, V201_Tri_Comparator::F2_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(37.48, 24.5)), module, V201_Tri_Comparator::F3_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(51.48, 24.5)), module, V201_Tri_Comparator::F4_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.48, 46.5)), module, V201_Tri_Comparator::F5_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.48, 46.5)), module, V201_Tri_Comparator::F6_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(37.48, 46.5)), module, V201_Tri_Comparator::F7_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(51.48, 46.5)), module, V201_Tri_Comparator::F8_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.48, 68.5)), module, V201_Tri_Comparator::F9_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.48, 68.5)), module, V201_Tri_Comparator::F10_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(37.48, 68.5)), module, V201_Tri_Comparator::F11_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(51.48, 68.5)), module, V201_Tri_Comparator::F12_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.48, 90.5)), module, V201_Tri_Comparator::F13_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.48, 90.5)), module, V201_Tri_Comparator::F14_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(37.48, 90.5)), module, V201_Tri_Comparator::F15_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(51.48, 90.5)), module, V201_Tri_Comparator::F16_OUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(9.48, 12.5)), module, V201_Tri_Comparator::F1_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(23.48, 12.5)), module, V201_Tri_Comparator::F2_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(37.48, 12.5)), module, V201_Tri_Comparator::F3_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(51.48, 12.5)), module, V201_Tri_Comparator::F4_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(9.48, 34.5)), module, V201_Tri_Comparator::F5_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(23.48, 34.5)), module, V201_Tri_Comparator::F6_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(37.48, 34.5)), module, V201_Tri_Comparator::F7_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(51.48, 34.5)), module, V201_Tri_Comparator::F8_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(9.48, 56.5)), module, V201_Tri_Comparator::F9_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(23.48, 56.5)), module, V201_Tri_Comparator::F10_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(37.48, 56.5)), module, V201_Tri_Comparator::F11_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(51.48, 56.5)), module, V201_Tri_Comparator::F12_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(9.48, 78.5)), module, V201_Tri_Comparator::F13_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(23.48, 78.5)), module, V201_Tri_Comparator::F14_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(37.48, 78.5)), module, V201_Tri_Comparator::F15_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(51.48, 78.5)), module, V201_Tri_Comparator::F16_LED));
        addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(9.48, 101.0)), module, V201_Tri_Comparator::IN_A_LED));
        addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(23.48, 101.0)), module, V201_Tri_Comparator::IN_B_LED));
        addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(37.48, 101.0)), module, V201_Tri_Comparator::IN_C_LED));
	}
};

Model* modelV201_Tri_Comparator = createModel<V201_Tri_Comparator, V201_Tri_ComparatorWidget>("V201-Tri_Comparator");
