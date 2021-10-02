/*
 * Dintree V102 Output Mixer
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
#include "utils/ThemeChooser.h"
#include "dsp_utils.h"

struct V102_Output_Mixer : Module {
    enum ParamIds {
        POT_LEVEL1,
        POT_PAN1,
        POT_LEVEL2,
        POT_PAN2,
        POT_LEVEL3,
        POT_PAN3,
        POT_LEVEL4,
        POT_PAN4,
        POT_MASTER,
        NUM_PARAMS
    };
    enum InputIds {
        IN1,
        IN2,
        IN3,
        IN4,
        SUB_INL,
        SUB_INR,
        NUM_INPUTS
    };
    enum OutputIds {
        OUTL,
        OUTR,
        PRE_OUTL,
        PRE_OUTR,
        NUM_OUTPUTS
    };
    enum LightIds {
        LED_METERL_P6,
        LED_METERR_P6,
        LED_METERL_0,
        LED_METERR_0,
        LED_METERL_M6,
        LED_METERR_M6,
        LED_METERL_M12,
        LED_METERR_M12,
        LED_METERL_M18,
        LED_METERR_M18,
        NUM_LIGHTS
    };

    // settings
    #define RT_TASK_RATE 1000.0  // Hz
    #define METER_SMOOTHING 0.9999

    // state
    dsp::ClockDivider task_timer;
    float master;
    float level1_l;
    float level1_r;
    float level2_l;
    float level2_r;
    float level3_l;
    float level3_r;
    float level4_l;
    float level4_r;
    float meter_outl;
    float meter_outr;
    // DC block hist
    float in_hist[4];
    float in_hist2[4];
    float sub_hist[2];
    float sub_hist2[2];

    V102_Output_Mixer() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(POT_LEVEL1, 0.f, 1.f, 0.f, "LEVEL 1");
        configParam(POT_PAN1, 0.f, 1.f, 0.5f, "PAN 1");
        configParam(POT_LEVEL2, 0.f, 1.f, 0.f, "LEVEL 2");
        configParam(POT_PAN2, 0.f, 1.f, 0.5f, "PAN 2");
        configParam(POT_LEVEL3, 0.f, 1.f, 0.f, "LEVEL 3");
        configParam(POT_PAN3, 0.f, 1.f, 0.5f, "PAN 3");
        configParam(POT_LEVEL4, 0.f, 1.f, 0.f, "LEVEL 4");
        configParam(POT_PAN4, 0.f, 1.f, 0.5f, "PAN 4");
        configParam(POT_MASTER, 0.f, 1.f, 0.5f, "POT_MASTER");

        // reset stuff
        onReset();
        onSampleRateChange();
    }

    // process a sample
    void process(const ProcessArgs& args) override {
        float outl, outr, tempf1, tempf2, tempf3, tempf4;

        // state
        if(task_timer.process()) {
            setParams();
        }

        // HPF
        DSP_UTILS_DCB(inputs[IN1].getVoltage(), tempf1, in_hist[0], in_hist2[0]);
        DSP_UTILS_DCB(inputs[IN2].getVoltage(), tempf2, in_hist[1], in_hist2[1]);
        DSP_UTILS_DCB(inputs[IN3].getVoltage(), tempf3, in_hist[2], in_hist2[2]);
        DSP_UTILS_DCB(inputs[IN4].getVoltage(), tempf4, in_hist[3], in_hist2[3]);

        // clamp inputs
        tempf1 = DSP_UTILS_CLAMP_RANGE(tempf1, -10.0f, 10.0f);
        tempf2 = DSP_UTILS_CLAMP_RANGE(tempf2, -10.0f, 10.0f);
        tempf3 = DSP_UTILS_CLAMP_RANGE(tempf3, -10.0f, 10.0f);
        tempf4 = DSP_UTILS_CLAMP_RANGE(tempf4, -10.0f, 10.0f);

        // channel mixing
        outl = tempf1 * level1_l;
        outl += tempf2 * level2_l;
        outl += tempf3 * level3_l;
        outl += tempf4 * level4_l;

        outr = tempf1 * level1_r;
        outr += tempf2 * level2_r;
        outr += tempf3 * level3_r;
        outr += tempf4 * level4_r;

        // pre out
        outputs[PRE_OUTL].setVoltage(outl);
        outputs[PRE_OUTR].setVoltage(outr);

        // sub in
        DSP_UTILS_DCB(inputs[SUB_INL].getVoltage(), tempf1, sub_hist[0], sub_hist2[0]);
        DSP_UTILS_DCB(inputs[SUB_INR].getVoltage(), tempf2, sub_hist[1], sub_hist2[1]);
        tempf1 = DSP_UTILS_CLAMP_RANGE(tempf1, -10.0f, 10.0f);
        tempf2 = DSP_UTILS_CLAMP_RANGE(tempf2, -10.0f, 10.0f);

        outl += tempf1;
        outr += tempf2;

        outl *= master;
        outr *= master;

        // output
        outputs[OUTL].setVoltage(outl);
        outputs[OUTR].setVoltage(outr);

        // meters
        DSP_UTILS_LMM(DSP_UTILS_ABS(outl),
            meter_outl, METER_SMOOTHING);
        DSP_UTILS_LMM(DSP_UTILS_ABS(outr),
            meter_outr, METER_SMOOTHING);
    }

    // samplerate changed
    void onSampleRateChange(void) override {
        task_timer.setDivision((int)(APP->engine->getSampleRate() / RT_TASK_RATE));
    }

    // module initialize
    void onReset(void) override {
        // set initial pot values
        params[POT_LEVEL1].setValue(0.0);
        params[POT_PAN1].setValue(0.5);
        params[POT_LEVEL2].setValue(0.0);
        params[POT_PAN2].setValue(0.5);
        params[POT_LEVEL3].setValue(0.0);
        params[POT_PAN3].setValue(0.5);
        params[POT_LEVEL4].setValue(0.0);
        params[POT_PAN4].setValue(0.5);
        params[POT_MASTER].setValue(0.5);

        lights[LED_METERL_P6].setBrightness(0.0);
        lights[LED_METERR_P6].setBrightness(0.0);
        lights[LED_METERL_0].setBrightness(0.0);
        lights[LED_METERR_0].setBrightness(0.0);
        lights[LED_METERL_M6].setBrightness(0.0);
        lights[LED_METERR_M6].setBrightness(0.0);
        lights[LED_METERL_M12].setBrightness(0.0);
        lights[LED_METERR_M12].setBrightness(0.0);
        lights[LED_METERL_M18].setBrightness(0.0);
        lights[LED_METERR_M18].setBrightness(0.0);

        meter_outl = 0.0;
        meter_outr = 0.0;
        for(int i = 0; i < 4; i ++) {
            in_hist[i] = 0.0;
            in_hist2[i] = 0.0;
        }
        for(int i = 0; i < 2; i ++) {
            sub_hist[i] = 0.0;
            sub_hist2[i] = 0.0;
        }
        setParams();
    }

    // set params based on input
    void setParams(void) {
        float level, pan;
        int db;

        // pots
        level = params[POT_LEVEL1].getValue();
        level *= level;
        pan = params[POT_PAN1].getValue();
        level1_l = level * (1.0 - pan);
        level1_r = level * pan;

        level = params[POT_LEVEL2].getValue();
        level *= level;
        pan = params[POT_PAN2].getValue();
        level2_l = level * (1.0 - pan);
        level2_r = level * pan;

        level = params[POT_LEVEL3].getValue();
        level *= level;
        pan = params[POT_PAN3].getValue();
        level3_l = level * (1.0 - pan);
        level3_r = level * pan;

        level = params[POT_LEVEL4].getValue();
        level *= level;
        pan = params[POT_PAN4].getValue();
        level4_l = level * (1.0 - pan);
        level4_r = level * pan;

        master = params[POT_MASTER].getValue();
        master *= master;
        master *= 4.0;

        // meters
        db = DSP_UTILS_F2DB(DSP_UTILS_CLAMP_POS(meter_outl * 0.1)) + 7;
        if(db > 6) {
            lights[LED_METERL_P6].setBrightness(1.0);
            lights[LED_METERL_0].setBrightness(1.0);
            lights[LED_METERL_M6].setBrightness(1.0);
            lights[LED_METERL_M12].setBrightness(1.0);
            lights[LED_METERL_M18].setBrightness(1.0);
        }
        else if(db > 0) {
            lights[LED_METERL_P6].setBrightness(0.0);
            lights[LED_METERL_0].setBrightness(1.0);
            lights[LED_METERL_M6].setBrightness(1.0);
            lights[LED_METERL_M12].setBrightness(1.0);
            lights[LED_METERL_M18].setBrightness(1.0);
        }
        else if(db > -6) {
            lights[LED_METERL_P6].setBrightness(0.0);
            lights[LED_METERL_0].setBrightness(0.0);
            lights[LED_METERL_M6].setBrightness(1.0);
            lights[LED_METERL_M12].setBrightness(1.0);
            lights[LED_METERL_M18].setBrightness(1.0);
        }
        else if(db > -12) {
            lights[LED_METERL_P6].setBrightness(0.0);
            lights[LED_METERL_0].setBrightness(0.0);
            lights[LED_METERL_M6].setBrightness(0.0);
            lights[LED_METERL_M12].setBrightness(1.0);
            lights[LED_METERL_M18].setBrightness(1.0);
        }
        else if(db > -18) {
            lights[LED_METERL_P6].setBrightness(0.0);
            lights[LED_METERL_0].setBrightness(0.0);
            lights[LED_METERL_M6].setBrightness(0.0);
            lights[LED_METERL_M12].setBrightness(0.0);
            lights[LED_METERL_M18].setBrightness(1.0);
        }
        else {
            lights[LED_METERL_P6].setBrightness(0.0);
            lights[LED_METERL_0].setBrightness(0.0);
            lights[LED_METERL_M6].setBrightness(0.0);
            lights[LED_METERL_M12].setBrightness(0.0);
            lights[LED_METERL_M18].setBrightness(0.0);
        }

        db = DSP_UTILS_F2DB(DSP_UTILS_CLAMP_POS(meter_outr * 0.1)) + 7;
        if(db > 6) {
            lights[LED_METERR_P6].setBrightness(1.0);
            lights[LED_METERR_0].setBrightness(1.0);
            lights[LED_METERR_M6].setBrightness(1.0);
            lights[LED_METERR_M12].setBrightness(1.0);
            lights[LED_METERR_M18].setBrightness(1.0);
        }
        else if(db > 0) {
            lights[LED_METERR_P6].setBrightness(0.0);
            lights[LED_METERR_0].setBrightness(1.0);
            lights[LED_METERR_M6].setBrightness(1.0);
            lights[LED_METERR_M12].setBrightness(1.0);
            lights[LED_METERR_M18].setBrightness(1.0);
        }
        else if(db > -6) {
            lights[LED_METERR_P6].setBrightness(0.0);
            lights[LED_METERR_0].setBrightness(0.0);
            lights[LED_METERR_M6].setBrightness(1.0);
            lights[LED_METERR_M12].setBrightness(1.0);
            lights[LED_METERR_M18].setBrightness(1.0);
        }
        else if(db > -12) {
            lights[LED_METERR_P6].setBrightness(0.0);
            lights[LED_METERR_0].setBrightness(0.0);
            lights[LED_METERR_M6].setBrightness(0.0);
            lights[LED_METERR_M12].setBrightness(1.0);
            lights[LED_METERR_M18].setBrightness(1.0);
        }
        else if(db > -18) {
            lights[LED_METERR_P6].setBrightness(0.0);
            lights[LED_METERR_0].setBrightness(0.0);
            lights[LED_METERR_M6].setBrightness(0.0);
            lights[LED_METERR_M12].setBrightness(0.0);
            lights[LED_METERR_M18].setBrightness(1.0);
        }
        else {
            lights[LED_METERR_P6].setBrightness(0.0);
            lights[LED_METERR_0].setBrightness(0.0);
            lights[LED_METERR_M6].setBrightness(0.0);
            lights[LED_METERR_M12].setBrightness(0.0);
            lights[LED_METERR_M18].setBrightness(0.0);
        }
    }
};

struct V102_Output_MixerWidget : ModuleWidget {
    ThemeChooser *theme_chooser;

    V102_Output_MixerWidget(V102_Output_Mixer* module) {
        setModule(module);

        theme_chooser = new ThemeChooser(this, DINTREE_THEME_FILE,
            "Classic", asset::plugin(pluginInstance, "res/V102-Output_Mixer.svg"));
        theme_chooser->addPanel("Dark", asset::plugin(pluginInstance, "res/V102-Output_Mixer-b.svg"));
        theme_chooser->initPanel();

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(27.579, 24.255)), module, V102_Output_Mixer::POT_LEVEL1));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(50.44, 24.255)), module, V102_Output_Mixer::POT_PAN1));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(27.579, 50.904)), module, V102_Output_Mixer::POT_LEVEL2));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(50.461, 50.904)), module, V102_Output_Mixer::POT_PAN2));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(27.58, 77.616)), module, V102_Output_Mixer::POT_LEVEL3));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(50.461, 77.574)), module, V102_Output_Mixer::POT_PAN3));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(75.84, 77.616)), module, V102_Output_Mixer::POT_MASTER));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(27.579, 104.197)), module, V102_Output_Mixer::POT_LEVEL4));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(50.461, 104.197)), module, V102_Output_Mixer::POT_PAN4));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.361, 44.236)), module, V102_Output_Mixer::IN1));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.361, 57.571)), module, V102_Output_Mixer::IN2));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.361, 70.906)), module, V102_Output_Mixer::IN3));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.361, 84.262)), module, V102_Output_Mixer::IN4));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.361, 97.575)), module, V102_Output_Mixer::SUB_INL));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.361, 110.932)), module, V102_Output_Mixer::SUB_INR));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.193, 97.575)), module, V102_Output_Mixer::OUTL));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(82.55, 97.575)), module, V102_Output_Mixer::OUTR));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.193, 110.932)), module, V102_Output_Mixer::PRE_OUTL));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(82.55, 110.932)), module, V102_Output_Mixer::PRE_OUTR));

        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(72.051, 24.255)), module, V102_Output_Mixer::LED_METERL_P6));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(79.692, 24.255)), module, V102_Output_Mixer::LED_METERR_P6));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(72.051, 31.832)), module, V102_Output_Mixer::LED_METERL_0));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(79.692, 31.832)), module, V102_Output_Mixer::LED_METERR_0));
        addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(72.051, 39.453)), module, V102_Output_Mixer::LED_METERL_M6));
        addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(79.692, 39.453)), module, V102_Output_Mixer::LED_METERR_M6));
        addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(72.051, 47.075)), module, V102_Output_Mixer::LED_METERL_M12));
        addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(79.692, 47.073)), module, V102_Output_Mixer::LED_METERR_M12));
        addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(72.051, 54.692)), module, V102_Output_Mixer::LED_METERL_M18));
        addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(79.692, 54.692)), module, V102_Output_Mixer::LED_METERR_M18));
    }

    void appendContextMenu(Menu *menu) override {
        V102_Output_Mixer *module = dynamic_cast<V102_Output_Mixer*>(this->module);
        assert(module);

        // theme chooser
        theme_chooser->populateThemeChooserMenuItems(menu);
    }

    void step() override {
        V102_Output_Mixer *module = dynamic_cast<V102_Output_Mixer*>(this->module);
        if(module) {
            // check theme
            theme_chooser->step();
        }
        Widget::step();
    }
};

Model* modelV102_Output_Mixer = createModel<V102_Output_Mixer, V102_Output_MixerWidget>("V102-Output_Mixer");
