# Dintree-Virtual

This repo contains Dintree Virtual Rack Modules for VCV Rack. Dintree is
the name of DIY open-source modules created by Andrew Kilpatrick of
Kilpatrick Audio. Please visit the links below for information about Andrew
and his DIY and commercial music and audio projects.

**Useful links:**

* [Dintree Synth DIY Site](http://dintree.com)
* [Andrew Kilpatrick](https://www.andrewkilpatrick.org)
* [Kilpatrick Audio](https://www.kilpatrickaudio.com)

## Modules

### V100 Scanner
**CV and voltage-control source sequencer / scanner**

<img align="right" src="https://github.com/hires/Dintree-Virtual/raw/master/res/images/V100-Scanner.png" />

The V100 Scanner module can work as a voltage-controlled input selector,
an input sequencer/switcher, or a source of randomly switching inputs. There
are 8 inputs which feed to the two output: OUT A and OUT B. One input is
always connected to the output at one time. The most recently selected input
(shown on the IN 1-8 LEDs) will come out of OUT A. The previously selected
input will come out of OUT B.

#### CTRL CV Mode

When the CTRL switch is up, CV mode is engaged. In this mode the CTRL IN
jack will be read as an analog CV input. A voltage from 0 to 10V will
activate one of the 8 inputs.

#### CTRL Clock Mode

When the CTRL switch is down, clock mode is engaged. In this mode the CTRL IN
jack will be read as a clock input. Each positive edge (>1V) will cause the
input selector to increment by 1 step.

#### RANGE Control

The RANGE control affects the range of outputs selected in either CV or clock
mode. Adjusting from 0-100% will select from 1 to 8 inputs to be included in
the scan. For instance a setting of around 50% will cause only inputs 1-4 to
be selected.

#### RAND Control

When the RAND switch is on, instead of sequentially choosing inputs, either by
CV or clock, a random input is selected. The RAND control observes the CV GAIN
setting so only inputs within the chosen range will be selected.

### V101 Dual Envelope
**Dual ADSR, AR and LFO Generator**

<img align="right" src="https://github.com/hires/Dintree-Virtual/raw/master/res/images/V101-Dual_Envelope.png" />

The V101 Dual Envelope module is a VCV clone of the [Dintree D101 Dual Envelope](http://dintree.com/#D101)
DIY module by Andrew Kilpatrick. You can use it as a simple ADSR, AR or LFO
with controllable attack and release times. Because it's based on an old
project based around a PIC16 microcontroller, the processing rate is only 1kHz
and the resolution is only 12 bits, so you might hear some steps in the output.
This is part of the clone and the charm! :)

Use the GATE IN jacks for gate signal inputs, and the ENV OUT jacks to get the
envelope or LFO outputs. Other than sharing a panel the two channels are
completely independent.

####ADSR Mode

In ADSR mode the output works in a standard Attack, Decay, Sustain, Release mode.

####AR Mode

In the AR mode a gate trigger starts the Attack / Release cycle which finishes
at its own time.

####LFO Mode

In LFO mode the ATTACK and RELEASE controls affect the UP/DOWN time of the LFO
wave. A small additional feature not included in the hardware version is the
ability to use the GATE IN jack to start and stop the LFO. When switching to LFO
mode, the LFO will run automatically. However, if you input a gate signal, it
will start and stop the LFO. To reset auto-run mode without a gate preset,
simply switch the mode to another setting and back to LFO to reset it.


### V102 Output Mixer
**Four Input Mixer with Pan Pots and Output Level Meter**

<img align="right" src="https://github.com/hires/Dintree-Virtual/raw/master/res/images/V102-Output_Mixer.png" />

The V102 Output Mixer is a VCV clone of the [Dintree D102 Output Mixer](dintree.com/#D102)
DIY module by Andrew Kilpatrick. You can use it to mix 4 sources including smooth
level and pan controls for each input. There is a master output level and stereo
level meter. Unlike the hardware version, the V102 also includes sub inputs which
feed into the master mix bus. This can be used as a convenient effects return or
as a way to daisy-chain multiple mixers together.
