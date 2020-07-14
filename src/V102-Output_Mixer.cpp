#include "plugin.hpp"


struct V102_Output_Mixer : Module {
	enum ParamIds {
		PATH1871_7_PARAM,
		PATH1871_3_PARAM,
		PATH1871_2_PARAM,
		PATH1871_65_PARAM,
		PATH1871_5_PARAM,
		PATH1871_26_PARAM,
		PATH1871_60_PARAM,
		PATH1871_8_PARAM,
		PATH1871_79_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		PATH1871_6_4_INPUT,
		PATH1871_6_10_INPUT,
		PATH1871_6_48_INPUT,
		PATH1871_6_70_INPUT,
		PATH1871_6_8_INPUT,
		PATH1871_6_62_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		PATH1871_6_0_3_4_OUTPUT,
		PATH1871_6_0_3_4_7_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		PATH1871_6_6_39_LIGHT,
		PATH1871_6_6_39_2_LIGHT,
		PATH1871_6_6_39_8_LIGHT,
		PATH1871_6_6_39_3_LIGHT,
		PATH1871_6_6_39_0_LIGHT,
		PATH1871_6_6_39_1_LIGHT,
		PATH1871_6_6_39_7_LIGHT,
		PATH1871_6_6_39_89_LIGHT,
		PATH1871_6_6_39_15_LIGHT,
		PATH1871_6_6_39_4_LIGHT,
		NUM_LIGHTS
	};

	V102_Output_Mixer() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PATH1871_7_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PATH1871_3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PATH1871_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PATH1871_65_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PATH1871_5_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PATH1871_26_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PATH1871_60_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PATH1871_8_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PATH1871_79_PARAM, 0.f, 1.f, 0.f, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct V102_Output_MixerWidget : ModuleWidget {
	V102_Output_MixerWidget(V102_Output_Mixer* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/V102-Output_Mixer.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<DintreeKnobBlackRed>(mm2px(Vec(27.579, 24.255)), module, V102_Output_Mixer::PATH1871_7_PARAM));
		addParam(createParamCentered<DintreeKnobBlackRed>(mm2px(Vec(50.44, 24.255)), module, V102_Output_Mixer::PATH1871_3_PARAM));
		addParam(createParamCentered<DintreeKnobBlackRed>(mm2px(Vec(27.579, 50.904)), module, V102_Output_Mixer::PATH1871_2_PARAM));
		addParam(createParamCentered<DintreeKnobBlackRed>(mm2px(Vec(50.461, 50.904)), module, V102_Output_Mixer::PATH1871_65_PARAM));
		addParam(createParamCentered<DintreeKnobBlackRed>(mm2px(Vec(50.461, 77.574)), module, V102_Output_Mixer::PATH1871_5_PARAM));
		addParam(createParamCentered<DintreeKnobBlackRed>(mm2px(Vec(27.58, 77.616)), module, V102_Output_Mixer::PATH1871_26_PARAM));
		addParam(createParamCentered<DintreeKnobBlackRed>(mm2px(Vec(75.84, 77.616)), module, V102_Output_Mixer::PATH1871_60_PARAM));
		addParam(createParamCentered<DintreeKnobBlackRed>(mm2px(Vec(27.579, 104.197)), module, V102_Output_Mixer::PATH1871_8_PARAM));
		addParam(createParamCentered<DintreeKnobBlackRed>(mm2px(Vec(50.461, 104.197)), module, V102_Output_Mixer::PATH1871_79_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.339, 44.236)), module, V102_Output_Mixer::PATH1871_6_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.361, 57.571)), module, V102_Output_Mixer::PATH1871_6_10_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.361, 70.906)), module, V102_Output_Mixer::PATH1871_6_48_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.361, 84.262)), module, V102_Output_Mixer::PATH1871_6_70_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.339, 97.575)), module, V102_Output_Mixer::PATH1871_6_8_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.361, 110.932)), module, V102_Output_Mixer::PATH1871_6_62_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.193, 104.244)), module, V102_Output_Mixer::PATH1871_6_0_3_4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(82.55, 104.244)), module, V102_Output_Mixer::PATH1871_6_0_3_4_7_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(72.051, 24.255)), module, V102_Output_Mixer::PATH1871_6_6_39_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(79.692, 24.255)), module, V102_Output_Mixer::PATH1871_6_6_39_2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(72.051, 31.832)), module, V102_Output_Mixer::PATH1871_6_6_39_8_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(79.692, 31.832)), module, V102_Output_Mixer::PATH1871_6_6_39_3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(72.051, 39.453)), module, V102_Output_Mixer::PATH1871_6_6_39_0_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(79.692, 39.453)), module, V102_Output_Mixer::PATH1871_6_6_39_1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(72.051, 47.072)), module, V102_Output_Mixer::PATH1871_6_6_39_7_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(79.692, 47.073)), module, V102_Output_Mixer::PATH1871_6_6_39_89_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(72.051, 54.692)), module, V102_Output_Mixer::PATH1871_6_6_39_15_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(79.692, 54.692)), module, V102_Output_Mixer::PATH1871_6_6_39_4_LIGHT));
	}
};


Model* modelV102_Output_Mixer = createModel<V102_Output_Mixer, V102_Output_MixerWidget>("V102-Output_Mixer");
