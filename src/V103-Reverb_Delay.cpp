/*
 * Dintree V103 Reverb Delay.
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

struct V103_Reverb_Delay : Module {
    enum ParamIds {
        POT_REV_MIX,
        POT_DEL_MIX,
        POT_DEL_TIME,
        DEL_SW,
        REV_SW,
        NUM_PARAMS
    };
    enum InputIds {
        INL,
        INR,
        NUM_INPUTS
    };
    enum OutputIds {
        OUTL,
        OUTR,
        NUM_OUTPUTS
    };
    enum LightIds {
        CLIP_LED,
        NUM_LIGHTS
    };
    enum {
        REV_COEFF_API1,
        REV_COEFF_API2,
        REV_COEFF_API3,
        REV_COEFF_API4,
        REV_COEFF_AP1,
        REV_COEFF_DEL1,
        REV_COEFF_AP2,
        REV_COEFF_DEL2,
        REV_COEFF_LPF_CUTOFF,
        REV_COEFF_HPF_CUTOFF,
        REV_COEFF_ECHO
    };

    #define RT_TASK_RATE 100.0
    #define METER_SMOOTHING 0.9999

    dsp::ClockDivider task_timer;
    // settings
    int AUDIO_FS;
    // delay line coeffs
    int api1_in;
    int api1;
    int api2_in;
    int api2;
    int api3_in;
    int api3;
    int api4_in;
    int api4;
    int ap1_in;
    int ap1;
    int del1_in;
    int del1;
    int ap2_in;
    int ap2;
    int del2_in;
    int del2;
    int echo_in;
    int echo;
    // filter coeffs
    float lfilt_a0;
    float hfilt_a0;
    // settings
    int rev;
    float filter;
    float size;
    float kap;
    float krt;
    float rev_mix;
    float del_mix;
    float del_time;
    float del_synco;
    float del_synco_t1;
    float del_synco_t2;
    // state
    float dmem[1024*1024];
    int dlen;  // delay memory length (must be a power of 2)
    int dp;  // delay memory pointer
    // working regs
    float lfilt_z1;
    float hfilt_z1;
    float peak;
    float feedback_samp;
    int del_len;
    int del_lp_z1;

    V103_Reverb_Delay() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(POT_REV_MIX, 0.f, 1.f, 0.f, "REVERB MIX");
        configParam(POT_DEL_MIX, 0.f, 1.f, 0.f, "DELAY MIX");
        configParam(POT_DEL_TIME, 0.f, 1.f, 0.f, "DELAY TIME");
        configParam(DEL_SW, 0.0f, 2.0f, 0.0f, "DELAY TYPE");
        configParam(REV_SW, 0.0f, 1.0f, 0.0f, "REVERB_TYPE");

        // reset stuff
        onReset();
        onSampleRateChange();
        setParams();
    }

    // process a sample
    void process(const ProcessArgs& args) override {
        float inlr, outl, outr, tempf;
        float klow, khigh, kpass;  // filter mixing coeffs
        float acc, temp1, apout, it1, lpout, hpout;

        // state
        if(task_timer.process()) {
            setParams();
        }

        // smooth time
        DSP_UTILS_F1LP(params[POT_DEL_TIME].getValue(), tempf, 0.999999999, del_time);

        // filter mixing coeffs
        khigh = DSP_UTILS_CLAMP_POS((filter - 0.5) * 2.0);
        klow = DSP_UTILS_CLAMP_POS((1.0 - (filter * 2.0)));
        kpass = 1.0 - DSP_UTILS_ABS((filter * 2.0) - 1.0);

        // process reverb
        // rotate delay mem
        DSP_UTILS_DROT(dp, dlen);

        inlr = inputs[INL].getVoltage() * 0.75;
        inlr += inputs[INR].getVoltage() * 0.75;

        // delay in
        DSP_UTILS_DWRITE(dmem, dp, dlen, echo_in, inlr + feedback_samp);

        // reverb
        DSP_UTILS_F1LP(inlr, lpout, lfilt_a0, lfilt_z1);
        DSP_UTILS_F1HP(inlr, hpout, hfilt_a0, hfilt_z1);
        acc = lpout * klow;
        acc += hpout * khigh;
        acc += inlr * kpass;

        DSP_UTILS_AP(dmem, dp, dlen, api1_in, api1, kap);
        DSP_UTILS_AP(dmem, dp, dlen, api2_in, api2, kap);
        DSP_UTILS_AP(dmem, dp, dlen, api3_in, api3, kap);
        DSP_UTILS_AP(dmem, dp, dlen, api4_in, api4, kap);
        apout = acc;

        DSP_UTILS_DREAD(dmem, dp, dlen, del2, temp1);
        acc += temp1;
        acc *= krt;
        DSP_UTILS_AP(dmem, dp, dlen, ap1_in, ap1, kap);
        DSP_UTILS_DWRITE(dmem, dp, dlen, del1_in, acc);
        outl = acc;

        acc = apout;
        DSP_UTILS_DREAD(dmem, dp, dlen, del1, temp1);
        acc += temp1;
        acc *= krt;
        DSP_UTILS_AP(dmem, dp, dlen, ap2_in, ap2, kap);
        DSP_UTILS_DWRITE(dmem, dp, dlen, del2_in, acc);
        outr = acc;

        outl *= rev_mix;
        outr *= rev_mix;

        DSP_UTILS_DREADF(dmem, dp, dlen, (float)echo_in + ((float)del_len * del_time), tempf);
        outl += tempf * del_mix;
        outr += tempf * del_mix;

        DSP_UTILS_DREADF(dmem, dp, dlen, (float)echo_in + ((float)del_len * del_time * del_synco_t1), tempf);
        outl += tempf * del_mix * del_synco;

        DSP_UTILS_DREADF(dmem, dp, dlen, (float)echo_in + ((float)del_len * del_time * del_synco_t2), tempf);
        outr += tempf * del_mix * del_synco;

        tempf *= 0.4;
        DSP_UTILS_F1LP(tempf, feedback_samp, 0.6, del_lp_z1);

        tempf = DSP_UTILS_ABS(outl);
        tempf = DSP_UTILS_MAX(DSP_UTILS_ABS(outr), tempf);
        DSP_UTILS_LMM(tempf, peak, METER_SMOOTHING);

        outputs[OUTL].setVoltage(outl);
        outputs[OUTR].setVoltage(outr);
    }

    // samplerate changed
    void onSampleRateChange(void) override {
        task_timer.setDivision((int)(APP->engine->getSampleRate() / RT_TASK_RATE));
        AUDIO_FS = (int)APP->engine->getSampleRate();
    }

    // module initialize
    void onReset(void) override {
        random::init();
        params[POT_REV_MIX].setValue(0.5);
        params[POT_DEL_MIX].setValue(0.5);
        params[POT_DEL_TIME].setValue(0.5);
        params[DEL_SW].setValue(2.0);
        params[REV_SW].setValue(1.0);
        rev = -1;
        dlen = 1024*1024;
        dp = 0;
        kap = 0.55;
        lfilt_z1 = 0.0;
        hfilt_z1 = 0.0;
        peak = 0.0;
        del_time = 0.0;
        feedback_samp = 0.0;
        del_len = 0;
        del_lp_z1 = 0.0;
    }

    // set params based on input
    void setParams(void) {
        float fscale = AUDIO_FS / (float)32768.0;  // scale for orig samplerate
        int new_rev;
        if(params[REV_SW].getValue() > 0.5) {
            new_rev = 1;
        }
        else {
            new_rev = 0;
        }

        if(params[DEL_SW].getValue() > 1.5) {
            del_synco = 0.0;
            del_synco_t1 = 0.666;
            del_synco_t2 = 0.333;
        }
        else if(params[DEL_SW].getValue() > 0.5) {
            del_synco = 0.6;
            del_synco_t1 = 0.666;
            del_synco_t2 = 0.333;
        }
        else {
            del_synco = 0.6;
            del_synco_t1 = 0.750;
            del_synco_t2 = 0.250;
        }

        if(rev != new_rev) {
            switch(new_rev) {
                case 1:
                    set_coeff(REV_COEFF_API1, (int)(553.0 * fscale));
                    set_coeff(REV_COEFF_API2, (int)(922.0 * fscale));
                    set_coeff(REV_COEFF_API3, (int)(122.0 * fscale));
                    set_coeff(REV_COEFF_API4, (int)(303.0 * fscale));
                    set_coeff(REV_COEFF_AP1, (int)(2062.0 * fscale));
                    set_coeff(REV_COEFF_DEL1, (int)(3375.0 * fscale));
                    set_coeff(REV_COEFF_AP2, (int)(2500.0 * fscale));
                    set_coeff(REV_COEFF_DEL2, (int)(2250.0 * fscale));
                    set_coeff(REV_COEFF_LPF_CUTOFF, 200.0);
                    set_coeff(REV_COEFF_HPF_CUTOFF, 4000.0);
                    del_len = (int)(AUDIO_FS * 0.5);
                    set_coeff(REV_COEFF_ECHO, del_len);
                    calc_coeffs();
                    break;
                case 0:
                default:
                    set_coeff(REV_COEFF_API1, (int)(522.0 * fscale));
                    set_coeff(REV_COEFF_API2, (int)(303.0 * fscale));
                    set_coeff(REV_COEFF_API3, (int)(653.0 * fscale));
                    set_coeff(REV_COEFF_API4, (int)(222.0 * fscale));
                    set_coeff(REV_COEFF_AP1, (int)(1050.0 * fscale));
                    set_coeff(REV_COEFF_DEL1, (int)(1775.0 * fscale));
                    set_coeff(REV_COEFF_AP2, (int)(962.0 * fscale));
                    set_coeff(REV_COEFF_DEL2, (int)(1550.0 * fscale));
                    set_coeff(REV_COEFF_LPF_CUTOFF, 400.0);
                    set_coeff(REV_COEFF_HPF_CUTOFF, 2000.0);
                    del_len = (int)(AUDIO_FS * 0.5);
                    set_coeff(REV_COEFF_ECHO, del_len);
                    calc_coeffs();
                    break;
            }
            rev = new_rev;
        }

        // get pots
        rev_mix = params[POT_REV_MIX].getValue() * 0.8;
        filter = 0.7;
        size = 0.7;
        krt = (size * 0.25) + 0.6;

        del_mix = params[POT_DEL_MIX].getValue();

        if(peak > 5.0) {
            lights[CLIP_LED].setBrightness(1.0);
            peak = 0.0;
        }
        else {
            lights[CLIP_LED].setBrightness(0.0);
        }
    }

    int set_coeff(int coeff, float val) {
        int temp;
        switch(coeff) {
            case REV_COEFF_API1:
                temp = (int)roundf(val);
                if(temp < 1) {
                    return -1;
                }
                api1 = temp;
                break;
            case REV_COEFF_API2:
                temp = (int)roundf(val);
                if(temp < 1) {
                    return -1;
                }
                api2 = temp;
                break;
            case REV_COEFF_API3:
                temp = (int)roundf(val);
                if(temp < 1) {
                    return -1;
                }
                api3 = temp;
                break;
            case REV_COEFF_API4:
                temp = (int)roundf(val);
                if(temp < 1) {
                    return -1;
                }
                api4 = temp;
                break;
            case REV_COEFF_AP1:
                temp = (int)roundf(val);
                if(temp < 1) {
                    return -1;
                }
                ap1 = temp;
                break;
            case REV_COEFF_DEL1:
                temp = (int)roundf(val);
                if(temp < 1) {
                    return -1;
                }
                del1 = temp;
                break;
            case REV_COEFF_AP2:
                temp = (int)roundf(val);
                if(temp < 1) {
                    return -1;
                }
                ap2 = temp;
                break;
            case REV_COEFF_DEL2:
                temp = (int)roundf(val);
                if(temp < 1) {
                    return -1;
                }
                del2 = temp;
                break;
            case REV_COEFF_LPF_CUTOFF:
                lfilt_a0 = val;
                break;
            case REV_COEFF_HPF_CUTOFF:
                hfilt_a0 = val;
                break;
            case REV_COEFF_ECHO:
                temp = (int)roundf(val);
                if(temp < 1) {
                    return -1;
                }
                echo = temp;
                break;
            default:
                return -1;
        }
        return 0;
    }

    // calculates internal coeffs once values are set
    void calc_coeffs(void) {
        int temp;
        float tempf1;
        // delay lines
        temp = 0;
        api1_in = temp;
        temp += api1;
        api1 = temp;
        temp ++;
        api2_in = temp;
        temp += api2;
        api2 = temp;
        temp ++;
        api3_in = temp;
        temp += api3;
        api3 = temp;
        temp ++;
        api4_in = temp;
        temp += api4;
        api4 = temp;
        temp ++;
        ap1_in = temp;
        temp += ap1;
        ap1 = temp;
        temp ++;
        del1_in = temp;
        temp += del1;
        del1 = temp;
        temp ++;
        ap2_in = temp;
        temp += ap2;
        ap2 = temp;
        temp ++;
        del2_in = temp;
        temp += del2;
        del2 = temp;
        temp ++;
        echo_in = temp;
        temp += echo;
        echo = temp;
        // filters
        tempf1 = lfilt_a0;
        DSP_UTILS_F1SC(tempf1, lfilt_a0);
        tempf1 = hfilt_a0;
        DSP_UTILS_F1SC(tempf1, hfilt_a0);
        lfilt_z1 = 0.0;
        lfilt_z1 = 0.0;
    }
};

struct V103_Reverb_DelayWidget : ModuleWidget {
    ThemeChooser *theme_chooser;

    V103_Reverb_DelayWidget(V103_Reverb_Delay* module) {
        setModule(module);

        theme_chooser = new ThemeChooser(this, DINTREE_THEME_FILE,
            "Classic", asset::plugin(pluginInstance, "res/V103-Reverb_Delay.svg"));
        theme_chooser->addPanel("Dark", asset::plugin(pluginInstance, "res/V103-Reverb_Delay-b.svg"));
        theme_chooser->initPanel();

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(27.623, 24.233)), module, V103_Reverb_Delay::POT_REV_MIX));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(27.623, 50.924)), module, V103_Reverb_Delay::POT_DEL_MIX));
        addParam(createParamCentered<KilpatrickKnobBlackRed>(mm2px(Vec(27.623, 77.594)), module, V103_Reverb_Delay::POT_DEL_TIME));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.324, 41.081)), module, V103_Reverb_Delay::INL));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.324, 55.475)), module, V103_Reverb_Delay::INR));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(11.303, 69.868)), module, V103_Reverb_Delay::OUTL));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(11.303, 85.32)), module, V103_Reverb_Delay::OUTR));

        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(11.324, 30.159)), module, V103_Reverb_Delay::CLIP_LED));

        addChild(createParamCentered<KilpatrickToggle3P>(mm2px(Vec(19.982, 99.798)), module, V103_Reverb_Delay::DEL_SW));
        addChild(createParamCentered<KilpatrickToggle2P>(mm2px(Vec(32.682, 99.798)), module, V103_Reverb_Delay::REV_SW));
    }

    void appendContextMenu(Menu *menu) override {
        V103_Reverb_Delay *module = dynamic_cast<V103_Reverb_Delay*>(this->module);
        assert(module);

        // theme chooser
        theme_chooser->populateThemeChooserMenuItems(menu);
    }

    void step() override {
        V103_Reverb_Delay *module = dynamic_cast<V103_Reverb_Delay*>(this->module);
        if(module) {
            // check theme
            theme_chooser->step();
        }
        Widget::step();
    }
};

Model* modelV103_Reverb_Delay = createModel<V103_Reverb_Delay, V103_Reverb_DelayWidget>("V103-Reverb_Delay");
