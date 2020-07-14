# Dintree-Virtual

This repo contains Dintree Virtual Rack Modules for
[VCV Rack Virtual Eurorack DAW](https://vcvrack.com). Dintree is
the name of DIY open-source modules created by Andrew Kilpatrick of
Kilpatrick Audio. Please visit the links below for information about Andrew
and his DIY and commercial music and audio projects.

**Useful links:**

* [VCV Rack Virtual Eurorack DAW](https://vcvrack.com)
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

#### ADSR Mode

In ADSR mode the output works in a standard Attack, Decay, Sustain, Release mode.

#### AR Mode

In the AR mode a gate trigger starts the Attack / Release cycle which finishes
at its own time.

#### LFO Mode

In LFO mode the ATTACK and RELEASE controls affect the UP/DOWN time of the LFO
wave. A small additional feature not included in the hardware version is the
ability to use the GATE IN jack to start and stop the LFO. When switching to LFO
mode, the LFO will run automatically. However, if you input a gate signal, it
will start and stop the LFO. To reset auto-run mode without a gate preset,
simply switch the mode to another setting and back to LFO to reset it.

### V102 Output Mixer
**Four Input Mixer with Pan Pots and Output Level Meter**

<img align="right" src="https://github.com/hires/Dintree-Virtual/raw/master/res/images/V102-Output_Mixer.png" />

The V102 Output Mixer is a VCV clone of the [Dintree D102 Output Mixer](http://dintree.com/#D102)
DIY module by Andrew Kilpatrick. You can use it to mix 4 sources including smooth
level and pan controls for each input. There is a master output level and stereo
level meter. The meter indicates in steps of 6dB. The 0dB indicator is at 5Vpk
(10Vpp) which is considered the nominal level for most audio signals in Eurorack.

**VCV-only Features!**
Unlike the hardware version, the V102 also includes SUB INs jacks which feed
into the master mix bus. These can be used as a convenient effects return or as
a way to daisy-chain multiple mixers together.

Also included are the PRE OUT jacks which gives the pre-master mix and are not
affected by the level of the master control. These can be used for sending the
mix to a master reverb unit, for instance. If you bring the reverb back in via
the SUB IN jacks, you will have an effects loop controlled only by the reverb
module itself.

### V103 Reverb Delay
**Stereo Reverb and Delay Digital Effects Processor**

<img align="right" src="https://github.com/hires/Dintree-Virtual/raw/master/res/images/V103-Reverb_Delay.png" />

The V103 Reverb Delay is a VCV clone of the [Dintree D103 Reverb / Delay](http://dintree.com/#D103)
DIY module by Andrew Kilpatrick. It implements a simple yet effective reverb
algorithm that sounds good on many kinds of sound sources. There is also an
integrated delay line with up to 0.5 seconds of delay. There are three output
modes which offer different mono and stereo taps for useful delay sounds. A clip
LED indicates that the output signal might be too hot.

Please note that the V103 is designed for use with an effects loop and not pass
any dry signal.

#### Reverb Mix

The REVERB MIX control affects the amount of reverb in the output.

#### Delay Mix

The DELAY MIX control affects the amount of delayed signal in the output.

#### Delay Time

The DELAY TIME control sets the total length of the delay line. From 0-500ms.

#### Delay Type

The delay type switch chooses how the delayed signal is created:

- **DEL1** - Mono delay output.
- **DEL2** - Mono delay output plus stereo syncopated echos at 1/3 and 2/3 of the delay time.
- **DEL3** - Mono delay output plus stereo syncopated echos in 1/4 and 3/4 of the delay time.

#### Reverb Type

The reverb type switch selects one of two reverb sounds: BIG or SMALL.
