/*
 * Levelmeter
 *
 * Copyright 2021: Kilpatrick Audio
 * Written by: Andrew Kilpatrick
 *
 */
#include "plugin.hpp"
#include "utils/DspUtils2.h"
#include "utils/KAComponents.h"
#include "utils/PUtils.h"
#include "utils/ThemeChooser.h"

// levelmeter display source
struct LevelmeterDisplaySource {
    // get the peak meter levels
    virtual void getPeakDbLevels(int chan, float *level, float *peak) { }

//    // get the LUFS levels
//    virtual float getLUFSLevel(void) { return 0.0f; }

    // handle hover scroll
    virtual void onHoverScroll(int id, const event::HoverScroll& e) { }
};

struct V202_Levelmeter : Module, LevelmeterDisplaySource {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		IN_L,
		IN_R,
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
    static constexpr int RT_TASK_RATE = 1000;
    static constexpr float AUDIO_IN_GAIN = 0.1f;
    static constexpr float PEAK_METER_SMOOTHING = 1.0f;
    static constexpr float PEAK_METER_PEAK_HOLD_TIME = 1.0f;
    dsp::ClockDivider taskTimer;
    dsp2::Filter2Pole hpfL;
    dsp2::Filter2Pole hpfR;
    dsp2::Levelmeter peakMeterL;
    dsp2::Levelmeter peakMeterR;

    // constructor
	V202_Levelmeter() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        onReset();
        onSampleRateChange();
	}

    // process a sample
	void process(const ProcessArgs& args) override {
        float inL, inR;
        // run tasks
        if(taskTimer.process()) {
        }

        inL = hpfL.process(inputs[IN_L].getVoltage()) * AUDIO_IN_GAIN;
        inR = hpfR.process(inputs[IN_R].getVoltage()) * AUDIO_IN_GAIN;
        peakMeterL.update(inL);
        peakMeterR.update(inR);
	}

    // samplerate changed
    void onSampleRateChange(void) override {
        taskTimer.setDivision((int)(APP->engine->getSampleRate() / RT_TASK_RATE));
        hpfL.setCutoff(dsp2::Filter2Pole::TYPE_HPF, 10.0f, 0.707f, 1.0f, APP->engine->getSampleRate());
        hpfR.setCutoff(dsp2::Filter2Pole::TYPE_HPF, 10.0f, 0.707f, 1.0f, APP->engine->getSampleRate());
        peakMeterL.setSmoothingFreq(PEAK_METER_SMOOTHING, APP->engine->getSampleRate());
        peakMeterR.setSmoothingFreq(PEAK_METER_SMOOTHING, APP->engine->getSampleRate());
        peakMeterL.setPeakHoldTime(PEAK_METER_PEAK_HOLD_TIME, APP->engine->getSampleRate());
        peakMeterR.setPeakHoldTime(PEAK_METER_PEAK_HOLD_TIME, APP->engine->getSampleRate());
    }

    // module initialize
    void onReset(void) override {
    }

    //
    // callbacks
    //
    // get the peak meter levels
    void getPeakDbLevels(int chan, float *level, float *peak) override {
        if(chan) {
            *level = peakMeterR.getDbLevel();
            *peak = peakMeterR.getPeakDbLevel();
            return;
        }
        *level = peakMeterL.getDbLevel();
        *peak = peakMeterL.getPeakDbLevel();
    }

    // handle hover scroll
    void onHoverScroll(int id, const event::HoverScroll& e) override {
    }

};

// levelmeter display
struct LevelmeterDisplay : widget::TransparentWidget {
    int id;
    LevelmeterDisplaySource *source;
    float rad;
    NVGcolor bgColor;
    KALevelmeter peakMeterL;
    KALevelmeter peakMeterR;

    // create a display
    LevelmeterDisplay(int id, math::Vec pos, math::Vec size) {
        this->id = id;
        this->source = NULL;
        rad = mm2px(2.0);
        box.pos = pos.minus(size.div(2));
        box.size = size;
        bgColor = nvgRGBA(0x00, 0x00, 0x00, 0xff);
        peakMeterL.textColor = nvgRGBA(0xe0, 0xe0, 0xe0, 0xff);
        peakMeterL.bgColor = nvgRGBA(0x30, 0x30, 0x30, 0xff);
        peakMeterL.barColor = nvgRGBA(0x00, 0xe0, 0x00, 0xff);
        peakMeterL.peakColor = nvgRGBA(0xe0, 0x00, 0x00, 0xff);
        peakMeterL.size.x = box.size.x * 0.35f;
        peakMeterL.size.y = box.size.y * 0.8f;
        peakMeterL.pos.x = (box.size.x * 0.3f) - (peakMeterL.size.x * 0.5f);
        peakMeterL.pos.y = box.size.y * 0.05f;
        peakMeterL.setMinLevel(-96.0f);
        peakMeterR.textColor = nvgRGBA(0xe0, 0xe0, 0xe0, 0xff);
        peakMeterR.bgColor = nvgRGBA(0x30, 0x30, 0x30, 0xff);
        peakMeterR.barColor = nvgRGBA(0x00, 0xe0, 0x00, 0xff);
        peakMeterR.peakColor = nvgRGBA(0xe0, 0x00, 0x00, 0xff);
        peakMeterR.size.x = box.size.x * 0.35f;
        peakMeterR.size.y = box.size.y * 0.8f;
        peakMeterR.pos.x = (box.size.x * 0.7f) - (peakMeterR.size.x * 0.5f);
        peakMeterR.pos.y = box.size.y * 0.05f;
        peakMeterR.setMinLevel(-96.0f);
    }

    // draw
    void draw(const DrawArgs& args) override {
        float level, peak;
        if(source == NULL) {
            return;
        }

        // background
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, rad);
        nvgFillColor(args.vg, bgColor);
        nvgFill(args.vg);

        source->getPeakDbLevels(0, &level, &peak);
        peakMeterL.setLevels(level, peak);
        peakMeterL.draw(args);

        source->getPeakDbLevels(1, &level, &peak);
        peakMeterR.setLevels(level, peak);
        peakMeterR.draw(args);
    }

    void onHoverScroll(const event::HoverScroll& e) override {
        if(source) {
            source->onHoverScroll(id, e);
            e.consume(NULL);
            return;
        }
        TransparentWidget::onHoverScroll(e);
    }
};

struct V202_LevelmeterWidget : ModuleWidget {
    ThemeChooser *theme_chooser;

	V202_LevelmeterWidget(V202_Levelmeter* module) {
		setModule(module);
        theme_chooser = new ThemeChooser(this, DINTREE_THEME_FILE,
            "Classic", asset::plugin(pluginInstance, "res/V202-Levelmeter.svg"));
        theme_chooser->addPanel("Dark", asset::plugin(pluginInstance, "res/V202-Levelmeter-b.svg"));
        theme_chooser->initPanel();

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        LevelmeterDisplay *disp = new LevelmeterDisplay(0, mm2px(Vec(15.24, 46.5)), mm2px(Vec(24.0, 64.0)));
        disp->source = module;
        addChild(disp);

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 94.5)), module, V202_Levelmeter::IN_L));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 108.5)), module, V202_Levelmeter::IN_R));
	}
    
    void appendContextMenu(Menu *menu) override {
        V202_Levelmeter *module = dynamic_cast<V202_Levelmeter*>(this->module);
        assert(module);

        // theme chooser
        theme_chooser->populateThemeChooserMenuItems(menu);
    }

    void step() override {
        V202_Levelmeter *module = dynamic_cast<V202_Levelmeter*>(this->module);
        if(module) {
            // check theme
            theme_chooser->step();
        }
        Widget::step();
    }
};

Model* modelV202_Levelmeter = createModel<V202_Levelmeter, V202_LevelmeterWidget>("V202-Levelmeter");
