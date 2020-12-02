/*
 * Dintree V218 SH Clock Noise
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
#include "utils/dsp_utils.h"

struct V218_SH_Clock_Noise : Module {
	enum ParamIds {
		SH_LEVEL_POT,
		CLOCK_FREQ_POT,
		NUM_PARAMS
	};
	enum InputIds {
		SH_GATE_IN,
		CLOCK_SYNC_IN,
		SH_TRIG_IN,
		CLOCK_FM_IN,
		SH_IN,
		NUM_INPUTS
	};
	enum OutputIds {
        CLOCK_TRIG_OUT,
		NOISE_W_OUT,
		CLOCK_SAW_OUT,
		NOISE_P_OUT,
		SH_OUT,
		CLOCK_SQ_OUT,
		NOISE_R_OUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		SH_GATE_IN_LED,
		CLOCK_SYNC_IN_LED,
		CLOCK_TRIG_OUT_LED,
		SH_TRIG_IN_LED,
		ENUMS(CLOCK_FM_IN_LED, 3),
		ENUMS(SH_IN_LED, 3),
		CLOCK_SAW_OUT_LED,
		ENUMS(SH_OUT_LED, 3),
		CLOCK_SQ_OUT_LED,
		ENUMS(NOISE_R_OUT_LED, 3),
		NUM_LIGHTS
	};

    // settings
    #define BICOLOR_LED_SCALE 0.2f
    #define CLOCK_FREQ_MIN 0.3f
    #define CLOCK_FREQ_MAX 30.0f
    #define RT_TASK_RATE 100.0f  // Hs
    #define CLOCK_SYNC_LED_PULSELEN 0.05f  // seconds
    #define SH_TRIG_LED_PULSELEN 0.05f  // seconds
    #define CLOCK_TRIG_PULSELEN 0.005f  // seconds
    dsp::ClockDivider taskTimer;
    struct ModuleDefaults module_defaults;
    float AUDIO_FS, CLOCK_FMIN, CLOCK_FMAX, SAMPLE_LEN;
    float clock_freq, clock_pa;
    int clock_trig_out_hist;  // old state of trigger out
    int clock_sync_hist;  // old state of sync in
    int sh_trig_hist;  // old state of SH trig in
    dsp::PulseGenerator clockTrigPulseGen;
    dsp::PulseGenerator clockSyncInGen;
    dsp::PulseGenerator shTrigPulseGen;
    float pink_state[3];
    float rand_state[3];

	V218_SH_Clock_Noise() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(SH_LEVEL_POT, 0.f, 1.f, 0.f, "S&H LEVEL");
		configParam(CLOCK_FREQ_POT, 0.0f, 1.0f, 0.f, "CLOCK FREQ");

        // load module defaults from user file
        loadDefaults(&module_defaults);

        // reset stuff
        onReset();
        onSampleRateChange();
        setParams();
	}

    // process a sample
	void process(const ProcessArgs& args) override {
        int temp, samp;
        float tempf, wn, pn, rn;
        // state
        if(taskTimer.process()) {
            setParams();
        }

        //
        // S&H
        //
        samp = 0;
        // trigger input
        temp = (int)(inputs[SH_TRIG_IN].getVoltage());
        if(temp && !sh_trig_hist) {
            shTrigPulseGen.trigger(SH_TRIG_LED_PULSELEN);
            samp = 1;
        }
        sh_trig_hist = temp;
        if(shTrigPulseGen.process(SAMPLE_LEN)) {
            lights[SH_TRIG_IN_LED].setBrightness(1.0f);
        }
        else {
            lights[SH_TRIG_IN_LED].setBrightness(0.0f);
        }
        // gate input
        if(inputs[SH_GATE_IN].getVoltage() > 1.0f) {
            lights[SH_GATE_IN_LED].setBrightness(1.0f);
            samp |= 1;
        }
        else {
            lights[SH_GATE_IN_LED].setBrightness(0.0f);
        }
        // sample input
        tempf = inputs[SH_IN].getVoltage();
        if(samp) {
            outputs[SH_OUT].setVoltage(tempf * params[SH_LEVEL_POT].getValue());
            lights[SH_OUT_LED + 0].setBrightness(clamp(-tempf, 0.0f, 1.0f));  // red
            lights[SH_OUT_LED + 1].setBrightness(clamp(tempf, 0.0f, 1.0f));  // green
        }
        lights[SH_IN_LED + 0].setBrightness(clamp(-tempf, 0.0f, 1.0f));  // red
        lights[SH_IN_LED + 1].setBrightness(clamp(tempf, 0.0f, 1.0f));  // green

        //
        // clock
        //
        // retrig
        temp = (int)(inputs[CLOCK_SYNC_IN].getVoltage());
        if(temp && !clock_sync_hist) {
            clock_pa = 0.0f;
            clockSyncInGen.trigger(CLOCK_SYNC_LED_PULSELEN);
        }
        if(clockSyncInGen.process(SAMPLE_LEN)) {
            lights[CLOCK_SYNC_IN_LED].setBrightness(1.0f);
        }
        else {
            lights[CLOCK_SYNC_IN_LED].setBrightness(0.0f);
        }
        clock_sync_hist = temp;
        // saw core
        tempf = inputs[CLOCK_FM_IN].getVoltage();
        DSP_UTILS_RLP(clamp(clock_freq + (tempf * 0.1 * CLOCK_FMAX),
            CLOCK_FMIN, CLOCK_FMAX), clock_pa);
        outputs[CLOCK_SAW_OUT].setVoltage(clock_pa * 10.0f);
        lights[CLOCK_SAW_OUT_LED].setBrightness(clock_pa);
        tempf *= BICOLOR_LED_SCALE;
        lights[CLOCK_FM_IN_LED + 0].setBrightness(clamp(-tempf, 0.0f, 1.0f));  // red
        lights[CLOCK_FM_IN_LED + 1].setBrightness(clamp(tempf, 0.0f, 1.0f));  // green
        // square out
        temp = (int)(clock_pa * 2.0f);
        outputs[CLOCK_SQ_OUT].setVoltage((float)temp * 10.0f);
        lights[CLOCK_SQ_OUT_LED].setBrightness((float)temp);
        // trig out
        if(temp && !clock_trig_out_hist) {
            clockTrigPulseGen.trigger(CLOCK_TRIG_PULSELEN);
        }
        clock_trig_out_hist = temp;
        if(clockTrigPulseGen.process(SAMPLE_LEN)) {
            outputs[CLOCK_TRIG_OUT].setVoltage(10.0f);
        }
        else {
            outputs[CLOCK_TRIG_OUT].setVoltage(0.0f);
        }

        //
        // noise
        //
        // white
        wn = random::uniform() - 0.5f;
        outputs[NOISE_W_OUT].setVoltage(wn * 20.0f);
        // pink
        pink_state[0] = 0.99765 * pink_state[0] + wn * 0.0990460;
        pink_state[1] = 0.96300 * pink_state[1] + wn * 0.2965164;
        pink_state[2] = 0.57000 * pink_state[2] + wn * 1.0526913;
        pn = pink_state[0] + pink_state[1] + pink_state[2] + wn * 0.1848f;
        outputs[NOISE_P_OUT].setVoltage(pn * 3.0f);
        // rand
        DSP_UTILS_LS(pn, rand_state[0], 0.001f, 0.0001f);
        DSP_UTILS_F1LP(rand_state[0], rn, 0.999999999f, rand_state[1]);
        DSP_UTILS_F1LP(rand_state[1], rn, 0.999999999f, rand_state[2]);
        rn -= 1.0f;
        rn *= 8.0f;
        outputs[NOISE_R_OUT].setVoltage(rn);
        lights[NOISE_R_OUT_LED + 0].setBrightness(clamp(-tempf, 0.0f, 1.0f));  // red
        lights[NOISE_R_OUT_LED + 1].setBrightness(clamp(tempf, 0.0f, 1.0f));  // green
	}

    // samplerate changed
    void onSampleRateChange(void) override {
        AUDIO_FS = APP->engine->getSampleRate();
        SAMPLE_LEN = 1.0f / AUDIO_FS;
        taskTimer.setDivision((int)(AUDIO_FS / RT_TASK_RATE));
    }

    // module initialize
    void onReset(void) override {
        // disable blue LEDs
        lights[CLOCK_FM_IN_LED + 2].setBrightness(0.0f);
        lights[SH_IN_LED + 2].setBrightness(0.0f);
        lights[SH_OUT_LED + 2].setBrightness(0.0f);
        lights[NOISE_R_OUT_LED + 2].setBrightness(0.0f);
        // reset stuff
        clock_pa = 0.0f;
        clock_sync_hist = 0;
        clock_trig_out_hist = 0;
        clockTrigPulseGen.reset();
        clockSyncInGen.reset();
        shTrigPulseGen.reset();
        sh_trig_hist = 0;
        params[SH_LEVEL_POT].setValue(1.0f);
        params[CLOCK_FREQ_POT].setValue(0.5f);
        pink_state[0] = 0.0f;
        pink_state[1] = 0.0f;
        pink_state[2] = 0.0f;
        rand_state[0] = 1.0f;  // start with offset
        rand_state[1] = 1.0f;
        rand_state[2] = 1.0f;
    }

    // module randomize
    void onRandomize(void) override {
        // no action
    }

    // module added to engine
    void onAdd(void) override {
        // no action
    }

    // module removed from engine
    void onRemove(void) override {
        // no action
    }

    // set params based on input
    void setParams(void) {
        float tempf;
        tempf = params[CLOCK_FREQ_POT].getValue();
        DSP_UTILS_RLSFP(clock_freq, ((tempf * tempf * tempf) * \
            (CLOCK_FREQ_MAX - CLOCK_FREQ_MIN)) + CLOCK_FREQ_MIN, AUDIO_FS);
        DSP_UTILS_RLSFP(CLOCK_FMIN, CLOCK_FREQ_MIN, AUDIO_FS);
        DSP_UTILS_RLSFP(CLOCK_FMAX, CLOCK_FREQ_MAX, AUDIO_FS);
    }
};


struct V218_SH_Clock_NoiseWidget : ModuleWidget {
    SvgPanel* darkPanel;

	V218_SH_Clock_NoiseWidget(V218_SH_Clock_Noise* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/V218-SH_Clock_Noise.svg")));

        darkPanel = new SvgPanel();
        darkPanel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/V218-SH_Clock_Noise-dark.svg")));
        darkPanel->visible = false;
        addChild(darkPanel);

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<DintreeKnobBlackRed>(mm2px(Vec(11.4, 22.5)), module, V218_SH_Clock_Noise::SH_LEVEL_POT));
		addParam(createParamCentered<DintreeKnobBlackRed>(mm2px(Vec(39.4, 22.5)), module, V218_SH_Clock_Noise::CLOCK_FREQ_POT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.4, 46.5)), module, V218_SH_Clock_Noise::SH_GATE_IN));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.4, 46.5)), module, V218_SH_Clock_Noise::CLOCK_SYNC_IN));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.4, 68.5)), module, V218_SH_Clock_Noise::SH_TRIG_IN));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.4, 68.5)), module, V218_SH_Clock_Noise::CLOCK_FM_IN));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.4, 90.5)), module, V218_SH_Clock_Noise::SH_IN));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.4, 46.5)), module, V218_SH_Clock_Noise::CLOCK_TRIG_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.4, 68.5)), module, V218_SH_Clock_Noise::NOISE_W_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.4, 90.5)), module, V218_SH_Clock_Noise::CLOCK_SAW_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.4, 90.5)), module, V218_SH_Clock_Noise::NOISE_P_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(11.4, 112.5)), module, V218_SH_Clock_Noise::SH_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.4, 112.5)), module, V218_SH_Clock_Noise::CLOCK_SQ_OUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.4, 112.5)), module, V218_SH_Clock_Noise::NOISE_R_OUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(11.4, 34.5)), module, V218_SH_Clock_Noise::SH_GATE_IN_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(25.4, 34.5)), module, V218_SH_Clock_Noise::CLOCK_SYNC_IN_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(39.4, 34.5)), module, V218_SH_Clock_Noise::CLOCK_TRIG_OUT_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(11.4, 56.5)), module, V218_SH_Clock_Noise::SH_TRIG_IN_LED));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(25.4, 56.5)), module, V218_SH_Clock_Noise::CLOCK_FM_IN_LED));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(11.4, 78.5)), module, V218_SH_Clock_Noise::SH_IN_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(25.4, 78.5)), module, V218_SH_Clock_Noise::CLOCK_SAW_OUT_LED));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(11.4, 100.5)), module, V218_SH_Clock_Noise::SH_OUT_LED));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(25.4, 100.5)), module, V218_SH_Clock_Noise::CLOCK_SQ_OUT_LED));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(39.4, 100.5)), module, V218_SH_Clock_Noise::NOISE_R_OUT_LED));
	}
};

Model* modelV218_SH_Clock_Noise = createModel<V218_SH_Clock_Noise, V218_SH_Clock_NoiseWidget>("V218-SH-Clock-Noise");
