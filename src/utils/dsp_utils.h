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

#define DSP_UTILS_ABS(x) ((x)<0 ? -(x) : (x))

#define DSP_UTILS_CLAMP(x) (((x) > (1.0)) ? (1.0) : \
    (((x) < (-1.0)) ? (-1.0) : (x)))

#define DSP_UTILS_CLAMP_POS(x) (((x) > (1.0)) ? (1.0) : \
    (((x) < (0.0)) ? (0.0) : (x)))

#define DSP_UTILS_LMM(in, out, sm) \
do { \
    if((in) > (out)) { \
        (out) = (in); \
    } \
    else { \
        (out) *= (sm); \
    } \
} while(0)

#define DSP_UTILS_DCB(in, out, ihst, ohst) ({ \
    (out) = (in) - (ihst) + (0.999 * (ohst)); \
    (ihst) = (in); \
    (ohst) = (out); \
})

#define DSP_UTILS_F1SC(f, a0) ({ \
    (a0) = 1.0 - expf(-2.0 * M_PI * ((f) / (float)AUDIO_FS)); \
})

#define DSP_UTILS_F1LP(in, out, a0, z1) ({ \
    (out) = (z1) = (((in) - (z1)) * (a0)) + (z1);  \
})

#define DSP_UTILS_F1HP(in, out, a0, z1) ({ \
    (out) = (z1) = (((in) - (z1)) * (a0)) + (z1);  \
    (out) = (in) - (out); \
})

#define DSP_UTILS_DROT(dp, dlen) ({ \
    (dp) = ((dp) - 1) & ((dlen) - 1); \
})

#define DSP_UTILS_DREAD(delay, dp, dlen, addr, out) ({ \
    (out) = delay[((dp) + (addr)) & ((dlen) - 1)]; \
})

#define DSP_UTILS_DREADF(delay, dp, dlen, addr, out) ({ \
    (it1) = (addr) - (int)(addr); \
    (out) = delay[((dp) + (int)(addr)) & ((dlen) - 1)] * (1.0 - (it1)); \
    (out) += delay[((dp) + ((int)(addr) + 1)) & ((dlen) - 1)] * (it1); \
})

#define DSP_UTILS_DWRITE(delay, dp, dlen, addr, in) ({ \
    delay[((dp) + (addr)) & ((dlen) - 1)] = (in); \
})

#define DSP_UTILS_AP(delay, dp, dlen, inaddr, outaddr, g) ({ \
    (it1) = delay[((dp) + (outaddr)) & ((dlen) - 1)]; \
    (acc) += ((it1) * -(g)); \
    delay[((dp) + (inaddr)) & ((dlen) - 1)] = (acc); \
    (acc) = ((acc) * (g)) + (it1); \
})

#define DSP_UTILS_APF(delay, dp, dlen, inaddr, outaddr, g) ({ \
    (it2) = (outaddr) - (int)(outaddr); \
    (it1) = delay[((dp) + (int)(outaddr)) & ((dlen) - 1)] * (1.0 - (it2)); \
    (it1) += delay[((dp) + ((int)(outaddr) + 1)) & ((dlen) - 1)] * (it2); \
    (acc) += ((it1) * -(g)); \
    delay[((dp) + (inaddr)) & ((dlen) - 1)] = (acc); \
    (acc) = ((acc) * (g)) + (it1); \
})

#define DSP_UTILS_F2DB(x) ({ \
    20.0 * log10f(x + DSP_UTILS_VSA); \
})
