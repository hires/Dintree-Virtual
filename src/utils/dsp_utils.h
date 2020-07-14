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

// convert a factor to a dB value - 1.0 = 0dB (field size)
#define DSP_UTILS_FACTOR_TO_DB(x) ({ \
    20.0 * log10f(x + DSP_UTILS_VSA); \
})
