# Dintree-VCV

This repo contains Dintree VCV Rack Modules. Dintree is the name of DIY
open-source modules created by Andrew Kilpatrick of Kilpatrick Audio.

This code is is licensed under GPL with the additional requirement
that commercial hardware versions of this module, code or concept
not be made without express written permission from Andrew Kilpatrick.

**Useful links:**

* [Dintree Synth DIY Site](http://dintree.com)
* [Andrew Kilpatrick](https://www.andrewkilpatrick.org)
* [Kilpatrick Audio](https://www.kilpatrickaudio.com)

## Modules

### V100 Scanner
**CV and voltage-control source sequencer / scanner**

<img align="right" src="https://github.com/hires/Dintree-VCV/raw/master/res/images/V100-Scanner.png" />

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

#### CV Gain Control

The CV GAIN control affects the range of outputs selected in either CV or clock
mode. A range of 0-100% will select from 1 to 8 inputs to be included in the
selection. For instance a setting of around 50% will cause only inputs 1-4 to
be selected.

#### RAND Control

When the RAND switch is on, instead of sequentially choosing inputs, either by
CV or clock, a random input is selected. The RAND control observes the CV GAIN
setting so only inputs within the chosen range will be selected.
