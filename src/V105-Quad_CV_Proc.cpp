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

struct V105_Quad_CV_Proc : Module {
    enum ParamIds {
        POT_GAIN1,
        POT_GAIN2,
        POT_GAIN3,
        POT_GAIN4,
        NUM_PARAMS
    };
    enum InputIds {
        IN1A,
        IN1B,
        IN2A,
        IN2B,
        IN3A,
        IN3B,
        IN4A,
        IN4B,
        NUM_INPUTS
    };
    enum OutputIds {
        OUT1P,
        OUT1M,
        OUT2P,
        OUT2M,
        OUT3P,
        OUT3M,
        OUT4P,
        OUT4M,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    // settings
    #define RT_TASK_RATE 1000.0  // Hz

    // state
    dsp::ClockDivider task_timer;
    float gain1;
    float gain2;
    float gain3;
    float gain4;

    V105_Quad_CV_Proc() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(POT_GAIN1, 0.f, 1.f, 0.f, "");
        configParam(POT_GAIN2, 0.f, 1.f, 0.f, "");
        configParam(POT_GAIN3, 0.f, 1.f, 0.f, "");
        configParam(POT_GAIN4, 0.f, 1.f, 0.f, "");

        // reset stuff
        onReset();
        onSampleRateChange();
    }

    // process a sample
    void process(const ProcessArgs& args) override {
        float tempf;

        // state
        if(task_timer.process()) {
            setParams();
        }
        tempf = (inputs[IN1A].getVoltage() + inputs[IN1B].getVoltage()) * gain1;
        outputs[OUT1P].setVoltage(tempf);
        outputs[OUT1M].setVoltage(-tempf);
        tempf = (inputs[IN2A].getVoltage() + inputs[IN2B].getVoltage()) * gain2;
        outputs[OUT2P].setVoltage(tempf);
        outputs[OUT2M].setVoltage(-tempf);
        tempf = (inputs[IN3A].getVoltage() + inputs[IN3B].getVoltage()) * gain3;
        outputs[OUT3P].setVoltage(tempf);
        outputs[OUT3M].setVoltage(-tempf);
        tempf = (inputs[IN4A].getVoltage() + inputs[IN4B].getVoltage()) * gain4;
        outputs[OUT4P].setVoltage(tempf);
        outputs[OUT4M].setVoltage(-tempf);
    }

    // samplerate changed
    void onSampleRateChange(void) override {
        task_timer.setDivision((int)(APP->engine->getSampleRate() / RT_TASK_RATE));
    }

    // module initialize
    void onReset(void) override {
        params[POT_GAIN1].setValue(0.5);
        params[POT_GAIN2].setValue(0.5);
        params[POT_GAIN3].setValue(0.5);
        params[POT_GAIN4].setValue(0.5);
    }

    // set params based on input
    void setParams(void) {
        gain1 = params[POT_GAIN1].getValue() * 2.0;
        gain2 = params[POT_GAIN2].getValue() * 2.0;
        gain3 = params[POT_GAIN3].getValue() * 2.0;
        gain4 = params[POT_GAIN4].getValue() * 2.0;
    }
};

struct V105_Quad_CV_ProcWidget : ModuleWidget {
    V105_Quad_CV_ProcWidget(V105_Quad_CV_Proc* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/V105-Quad_CV_Proc.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(21.231, 24.254)), module, V105_Quad_CV_Proc::POT_GAIN1));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(21.252, 50.924)), module, V105_Quad_CV_Proc::POT_GAIN2));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(21.231, 77.594)), module, V105_Quad_CV_Proc::POT_GAIN3));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(21.252, 104.285)), module, V105_Quad_CV_Proc::POT_GAIN4));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.011, 17.544)), module, V105_Quad_CV_Proc::IN1A));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.032, 30.879)), module, V105_Quad_CV_Proc::IN1B));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.011, 44.214)), module, V105_Quad_CV_Proc::IN2A));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.011, 57.549)), module, V105_Quad_CV_Proc::IN2B));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.032, 70.884)), module, V105_Quad_CV_Proc::IN3A));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.032, 84.24)), module, V105_Quad_CV_Proc::IN3B));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.032, 97.575)), module, V105_Quad_CV_Proc::IN4A));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.011, 110.91)), module, V105_Quad_CV_Proc::IN4B));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(54.272, 17.544)), module, V105_Quad_CV_Proc::OUT1P));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(54.272, 30.879)), module, V105_Quad_CV_Proc::OUT1M));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(54.272, 44.214)), module, V105_Quad_CV_Proc::OUT2P));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(54.272, 57.549)), module, V105_Quad_CV_Proc::OUT2M));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(54.272, 70.905)), module, V105_Quad_CV_Proc::OUT3P));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(54.272, 84.219)), module, V105_Quad_CV_Proc::OUT3M));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(54.272, 97.575)), module, V105_Quad_CV_Proc::OUT4P));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(54.272, 110.91)), module, V105_Quad_CV_Proc::OUT4M));
    }
};

Model* modelV105_Quad_CV_Proc = createModel<V105_Quad_CV_Proc, V105_Quad_CV_ProcWidget>("V105-Quad_CV_Proc");
