/*
 * Dintree V100 Scanner
 *
 * Written by: Andrew Kilpatrick
 * Copyright 2020: Andrew Kilpatrick
 *
 * This file is part of dintree-modules.
 *
 * dintree-modules is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * dintree-modules is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CARBON.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "plugin.hpp"

// Dintree V100 Scanner module
struct V100_Scanner : Module {
	enum ParamIds {
		CV_GAIN,
		NUM_PARAMS
	};
	enum InputIds {
		IN1,
		IN2,
		IN3,
		IN4,
		IN5,
		IN6,
		CTRL_IN,
		IN7,
		IN8,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTA,
        OUTB,
		NUM_OUTPUTS
	};
	enum LightIds {
		IN1_LED,
		IN2_LED,
		RAND_LED,
		IN3_LED,
		IN4_LED,
		CV_LED,
		IN5_LED,
		IN6_LED,
		CTRL_LED,
		IN7_LED,
		OUTA_LED,
		IN8_LED,
		OUTB_LED,
		NUM_LIGHTS
	};

    // constructor
	V100_Scanner() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(CV_GAIN, 0.f, 1.f, 0.f, "");
	}

    // process a sample
	void process(const ProcessArgs& args) override {
        lights[RAND_LED].setBrightness(params[CV_GAIN].getValue());
	}

    // samplerate changed
    void onSampleRateChange() {
        DEBUG("moo");
    }

    // module initialize
    void onReset() {

    }

    // module randomize
    void onRandomize() {

    }

    // module added to engine
    void onAdd() {

    }

    // module removed from engine
    void onRemove() {

    }
};

// widgets
struct V100_ScannerWidget : ModuleWidget {
	V100_ScannerWidget(V100_Scanner* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/V100-Scanner.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(29.058, 26.163)), module, V100_Scanner::CV_GAIN));

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

		addChild(createWidgetCentered<Widget>(mm2px(Vec(29.079, 45.192))));
		addChild(createWidgetCentered<Widget>(mm2px(Vec(29.079, 64.242))));
	}
};


Model* modelV100_Scanner = createModel<V100_Scanner, V100_ScannerWidget>("V100-Scanner");
