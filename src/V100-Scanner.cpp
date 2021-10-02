/*
 * Dintree V100 Scanner
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

#define V100_NUM_INPUTS 8

// Dintree V100 Scanner module
struct V100_Scanner : Module {
    enum ParamIds {
        POT_RANGE,
        RAND_SW,
        CV_SW,
        NUM_PARAMS
    };
    enum InputIds {
        IN1,  // inputs must be sequential
        IN2,
        IN3,
        IN4,
        IN5,
        IN6,
        IN7,
        IN8,
        CTRL_IN,
        NUM_INPUTS
    };
    enum OutputIds {
        OUTA,
        OUTB,
        NUM_OUTPUTS
    };
    enum LightIds {
        IN1_LED,  // input LEDs must be sequential
        IN2_LED,
        IN3_LED,
        IN4_LED,
        IN5_LED,
        IN6_LED,
        IN7_LED,
        IN8_LED,
        RAND_LED,
        CV_LED,
        CTRL_LED,
        OUTA_LED,
        OUTB_LED,
        NUM_LIGHTS
    };
    enum {
        MODE_CV,
        MODE_CLOCK
    };
    #define CLOCK_THRESH_HI 1.01
    #define CLOCK_THRESH_LO 0.99
    #define RT_TASK_RATE 100.0
    dsp::ClockDivider task_timer;
    int chan_a, chan_b, old_chan;
    int mode, random;
    int clk_state;

    // constructor
    V100_Scanner() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(POT_RANGE, 0.f, 1.f, 0.f, "POT_RANGE");

        // reset stuff
        clk_state = 0;
        chan_a = 0;
        chan_b = 0;
        old_chan = 0;
        onReset();
        onSampleRateChange();
        setParams();
    }

    // process a sample
    void process(const ProcessArgs& args) override {
        float out;
        // state
        if(task_timer.process()) {
            setParams();
        }
        out = inputs[IN1 + chan_a].getVoltage();
        outputs[OUTA].setVoltage(out);
        lights[OUTA_LED].setBrightness(clamp(out * 0.5, 0.0f, 1.0f));
        out = inputs[IN1 + chan_b].getVoltage();
        outputs[OUTB].setVoltage(out);
        lights[OUTB_LED].setBrightness(clamp(out * 0.5, 0.0f, 1.0f));
    }

    // samplerate changed
    void onSampleRateChange(void) override {
        task_timer.setDivision((int)(APP->engine->getSampleRate() / RT_TASK_RATE));
    }

    // module initialize
    void onReset(void) override {
        random::init();
        params[POT_RANGE].setValue(1.0);
    }

    // set params based on input
    void setParams(void) {
        int i;
        float range;

        // get random switch
        if(params[RAND_SW].getValue() > 0.5) {
            lights[RAND_LED].setBrightness(1.0);
            random = 1;
        }
        else {
            lights[RAND_LED].setBrightness(0.0);
            random = 0;
        }

        // get CV switch
        if(params[CV_SW].getValue() > 0.5) {
            lights[CV_LED].setBrightness(1.0);
            mode = MODE_CV;
        }
        else {
            lights[CV_LED].setBrightness(0.0);
            mode = MODE_CLOCK;
        }

        // range control
        range = params[POT_RANGE].getValue() * 7.999;

        // process CV input
        if(mode == MODE_CV) {
            chan_a = clamp((int)(inputs[CTRL_IN].getVoltage() * 0.1 * range), 0, 7);
            lights[CTRL_LED].setBrightness(clamp(inputs[CTRL_IN].getVoltage() * 0.1, 0.0f, 1.0f));
            if(random && old_chan != chan_a) {
                chan_a = random::u32() % ((int)range + 1);
            }
        }
        // process clock input
        else {
            // pulse going low
            if(clk_state && inputs[CTRL_IN].getVoltage() < CLOCK_THRESH_LO) {
                clk_state = 0;
            }
            // pulse going high
            else if(!clk_state && inputs[CTRL_IN].getVoltage() > CLOCK_THRESH_HI) {
                clk_state = 1;
                if(random) {
                    chan_a = random::u32() % ((int)range + 1);
                }
                else {
                    chan_a ++;
                    if(chan_a > (int)range) {
                        chan_a = 0;
                    }
                }
            }
            lights[CTRL_LED].setBrightness((float)clk_state);
        }

        // select channel LED
        if(old_chan != chan_a) {
            chan_b = old_chan;
            old_chan = chan_a;
        }
        for(i = 0; i < V100_NUM_INPUTS; i ++) {
            if(i == chan_a) {
                lights[IN1_LED + i].setBrightness(1.0);
            }
            else {
                lights[IN1_LED + i].setBrightness(0.0);
            }
        }
    }
};

struct V100_ScannerWidget : ModuleWidget {
    ThemeChooser *theme_chooser;

    V100_ScannerWidget(V100_Scanner* module) {
        setModule(module);

        theme_chooser = new ThemeChooser(this, DINTREE_THEME_FILE,
            "Classic", asset::plugin(pluginInstance, "res/V100-Scanner.svg"));
        theme_chooser->addPanel("Dark", asset::plugin(pluginInstance, "res/V100-Scanner-b.svg"));
        theme_chooser->initPanel();

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(29.058, 26.163)), module, V100_Scanner::POT_RANGE));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.745, 19.792)), module, V100_Scanner::IN1));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.745, 32.492)), module, V100_Scanner::IN2));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.745, 45.192)), module, V100_Scanner::IN3));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.759, 57.913)), module, V100_Scanner::IN4));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.745, 70.592)), module, V100_Scanner::IN5));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.745, 83.292)), module, V100_Scanner::IN6));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(29.058, 83.313)), module, V100_Scanner::CTRL_IN));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.745, 95.992)), module, V100_Scanner::IN7));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.745, 108.692)), module, V100_Scanner::IN8));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(29.079, 95.992)), module, V100_Scanner::OUTA));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(29.104, 108.666)), module, V100_Scanner::OUTB));

        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.649, 19.792)), module, V100_Scanner::IN1_LED));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.649, 32.492)), module, V100_Scanner::IN2_LED));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(37.969, 45.192)), module, V100_Scanner::RAND_LED));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.649, 45.213)), module, V100_Scanner::IN3_LED));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.649, 57.892)), module, V100_Scanner::IN4_LED));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(37.969, 64.242)), module, V100_Scanner::CV_LED));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.649, 70.592)), module, V100_Scanner::IN5_LED));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.649, 83.292)), module, V100_Scanner::IN6_LED));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(37.969, 83.292)), module, V100_Scanner::CTRL_LED));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.649, 95.992)), module, V100_Scanner::IN7_LED));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(37.969, 95.992)), module, V100_Scanner::OUTA_LED));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.649, 108.692)), module, V100_Scanner::IN8_LED));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(37.969, 108.692)), module, V100_Scanner::OUTB_LED));

        addParam(createParamCentered<KilpatrickToggle2P>(mm2px(Vec(29.079, 46.192)), module, V100_Scanner::RAND_SW));
        addParam(createParamCentered<KilpatrickToggle2P>(mm2px(Vec(29.079, 65.242)), module, V100_Scanner::CV_SW));
    }

    void appendContextMenu(Menu *menu) override {
        V100_Scanner *module = dynamic_cast<V100_Scanner*>(this->module);
        assert(module);

        // theme chooser
        theme_chooser->populateThemeChooserMenuItems(menu);
    }

    void step() override {
        V100_Scanner *module = dynamic_cast<V100_Scanner*>(this->module);
        if(module) {
            // check theme
            theme_chooser->step();
        }
        Widget::step();
    }
};

Model* modelV100_Scanner = createModel<V100_Scanner, V100_ScannerWidget>("V100-Scanner");
