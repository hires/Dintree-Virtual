# Dintree-Virtual

This repo contains Dintree Virtual Rack Modules for
[VCV Rack Virtual Eurorack DAW](https://vcvrack.com). Dintree is
the name of DIY open-source modules created by Andrew Kilpatrick of
Kilpatrick Audio. Please visit the links below for information about Andrew
and his DIY and commercial music and audio projects.

**Useful links:**

* [VCV Rack Virtual Eurorack DAW](https://vcvrack.com) - VCV Rack is what this code runs on
* [Dintree Synth DIY Site](http://dintree.com) - The SDIY site on which many of these modules are based
* [Andrew Kilpatrick](https://www.andrewkilpatrick.org) - Creator of these modules (hardware and software)
* [Kilpatrick Audio](https://www.kilpatrickaudio.com) - Commercial music products from the creator

## Licence

All code in this repository is licensed under GPL 3.0 or later. All panel designs, graphics, logos
and other media files are copyright Andrew Kilpatrick and/or others and may not be used, forked or
redistributed without permission.

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

<br clear="right"/>

----

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

<br clear="right"/>

----

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

#### LEVEL Controls

The LEVEL controls affect the level of the input signal before it reaches the
pan pot. The control offers enough gain so that a mid-range setting is probably
best in most cases.

#### PAN Controls

The PAN controls alter the relative balance of the signal from an input into
the left and right mix busses. In the centre both channels are the same level.
Turning to an extreme end will cause about a 6dB boost in that output channel
and the other channel will go all the way to zero.

#### MASTER Control

The MASTER control affects the master output jacks. A midrange setting is
probably best in most cases. You can use the levelmeters to get a sense of the
overall output level. Aiming for around 0dB will ensure you drive the next
module with a decent level.

<br clear="right"/>

----

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

<br clear="right"/>

----

### V104 Four Vs
**Four Channel Voltage Source**

<img align="right" src="https://github.com/hires/Dintree-Virtual/raw/master/res/images/V104-Four_Vs.png" />

The V104 Four Vs is a VCV clone of the [Dintree D104 Four Vs](http://dintree.com/#D104)
DIY module by Andrew Kilpatrick. It just produces four different voltages but
can be used for testing and triggering modules that require a voltage present
to operate. Unlike the hardware version, this module produces a bi-polar voltage
which is probably more useful. Also unlike the hardware version, the V104 has
on/off switches on the first two outputs. This lets you toggle on and off a
preset voltage.

#### VOLTAGE Controls

Each output has a dedicated voltage control. In the middle the output will be
about zero volts. By turning counter-clockwise you can create as much as -5V,
and by turning fully clockwise you can create as much as +5V. Manual control like
this is super useful for testing the range of voltages a module can handle as well
as manually adjusting some simpler modules that have input jacks to trigger
functions but no actual panel knobs or buttons to do it manually.

#### ON Switches

As an added bonus the first two channels feature ON switches which can be used
to turn on and off an output. If you want to test a particular voltage and then
try adding and removing that voltage, you can use the ON switch to do this
while leaving the voltage setting preset on the VOLTAGE control.

This can be used to manually generate gates or pulses that are easy to control
with the mouse.

<br clear="right"/>

----

### V105 Quad CV Proc
**Quad Control Voltage Processor**

<img align="right" src="https://github.com/hires/Dintree-Virtual/raw/master/res/images/V105-Quad_CV_Proc.png" />

The V105 Quad CV Proc is a VCV clone of the [Dintree D105 Quad CV Proc](http://dintree.com/#D105)
DIY module by Andrew Kilpatrick. It provides four channels of simple CV
processing which allow signals to be mixed, gained up and down, and inverted.
Two inputs on each channel are buffered and mixed together. The gain control
allows the final output to be set from 0x to 2x, offering both attenuation
and gain. Dual outputs from each section offer non-inverted (+) and inverted (-)
outputs.

#### Dual Inputs

Each of the four channels contain two inputs labeled A and B. You can feed two
signals and they will be actively buffered and mixed together before being sent
to the GAIN control. Use this as a basic signal mixer for CV or audio signals.

#### GAIN Controls

Each channel features a GAIN control which is wired after the two inputs are
mixed together. The GAIN controls are marked with the gain range from 0x to 2x.
At the full counter-clockwise position the output signal will be zero. At the
mid-range position the signal will be passed through unaltered. At the full
setting the signal will be doubled in amplitude. (200% or +6dB) Use this to
bring up weak signals or adjust the range of a signal into a module that lacks
any kind of input attenuator.

#### Dual Outputs

Once gained up or down, the signal is fed into a pair of outputs. The + output
carries a non-inverting signal. With the GAIN control set to mid-range, this
jack will create a perfect copy of the input. The - output carries an inverted
version of the signal. Use this instead of an "attenuverter" which is a clumsy
way to adjust gain and invert a signal. By using both outputs at the same time
you can easily make opposing CV signals for such things as panning circuits,
stereo phasing effects, or multiple filters which cross over for unique timbral
characteristics.

<br clear="right"/>

----

### V107 Dual Slew

**Dual Slew Rate Limiter**

<img align="right" src="https://github.com/hires/Dintree-Virtual/raw/master/res/images/V107-Dual_Slew.png" />

The V107 Dual Slew is a VCV clone of the [Dintree D107 Dual Slew](http://dintree.com/#D107)
DIY module by Andrew Kilpatrick. It simulates a basic RC filter slew rate limiter
which in the hardware version is based around a large variable resistor feeding a
pair of back to back electrolytic capacitors. The output of the RC junction is
sampled and buffered by a TL082 JFET input op amp which provides nearly zero
current draw. This means that the output signal lags the input by an amount that
is based solely on how fast the capacitors can be charged and discharged via the
variable resistor. A low impedance source is used internally so that the particular
signal fed into the module makes no difference. (in VCV rack of course this is
arbitrary since inputs don't load down cables... but anyway...)

The most common use of a slew rate limiter is for use in portamento by feeding
a control voltage through the slew unit and then into a VCO pitch CV input. But
because a simple RC filter has a log curve the pitch will not track evenly per
octave when used with a V/oct type of CV signal. Another use is for making a
simple lag processor to convert a single CV into multiple versions that have
slightly different offsets. This can create interesting effects with filters.

<br clear="right"/>

----
### V201 Tri Comparator
**Analog Comparator Functions**

<img align="right" src="https://github.com/hires/Dintree-Virtual/raw/master/res/images/V201-Tri_Comparator.png" />

The V201 Tri Comparator performs 16 different voltage comparison functions between three
inputs. The inputs are labeled IN A, IN B and IN C and accept any range of voltages. The
outputs each produce a function by comparing the three inputs in different combinations.
Possible comparison functions are:

- **=** - Equal - A pair of inputs are said to be equal if they are within 0.01V.
- **<** - Less Than - The left input is less than the right input.
- **>** - Greater Than - The left input is greater than the right input.
- **&#8800;** - Not Equal - The inverse of Equal

The functions that operate on each of the 16 outputs are as follows:

- **A = B = C**
- **A = B < C**
- **A = B > C**
- **A = B ≠ C**
- **A < B = C**
- **A < B < C**
- **A < B > C**
- **A < B ≠ C**
- **A > B = C**
- **A > B < C**
- **A > B > C**
- **A > B ≠ C**
- **A ≠ B = C**
- **A ≠ B < C**
- **A ≠ B > C**
- **A ≠ B ≠ C**

All outputs are processed at the full system rate. You might want to use a slew
processor or other type of filter to slow down very fast signals so they will create
more interesting outputs, especially when using the equal and not equal functions.

<br clear="right"/>

----
### V218 S&H / Clock / Noise Generator
**Sample and Hold, Clock and Noise Generator**

<img align="right" src="https://github.com/hires/Dintree-Virtual/raw/master/res/images/V218-SH-Clock-Noise.png" />

The V218 S&H / Clock / Noise Generator is basically three modules in one. It is
based on the Aries AR-318 module of a similar name. Please note that this is not
a hardware clone and the functionality and behaviour is based on looking at the
schematic and front panel controls. No calculations or models were made to verify
the same behaviour.

**Sample and Hold Section**

The sample and hold (S&H) section can be used to capture and "hold" a voltage
for as long as necessary. An audio or control voltage is input into the SH IN jack.
The sampled / held voltage is output from the SH OUT jack. There are two ways to
operate the sample and hold circuit:

- **GATE IN** - The GATE IN jack will cause the signal to "track" or pass through as long as
the gate signal is above +1.0 volts. When the gate falls below +1.0V the most recent
value is held on the output.

**TRIG IN** - The TRIG IN jack will cause the current input voltage to be sampled.
The sample happens on the positive edge of the signal. Using the TRIG IN there is no
"tracking" of the input signal, the input is sampled at only one moment in time for
each positive edge of the TRIG IN signal.

The level of the SH OUT signal can be adjusted with the S&H LEVEL control. This signal
happens after the sample and hold circuit and does not affect the sampling.

**Clock Section**

The clock section is a convenient clock generator with a range from 0.3Hz to 30Hz per
cycle. The frequency is adjusted with the CLOCK FREQ control. The clock has three
simultanous outputs:

- **SAW OUT** - This is a voltage which ranges from 0 to +10V and is the internal
"core" of the clock generator.
- **SQ OUT** - The SQ OUT jack produces a square with (50% duty cycle) which goes
high in the middle of each saw wave cycle, and resets at the end of each saw wave.
- **TRIG OUT** - The TRIG OUT jack produces a short pulse every time the SQ OUT goes
high. The difference is that the TRIG OUT signal is only on for 5ms.

There are several inputs that can be used to control the clock generator:

- **FM IN** - The FM IN jack allows a voltage to adjust the frequency of the clock.
A bipolar signal will add or subtract from the current setting of the CLOCK FREQ control.
The range of frequencies that can be produced is clamped to the the range of the CLOCK FREQ
control: 0.3Hz to 30Hz.
- **SYNC IN** - The SYNC IN jack allows the clock generator sawtooth core to be reset
at any point in the cycle. This can be used to create syncopating or chaotic pulses.

**Noise Generator**

The noise generator uses a white noise source to develop three useful types of noise
which come out on three jacks:

- **W OUT** - Raw white noise signal from a random number generator in the computer.
This signal has an amplitude of 20V peak to peak.
- **P OUT** - A filtered pink noise source that is made from the white noise signal.
It uses a three stage filter to develop a pink noise approximation of -3dB/oct
(-10dB/decade) filtered signal. This is useful for audio tests or if you don't want
to blow your speakers. The output level is a bit less than the white noise signal.
- **R OUT** - The R OUT jack produces a slowly varying voltage. It uses a modified
peak detector and two stages of low-pass filtering similar to the Aries circuit. It
can be used to generate weird warbly pitches or other kinds of control voltage rate
random voltages.
