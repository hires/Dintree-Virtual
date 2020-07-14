/*
 * Dintree DSP Utils
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
 #include <math.h>

#define DSP_UTILS_VSA (1.0 / 4294967295.0)  // very small number

// absolute value
#define DSP_UTILS_ABS(x) ((x)<0 ? -(x) : (x))

// clamp a value to -1.0 to +1.0
#define DSP_UTILS_CLAMP(x) (((x) > (1.0)) ? (1.0) : \
    (((x) < (-1.0)) ? (-1.0) : (x)))

// clamp a value to 0.0 to 1.0
#define DSP_UTILS_CLAMP_POS(x) (((x) > (1.0)) ? (1.0) : \
    (((x) < (0.0)) ? (0.0) : (x)))

// process a mono levelmeter
#define DSP_UTILS_LEVELMETER_MONO(in, out, smoothing) \
do { \
    if((in) > (out)) { \
        (out) = (in); \
    } \
    else { \
        (out) *= (smoothing); \
    } \
} while(0)

//
// DC blocking filter
//
// in - input sample
// out - output sample - must be differen than in
// inhist - input history for next time
// outhist - output history for next time
//
#define DSP_UTILS_DC_BLOCK(in, out, inhist, outhist) ({ \
    (out) = (in) - (inhist) + (0.999 * (outhist)); \
    (inhist) = (in); \
    (outhist) = (out); \
})

//
// 1 pole filter
//
// set the coefficient for a single pole low pass filter
// cutoff - cutoff freq in Hz
// a0 - zero coeff
#define DSP_UTILS_FILTER1_SET_CUTOFF(cutoff, a0) ({ \
    (a0) = 1.0 - expf(-2.0 * M_PI * ((cutoff) / (float)AUDIO_FS)); \
})

// run the low pass for one sample
// in - input sample
// out - output sample - must be different than in
// a0 - zero coeff
// z1 - history reg - must be retained between runs
#define DSP_UTILS_FILTER1_RUN_LPF(in, out, a0, z1) ({ \
    (out) = (z1) = (((in) - (z1)) * (a0)) + (z1);  \
})

// run the high pass for one sample
// in - input sample
// out - output sample - must be different than in
// a0 - zero coeff
// z1 - history reg - must be retained between runs
#define DSP_UTILS_FILTER1_RUN_HPF(in, out, a0, z1) ({ \
    (out) = (z1) = (((in) - (z1)) * (a0)) + (z1);  \
    (out) = (in) - (out); \
})

//
// delay memory
//
// rotate the delay memory pointer
// delayp - the delay pointer to be updated
// delay_len - the delay len (must be a power of 2)
#define DSP_UTILS_ROTATE_DELAY(delayp, delay_len) ({ \
    (delayp) = ((delayp) - 1) & ((delay_len) - 1); \
})

// read a sample from a delay line - single sample
// delayp - the delay pointer to be updated
// delay_len - the delay len (must be a power of 2)
// addr - the address to read from
// out - the output var
#define DSP_UTILS_READ_DELAY(delay, delayp, delay_len, addr, out) ({ \
    (out) = delay[((delayp) + (addr)) & ((delay_len) - 1)]; \
})

// read a sample from the delay line - fraction sample interpolated
// make sure there are enough samples so you don't run off the end
// delayp - the delay pointer to be updated
// delay_len - the delay len (must be a power of 2)
// addr - the address to read from as a float - real = addr, fract = interp
// out - the output var
#define DSP_UTILS_READ_DELAY_FRACT(delay, delayp, delay_len, addr, out) ({ \
    (it1) = (addr) - (int)(addr); \
    (out) = delay[((delayp) + (int)(addr)) & ((delay_len) - 1)] * (1.0 - (it1)); \
    (out) += delay[((delayp) + ((int)(addr) + 1)) & ((delay_len) - 1)] * (it1); \
})

// write a sample to the delay line
// delayp - the delay pointer to be updated
// delay_len - the delay len (must be a power of 2)
// addr - the address to write to
// in - the input var
#define DSP_UTILS_WRITE_DELAY(delay, delayp, delay_len, addr, in) ({ \
    delay[((delayp) + (addr)) & ((delay_len) - 1)] = (in); \
})

// run a first-order all-pass filter
// delayp - the delay pointer to be updated
// delay_len - the delay len (must be a power of 2)
// inaddr - the address to write to
// outaddr - the address to read from
// g - AP feedback coeff - + = alternating sign, - = same sign
// acc - used for input and output
// it1 - internal temp var - does not need to be retained
#define DSP_UTILS_ALLPASS(delay, delayp, delay_len, inaddr, outaddr, g) ({ \
    (it1) = delay[((delayp) + (outaddr)) & ((delay_len) - 1)]; \
    (acc) += ((it1) * -(g)); \
    delay[((delayp) + (inaddr)) & ((delay_len) - 1)] = (acc); \
    (acc) = ((acc) * (g)) + (it1); \
})

// run a first-order all-pass filter with fractional delay read
// make sure there are enough samples so you don't run off the end
// delayp - the delay pointer to be updated
// delay_len - the delay len (must be a power of 2)
// inaddr - the address to write to
// outaddr - the address to read from as a float - real = addr, fract = interp
// g - AP feedback coeff
// acc - used for input and output
// it1 - internal temp var - does not need to be retained
// it2 - internal temp var - does not need to be retained
#define DSP_UTILS_ALLPASS_FRACT(delay, delayp, delay_len, inaddr, outaddr, g) ({ \
    (it2) = (outaddr) - (int)(outaddr); \
    (it1) = delay[((delayp) + (int)(outaddr)) & ((delay_len) - 1)] * (1.0 - (it2)); \
    (it1) += delay[((delayp) + ((int)(outaddr) + 1)) & ((delay_len) - 1)] * (it2); \
    (acc) += ((it1) * -(g)); \
    delay[((delayp) + (inaddr)) & ((delay_len) - 1)] = (acc); \
    (acc) = ((acc) * (g)) + (it1); \
})

// convert a factor to a dB value - 1.0 = 0dB (field size)
#define DSP_UTILS_FACTOR_TO_DB(x) ({ \
    20.0 * log10f(x + DSP_UTILS_VSA); \
})
