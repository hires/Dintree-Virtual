/*
 * Dintree V101 Dual Envelope
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
#include "utils/MenuHelper.h"
#include "utils/ThemeChooser.h"
#include "dsp_utils.h"

struct V101_Dual_Envelope : Module {
    enum ParamIds {
        POT_ATTACK1,
        POT_ATTACK2,
        POT_DECAY1,
        POT_DECAY2,
        POT_SUSTAIN1,
        POT_SUSTAIN2,
        POT_RELEASE1,
        POT_RELEASE2,
        MODE1_SW,
        MODE2_SW,
        NUM_PARAMS
    };
    enum InputIds {
        GATE1_IN,
        GATE2_IN,
        NUM_INPUTS
    };
    enum OutputIds {
        ENV1_OUT,
        ENV2_OUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        ENV1_LED,
        ENV2_LED,
        NUM_LIGHTS
    };

    uint16_t TIME_TABLE[128] = {
        0xFFFF    ,
        0xEADC    ,
        0xD777    ,
        0xC5AD    ,
        0xB55B    ,
        0xA661    ,
        0x98A4    ,
        0x8C0A    ,
        0x807A    ,
        0x75DE    ,
        0x6C23    ,
        0x6335    ,
        0x5B04    ,
        0x5555    ,
        0x4CE0    ,
        0x4542    ,
        0x3E65    ,
        0x3836    ,
        0x32A4    ,
        0x2D9F    ,
        0x291A    ,
        0x2507    ,
        0x215C    ,
        0x1E0D    ,
        0x1B13    ,
        0x199A    ,
        0x177C    ,
        0x158C    ,
        0x13C5    ,
        0x1223    ,
        0x10A3    ,
        0x0F44    ,
        0x0E01    ,
        0x0CD9    ,
        0x0BC9    ,
        0x0AD0    ,
        0x09EC    ,
        0x091A    ,
        0x0889    ,
        0x0817    ,
        0x07AB    ,
        0x0744    ,
        0x06E3    ,
        0x0687    ,
        0x0630    ,
        0x05DE    ,
        0x058F    ,
        0x0545    ,
        0x04FF    ,
        0x04BC    ,
        0x047D    ,
        0x0444    ,
        0x041A    ,
        0x03F2    ,
        0x03CB    ,
        0x03A6    ,
        0x0382    ,
        0x035F    ,
        0x033E    ,
        0x031E    ,
        0x02FF    ,
        0x02E2    ,
        0x02C6    ,
        0x02AA    ,
        0x028F    ,
        0x0259    ,
        0x0228    ,
        0x01FA    ,
        0x01D0    ,
        0x01AA    ,
        0x0187    ,
        0x0166    ,
        0x0149    ,
        0x012E    ,
        0x0115    ,
        0x00FE    ,
        0x00E9    ,
        0x00DA    ,
        0x00CF    ,
        0x00C4    ,
        0x00BA    ,
        0x00B0    ,
        0x00A7    ,
        0x009E    ,
        0x0096    ,
        0x008E    ,
        0x0087    ,
        0x0080    ,
        0x0079    ,
        0x0073    ,
        0x006D    ,
        0x0069    ,
        0x0064    ,
        0x0060    ,
        0x005C    ,
        0x0058    ,
        0x0054    ,
        0x0050    ,
        0x004D    ,
        0x0049    ,
        0x0046    ,
        0x0043    ,
        0x0042    ,
        0x003C    ,
        0x0036    ,
        0x0031    ,
        0x002D    ,
        0x0029    ,
        0x0025    ,
        0x0022    ,
        0x001F    ,
        0x001C    ,
        0x0019    ,
        0x0017    ,
        0x0016    ,
        0x0014    ,
        0x0012    ,
        0x0010    ,
        0x000F    ,
        0x000E    ,
        0x000C    ,
        0x000B    ,
        0x000A    ,
        0x0009    ,
        0x0008    ,
        0x0008    ,
        0x0007    ,
        0x0007
    };

    // settings
    #define RT_TASK_RATE 1000.0  // Hz

    // maximum level for ramping up
    #define MAX_LEVEL 65535
    // envelope modes
    #define MODE_ADSR 0
    #define MODE_AR 1
    #define MODE_LFO 2
    uint8_t env_mode[2];  // the ADSR, AR, LFO mode
    // envelope states
    #define ENV_IDLE 0
    #define ENV_ATTACK 1
    #define ENV_DECAY 2
    #define ENV_SUSTAIN 3
    #define ENV_RELEASE 4
    uint8_t env_state[2];  // the current ADSR state
    // other stuff
    uint8_t chan_sel = 0;
    uint8_t gate_state[2];  // the state of the gate
    int32_t env_level[2];  // the current DAC level for the output
    int32_t attack[2];  // step size for attack phase
    int32_t decay[2];  // step size for decay phase
    int32_t sustain[2];  // sustain level
    int32_t release[2];  // step size for release phase
    uint8_t lfo_trig[2];  // 1 = auto trig, 0 = reset by gate
    float env1_out, env2_out;
    float dac0_z1, dac1_z1;

    // state
    dsp::ClockDivider task_timer;
    int timer_div;

    // constructor
    V101_Dual_Envelope() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(POT_ATTACK1, 0.f, 1.f, 0.f, "ATTACK 1");
        configParam(POT_ATTACK2, 0.f, 1.f, 0.f, "ATTACK 2");
        configParam(POT_DECAY1, 0.f, 1.f, 0.f, "DECAY 1");
        configParam(POT_DECAY2, 0.f, 1.f, 0.f, "DECAY 2");
        configParam(POT_SUSTAIN1, 0.f, 1.f, 0.f, "SUSTAIN 1");
        configParam(POT_SUSTAIN2, 0.f, 1.f, 0.f, "SUSTAIN 2");
        configParam(POT_RELEASE1, 0.f, 1.f, 0.f, "RELEASE 1");
        configParam(POT_RELEASE2, 0.f, 1.f, 0.f, "RELEASE 2");
        configParam(MODE1_SW, 0.0f, 2.0f, 0.0f, "MODE 1");
        configParam(MODE2_SW, 0.0f, 2.0f, 0.0f, "MODE 2");

        // reset stuff
        onReset();
        onSampleRateChange();
    }

    // process a sample
    void process(const ProcessArgs& args) override {
        float tempf;

        // state
        if(task_timer.process()) {
            if((timer_div & 0x08) == 0) {
                setParams();
            }
            // run envelopes
            envelope_control(0, inputs[GATE1_IN].getVoltage() > 1.0 ? 0 : 1);
            envelope_control(1, inputs[GATE2_IN].getVoltage() > 1.0 ? 0 : 1);
            // convert to float now so we can smooth it a bit since we don't have analog hardware
            env1_out = (float)env_level[0] * 0.000152588;
            env2_out = (float)env_level[1] * 0.000152588;
            lights[ENV1_LED].setBrightness(env1_out * 0.1);
            lights[ENV2_LED].setBrightness(env2_out * 0.1);
            timer_div ++;
        }

        DSP_UTILS_F1LP(env1_out, tempf, 0.1, dac0_z1);
        outputs[ENV1_OUT].setVoltage(tempf);
        DSP_UTILS_F1LP(env2_out, tempf, 0.1, dac1_z1);
        outputs[ENV2_OUT].setVoltage(tempf);
    }

    // samplerate changed
    void onSampleRateChange(void) override {
        task_timer.setDivision((int)(APP->engine->getSampleRate() / RT_TASK_RATE));
    }

    // module initialize
    void onReset(void) override {
        // set initial pot values
        params[POT_ATTACK1].setValue(0.1);
        params[POT_ATTACK2].setValue(0.1);
        params[POT_DECAY1].setValue(0.1);
        params[POT_DECAY2].setValue(0.1);
        params[POT_SUSTAIN1].setValue(0.8);
        params[POT_SUSTAIN2].setValue(0.8);
        params[POT_RELEASE1].setValue(0.2);
        params[POT_RELEASE2].setValue(0.2);

        // clear outputs
        env1_out = 0.0f;
        env2_out = 0.0f;

        // reset stuff
        timer_div = 0;
        env_mode[0] = MODE_ADSR;
        env_mode[1] = MODE_ADSR;
        env_state[0] = ENV_IDLE;
        env_state[1] = ENV_IDLE;
        gate_state[0] = 0;
        gate_state[1] = 0;
        env_level[0] = 0;
        env_level[1] = 0;
        lfo_trig[0] = 1;
        lfo_trig[1] = 1;
        env1_out = 0.0f;
        env2_out = 0.0f;
        dac0_z1 = 0.0f;
        dac1_z1 = 0.0f;
        setParams();
    }

    // set params based on input
    void setParams(void) {
        // process pot input to control variables
        switch(chan_sel & 0x07) {
            case 0:
                attack[0] = time_lookup(params[POT_ATTACK1].getValue());
                break;
            case 1:
                decay[0] = time_lookup(params[POT_DECAY1].getValue());
                break;
            case 2:
                sustain[0] = (int)(params[POT_SUSTAIN1].getValue() * 255.0) << 8;
                break;
            case 3:
                release[0] = time_lookup(params[POT_RELEASE1].getValue());
                break;
            case 4:
                attack[1] = time_lookup(params[POT_ATTACK2].getValue());
                break;
            case 5:
                decay[1] = time_lookup(params[POT_DECAY2].getValue());
                break;
            case 6:
                sustain[1] = (int)(params[POT_SUSTAIN2].getValue() * 255.0) << 8;
                break;
            case 7:
                release[1] = time_lookup(params[POT_RELEASE2].getValue());
                break;
        }
        chan_sel ++;

        // handle mode switches
        if(params[MODE1_SW].getValue() > 1.5) {
            if(env_mode[0] != MODE_ADSR) {
                env_mode[0] = MODE_ADSR;
                reset_envelope(0);
            }
        }
        else if(params[MODE1_SW].getValue() > 0.5) {
            if(env_mode[0] != MODE_AR) {
                env_mode[0] = MODE_AR;
                reset_envelope(0);
            }
        }
        else {
            if(env_mode[0] != MODE_LFO) {
                env_mode[0] = MODE_LFO;
                lfo_trig[0] = 1;  // reset
                reset_envelope(0);
            }
        }

        if(params[MODE2_SW].getValue() > 1.5) {
            if(env_mode[1] != MODE_ADSR) {
                env_mode[1] = MODE_ADSR;
                reset_envelope(1);
            }
        }
        else if(params[MODE2_SW].getValue() > 0.5) {
            if(env_mode[1] != MODE_AR) {
                env_mode[1] = MODE_AR;
                reset_envelope(1);
            }
        }
        else {
            if(env_mode[1] != MODE_LFO) {
                env_mode[1] = MODE_LFO;
                lfo_trig[1] = 1;  // reset
                reset_envelope(1);
            }
        }
    }

    // reset an envelope
    void reset_envelope(unsigned char chan) {
        if(chan > 1) return;
        env_state[chan] = ENV_IDLE;
        env_level[chan] = 0;
    }

    // control an envelope
    // gate signal is inverted - 0 = on, 1 = off
    void envelope_control(unsigned char chan, unsigned char gate) {
        if(chan > 1) return;

        //
        // GATE CONTROL
        //
        if(env_mode[chan] == MODE_LFO) {
            // gate came on
            if(gate == 0 && gate_state[chan] == 0) {
                gate_state[chan] = 1;
                lfo_trig[chan] = 1;
            }
            // gate went off
            if(gate == 1 && gate_state[chan] == 1) {
                gate_state[chan] = 0;
                lfo_trig[chan] = 0;
            }
            // restart the envelope if it is stopped
            if(lfo_trig[chan] == 1 && env_state[chan] == ENV_IDLE) {
                env_state[chan] = ENV_ATTACK;
            }
        }
        else {
            // gate came on
            if(gate == 0 && gate_state[chan] == 0) {
                gate_state[chan] = 1;
                env_state[chan] = ENV_ATTACK;
            }
            // gate went off
            if(gate == 1 && gate_state[chan] == 1) {
                gate_state[chan] = 0;
                // only change to release phase for ADSR
                if(env_mode[chan] == MODE_ADSR) env_state[chan] = ENV_RELEASE;
            }
        }

        //
        // ENV STATE CONTROL
        //
        // attack
        if(env_state[chan] == ENV_ATTACK) {
            env_level[chan] += attack[chan];
            // is it time to move to the decay or release phase?
            if(env_level[chan] > MAX_LEVEL) {
                env_level[chan] = MAX_LEVEL;
                // ADSR mode
                if(env_mode[chan] == MODE_ADSR) {
                    env_state[chan] = ENV_DECAY;
                }
                // AR or LFO mode
                else {
                    env_state[chan] = ENV_RELEASE;
                }
            }
            return;
        }
        // decay
        if(env_state[chan] == ENV_DECAY) {
            env_level[chan] -= decay[chan];
            // is it time to move to the systain phase?
            if(env_level[chan] < sustain[chan]) {
                env_level[chan] = sustain[chan];
                env_state[chan] = ENV_SUSTAIN;
            }
            return;
        }
        // sustain
        if(env_state[chan] == ENV_SUSTAIN) {
            // make the sustain control interactive in realtime
            env_level[chan] = sustain[chan];
            return;
        }
        // release
        if(env_state[chan] == ENV_RELEASE) {
            env_level[chan] -= release[chan];
            // is it time to end the release phase?
            if(env_level[chan] < 0) {
                env_level[chan] = 0;
                env_state[chan] = ENV_IDLE;
            }
            return;
        }
    }

    // look up a time val and return the step size
    unsigned int time_lookup(float val) {
        return TIME_TABLE[(int)roundf(val * 127.0)];
    }
};

struct V101_Dual_EnvelopeWidget : ModuleWidget {
    ThemeChooser *theme_chooser;

    V101_Dual_EnvelopeWidget(V101_Dual_Envelope* module) {
        setModule(module);

        theme_chooser = new ThemeChooser(this, DINTREE_THEME_FILE,
            "Classic", asset::plugin(pluginInstance, "res/V101-Dual_Envelope.svg"));
        theme_chooser->addPanel("Dark", asset::plugin(pluginInstance, "res/V101-Dual_Envelope-b.svg"));
        theme_chooser->initPanel();

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(27.569, 18.519)), module, V101_Dual_Envelope::POT_ATTACK1));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(50.389, 18.519)), module, V101_Dual_Envelope::POT_ATTACK2));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(27.602, 41.353)), module, V101_Dual_Envelope::POT_DECAY1));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(50.389, 41.353)), module, V101_Dual_Envelope::POT_DECAY2));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(27.569, 64.259)), module, V101_Dual_Envelope::POT_SUSTAIN1));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(50.462, 64.259)), module, V101_Dual_Envelope::POT_SUSTAIN2));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(27.602, 87.119)), module, V101_Dual_Envelope::POT_RELEASE1));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(50.389, 87.119)), module, V101_Dual_Envelope::POT_RELEASE2));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.284, 56.66)), module, V101_Dual_Envelope::GATE1_IN));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.284, 94.056)), module, V101_Dual_Envelope::GATE2_IN));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.284, 43.337)), module, V101_Dual_Envelope::ENV1_OUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.284, 80.734)), module, V101_Dual_Envelope::ENV2_OUT));

        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.331, 31.159)), module, V101_Dual_Envelope::ENV1_LED));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.331, 68.538)), module, V101_Dual_Envelope::ENV2_LED));

        addParam(createParamCentered<KilpatrickToggle3P>(mm2px(Vec(27.569, 109.925)), module, V101_Dual_Envelope::MODE1_SW));
        addParam(createParamCentered<KilpatrickToggle3P>(mm2px(Vec(50.389, 109.925)), module, V101_Dual_Envelope::MODE2_SW));
    }

    void appendContextMenu(Menu *menu) override {
        V101_Dual_Envelope *module = dynamic_cast<V101_Dual_Envelope*>(this->module);
        assert(module);

        // theme chooser
        theme_chooser->populateThemeChooserMenuItems(menu);
    }

    void step() override {
        V101_Dual_Envelope *module = dynamic_cast<V101_Dual_Envelope*>(this->module);
        if(module) {
            // check theme
            theme_chooser->step();
        }
        Widget::step();
    }
};

Model* modelV101_Dual_Envelope = createModel<V101_Dual_Envelope, V101_Dual_EnvelopeWidget>("V101-Dual_Envelope");
