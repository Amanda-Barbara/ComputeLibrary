/*
 * Copyright (c) 2021, 2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <cstddef>
#include <cstdint>

#if defined(ARM_COMPUTE_ENABLE_SVE)

namespace arm_conv {
namespace depthwise {

void sve_fp32_packed_to_nhwc_5x5_s1_with_multiplier_output2x4_mla_depthfirst_impl(
  const float *const *const inptrs,
  float *const *const outptrs,
  const void *params,
  const unsigned int n_output_channels,
  const float activation_min,
  const float activation_max
)
{
  const float minmax_vals[2] = { activation_min, activation_max };

  __asm__ __volatile__(
    "mov x15, #0x0\n"
    "whilelt p2.s, x15, %x[channel_multiplier]\n"
    "ldr x14, [%x[inptrs], #0x0]\n"
    "ldr x13, [%x[inptrs], #0x8]\n"
    "ptrue p1.b\n"
    "ldr x12, [%x[inptrs], #0x10]\n"
    "ldr x11, [%x[inptrs], #0x18]\n"
    "mov x10, #0x0\n"
    "ldr x9, [%x[inptrs], #0x20]\n"
    "ldr x28, [%x[inptrs], #0x28]\n"
    "ld1w { z25.s }, p2/Z, [%x[params]]\n"
    "ldp x27, x26, [%x[outptrs], #0x0]\n"
    "mov z24.d, z25.d\n"
    "mov z23.d, z25.d\n"
    "ldp x25, x24, [%x[outptrs], #0x10]\n"
    "ldp x23, x22, [%x[outptrs], #0x20]\n"
    "mov z22.d, z25.d\n"
    "mov z21.d, z25.d\n"
    "ldp x21, x20, [%x[outptrs], #0x30]\n"
    "ld1rqw { z2.s }, p1/Z, [x14]\n"
    "mov z20.d, z25.d\n"
    "mov z19.d, z25.d\n"
    "ld1rqw { z3.s }, p1/Z, [x14, #16]\n"
    "ld1rqw { z4.s }, p1/Z, [x13]\n"
    "mov z18.d, z25.d\n"
    "ld1rqw { z5.s }, p1/Z, [x13, #16]\n"
    "ld1rqw { z6.s }, p1/Z, [x12]\n"
    "ld1rqw { z7.s }, p1/Z, [x12, #16]\n"
    "ld1rqw { z8.s }, p1/Z, [x11]\n"
    "ld1rqw { z9.s }, p1/Z, [x11, #16]\n"
    "ld1rqw { z10.s }, p1/Z, [x9]\n"
    "ld1rqw { z11.s }, p1/Z, [x9, #16]\n"
    "ld1rqw { z12.s }, p1/Z, [x28]\n"
    "ld1rqw { z13.s }, p1/Z, [x28, #16]\n"
    "ld1rw { z17.s }, p1/Z, [%x[clamps]]\n"
    "ld1rw { z16.s }, p1/Z, [%x[clamps], #4]\n"
    "ld1w { z31.s }, p2/Z, [%x[params], #1, MUL VL]\n"
    "ld1w { z30.s }, p2/Z, [%x[params], #2, MUL VL]\n"
    "ld1w { z29.s }, p2/Z, [%x[params], #3, MUL VL]\n"
    "ld1w { z28.s }, p2/Z, [%x[params], #4, MUL VL]\n"
    "ld1w { z27.s }, p2/Z, [%x[params], #5, MUL VL]\n"
    "addvl %x[params], %x[params], #6\n"
    "1:"  // Output channel complete vector loop
    "fmla z25.s, z31.s, z2.s[0]\n"
    "fmla z24.s, z31.s, z2.s[1]\n"
    "mov z0.d, z8.d\n"
    "incw x15\n"
    "fmla z23.s, z31.s, z2.s[2]\n"
    "fmla z22.s, z31.s, z2.s[3]\n"
    "mov z1.d, z9.d\n"
    "mov p0.b, p2.b\n"
    "fmla z21.s, z31.s, z4.s[0]\n"
    "fmla z20.s, z31.s, z4.s[1]\n"
    "whilelt p2.s, x15, %x[channel_multiplier]\n"
    "fmla z19.s, z31.s, z4.s[2]\n"
    "fmla z18.s, z31.s, z4.s[3]\n"
    "ld1w { z31.s }, p1/Z, [%x[params]]\n"
    "fmla z25.s, z30.s, z2.s[1]\n"
    "fmla z24.s, z30.s, z2.s[2]\n"
    "fmla z23.s, z30.s, z2.s[3]\n"
    "fmla z22.s, z30.s, z3.s[0]\n"
    "fmla z21.s, z30.s, z4.s[1]\n"
    "fmla z20.s, z30.s, z4.s[2]\n"
    "fmla z19.s, z30.s, z4.s[3]\n"
    "fmla z18.s, z30.s, z5.s[0]\n"
    "ld1w { z30.s }, p1/Z, [%x[params], #1, MUL VL]\n"
    "fmla z25.s, z29.s, z2.s[2]\n"
    "fmla z24.s, z29.s, z2.s[3]\n"
    "fmla z23.s, z29.s, z3.s[0]\n"
    "fmla z22.s, z29.s, z3.s[1]\n"
    "fmla z21.s, z29.s, z4.s[2]\n"
    "fmla z20.s, z29.s, z4.s[3]\n"
    "fmla z19.s, z29.s, z5.s[0]\n"
    "fmla z18.s, z29.s, z5.s[1]\n"
    "ld1w { z29.s }, p1/Z, [%x[params], #2, MUL VL]\n"
    "fmla z25.s, z28.s, z2.s[3]\n"
    "fmla z24.s, z28.s, z3.s[0]\n"
    "fmla z23.s, z28.s, z3.s[1]\n"
    "fmla z22.s, z28.s, z3.s[2]\n"
    "fmla z21.s, z28.s, z4.s[3]\n"
    "fmla z20.s, z28.s, z5.s[0]\n"
    "fmla z19.s, z28.s, z5.s[1]\n"
    "fmla z18.s, z28.s, z5.s[2]\n"
    "ld1w { z28.s }, p1/Z, [%x[params], #3, MUL VL]\n"
    "fmla z25.s, z27.s, z3.s[0]\n"
    "fmla z24.s, z27.s, z3.s[1]\n"
    "fmla z23.s, z27.s, z3.s[2]\n"
    "fmla z22.s, z27.s, z3.s[3]\n"
    "fmla z21.s, z27.s, z5.s[0]\n"
    "fmla z20.s, z27.s, z5.s[1]\n"
    "fmla z19.s, z27.s, z5.s[2]\n"
    "fmla z18.s, z27.s, z5.s[3]\n"
    "ld1w { z27.s }, p1/Z, [%x[params], #4, MUL VL]\n"
    "fmla z25.s, z31.s, z4.s[0]\n"
    "fmla z24.s, z31.s, z4.s[1]\n"
    "fmla z23.s, z31.s, z4.s[2]\n"
    "fmla z22.s, z31.s, z4.s[3]\n"
    "fmla z21.s, z31.s, z6.s[0]\n"
    "fmla z20.s, z31.s, z6.s[1]\n"
    "fmla z19.s, z31.s, z6.s[2]\n"
    "fmla z18.s, z31.s, z6.s[3]\n"
    "ld1w { z31.s }, p1/Z, [%x[params], #5, MUL VL]\n"
    "fmla z25.s, z30.s, z4.s[1]\n"
    "fmla z24.s, z30.s, z4.s[2]\n"
    "fmla z23.s, z30.s, z4.s[3]\n"
    "fmla z22.s, z30.s, z5.s[0]\n"
    "fmla z21.s, z30.s, z6.s[1]\n"
    "fmla z20.s, z30.s, z6.s[2]\n"
    "fmla z19.s, z30.s, z6.s[3]\n"
    "fmla z18.s, z30.s, z7.s[0]\n"
    "ld1w { z30.s }, p1/Z, [%x[params], #6, MUL VL]\n"
    "fmla z25.s, z29.s, z4.s[2]\n"
    "fmla z24.s, z29.s, z4.s[3]\n"
    "fmla z23.s, z29.s, z5.s[0]\n"
    "fmla z22.s, z29.s, z5.s[1]\n"
    "fmla z21.s, z29.s, z6.s[2]\n"
    "fmla z20.s, z29.s, z6.s[3]\n"
    "fmla z19.s, z29.s, z7.s[0]\n"
    "fmla z18.s, z29.s, z7.s[1]\n"
    "ld1w { z29.s }, p1/Z, [%x[params], #7, MUL VL]\n"
    "addvl %x[params], %x[params], #16\n"
    "fmla z25.s, z28.s, z4.s[3]\n"
    "fmla z24.s, z28.s, z5.s[0]\n"
    "fmla z23.s, z28.s, z5.s[1]\n"
    "fmla z22.s, z28.s, z5.s[2]\n"
    "fmla z21.s, z28.s, z6.s[3]\n"
    "fmla z20.s, z28.s, z7.s[0]\n"
    "fmla z19.s, z28.s, z7.s[1]\n"
    "fmla z18.s, z28.s, z7.s[2]\n"
    "ld1w { z28.s }, p1/Z, [%x[params], #-8, MUL VL]\n"
    "fmla z25.s, z27.s, z5.s[0]\n"
    "fmla z24.s, z27.s, z5.s[1]\n"
    "fmla z23.s, z27.s, z5.s[2]\n"
    "fmla z22.s, z27.s, z5.s[3]\n"
    "fmla z21.s, z27.s, z7.s[0]\n"
    "fmla z20.s, z27.s, z7.s[1]\n"
    "fmla z19.s, z27.s, z7.s[2]\n"
    "fmla z18.s, z27.s, z7.s[3]\n"
    "ld1w { z27.s }, p1/Z, [%x[params], #-7, MUL VL]\n"
    "fmla z25.s, z31.s, z6.s[0]\n"
    "fmla z24.s, z31.s, z6.s[1]\n"
    "fmla z23.s, z31.s, z6.s[2]\n"
    "fmla z22.s, z31.s, z6.s[3]\n"
    "fmla z21.s, z31.s, z0.s[0]\n"
    "fmla z20.s, z31.s, z0.s[1]\n"
    "fmla z19.s, z31.s, z0.s[2]\n"
    "fmla z18.s, z31.s, z0.s[3]\n"
    "ld1w { z31.s }, p1/Z, [%x[params], #-6, MUL VL]\n"
    "fmla z25.s, z30.s, z6.s[1]\n"
    "fmla z24.s, z30.s, z6.s[2]\n"
    "fmla z23.s, z30.s, z6.s[3]\n"
    "fmla z22.s, z30.s, z7.s[0]\n"
    "fmla z21.s, z30.s, z0.s[1]\n"
    "fmla z20.s, z30.s, z0.s[2]\n"
    "fmla z19.s, z30.s, z0.s[3]\n"
    "fmla z18.s, z30.s, z1.s[0]\n"
    "ld1w { z30.s }, p1/Z, [%x[params], #-5, MUL VL]\n"
    "fmla z25.s, z29.s, z6.s[2]\n"
    "fmla z24.s, z29.s, z6.s[3]\n"
    "fmla z23.s, z29.s, z7.s[0]\n"
    "fmla z22.s, z29.s, z7.s[1]\n"
    "fmla z21.s, z29.s, z0.s[2]\n"
    "fmla z20.s, z29.s, z0.s[3]\n"
    "fmla z19.s, z29.s, z1.s[0]\n"
    "fmla z18.s, z29.s, z1.s[1]\n"
    "ld1w { z29.s }, p1/Z, [%x[params], #-4, MUL VL]\n"
    "fmla z25.s, z28.s, z6.s[3]\n"
    "fmla z24.s, z28.s, z7.s[0]\n"
    "fmla z23.s, z28.s, z7.s[1]\n"
    "fmla z22.s, z28.s, z7.s[2]\n"
    "fmla z21.s, z28.s, z0.s[3]\n"
    "fmla z20.s, z28.s, z1.s[0]\n"
    "fmla z19.s, z28.s, z1.s[1]\n"
    "fmla z18.s, z28.s, z1.s[2]\n"
    "ld1w { z28.s }, p1/Z, [%x[params], #-3, MUL VL]\n"
    "fmla z25.s, z27.s, z7.s[0]\n"
    "fmla z24.s, z27.s, z7.s[1]\n"
    "fmla z23.s, z27.s, z7.s[2]\n"
    "fmla z22.s, z27.s, z7.s[3]\n"
    "fmla z21.s, z27.s, z1.s[0]\n"
    "fmla z20.s, z27.s, z1.s[1]\n"
    "fmla z19.s, z27.s, z1.s[2]\n"
    "fmla z18.s, z27.s, z1.s[3]\n"
    "ld1w { z27.s }, p1/Z, [%x[params], #-2, MUL VL]\n"
    "fmla z25.s, z31.s, z0.s[0]\n"
    "fmla z24.s, z31.s, z0.s[1]\n"
    "fmla z23.s, z31.s, z0.s[2]\n"
    "fmla z22.s, z31.s, z0.s[3]\n"
    "mov z0.d, z10.d\n"
    "fmla z21.s, z31.s, z0.s[0]\n"
    "fmla z20.s, z31.s, z0.s[1]\n"
    "fmla z19.s, z31.s, z0.s[2]\n"
    "fmla z18.s, z31.s, z0.s[3]\n"
    "mov z0.d, z8.d\n"
    "ld1w { z31.s }, p1/Z, [%x[params], #-1, MUL VL]\n"
    "fmla z25.s, z30.s, z0.s[1]\n"
    "fmla z24.s, z30.s, z0.s[2]\n"
    "fmla z23.s, z30.s, z0.s[3]\n"
    "fmla z22.s, z30.s, z1.s[0]\n"
    "mov z0.d, z10.d\n"
    "mov z1.d, z11.d\n"
    "fmla z21.s, z30.s, z0.s[1]\n"
    "fmla z20.s, z30.s, z0.s[2]\n"
    "fmla z19.s, z30.s, z0.s[3]\n"
    "fmla z18.s, z30.s, z1.s[0]\n"
    "mov z0.d, z8.d\n"
    "ld1w { z30.s }, p1/Z, [%x[params]]\n"
    "mov z1.d, z9.d\n"
    "fmla z25.s, z29.s, z0.s[2]\n"
    "fmla z24.s, z29.s, z0.s[3]\n"
    "fmla z23.s, z29.s, z1.s[0]\n"
    "fmla z22.s, z29.s, z1.s[1]\n"
    "mov z0.d, z10.d\n"
    "mov z1.d, z11.d\n"
    "fmla z21.s, z29.s, z0.s[2]\n"
    "fmla z20.s, z29.s, z0.s[3]\n"
    "fmla z19.s, z29.s, z1.s[0]\n"
    "fmla z18.s, z29.s, z1.s[1]\n"
    "mov z0.d, z8.d\n"
    "ld1w { z29.s }, p1/Z, [%x[params], #1, MUL VL]\n"
    "mov z1.d, z9.d\n"
    "fmla z25.s, z28.s, z0.s[3]\n"
    "fmla z24.s, z28.s, z1.s[0]\n"
    "fmla z23.s, z28.s, z1.s[1]\n"
    "fmla z22.s, z28.s, z1.s[2]\n"
    "mov z0.d, z10.d\n"
    "mov z1.d, z11.d\n"
    "fmla z21.s, z28.s, z0.s[3]\n"
    "fmla z20.s, z28.s, z1.s[0]\n"
    "fmla z19.s, z28.s, z1.s[1]\n"
    "fmla z18.s, z28.s, z1.s[2]\n"
    "mov z1.d, z9.d\n"
    "ld1w { z28.s }, p1/Z, [%x[params], #2, MUL VL]\n"
    "fmla z25.s, z27.s, z1.s[0]\n"
    "fmla z24.s, z27.s, z1.s[1]\n"
    "fmla z23.s, z27.s, z1.s[2]\n"
    "fmla z22.s, z27.s, z1.s[3]\n"
    "mov z1.d, z11.d\n"
    "fmla z21.s, z27.s, z1.s[0]\n"
    "fmla z20.s, z27.s, z1.s[1]\n"
    "fmla z19.s, z27.s, z1.s[2]\n"
    "fmla z18.s, z27.s, z1.s[3]\n"
    "ld1w { z27.s }, p1/Z, [%x[params], #3, MUL VL]\n"
    "fmla z25.s, z31.s, z0.s[0]\n"
    "fmla z24.s, z31.s, z0.s[1]\n"
    "fmla z23.s, z31.s, z0.s[2]\n"
    "fmla z22.s, z31.s, z0.s[3]\n"
    "mov z0.d, z12.d\n"
    "fmla z21.s, z31.s, z0.s[0]\n"
    "fmla z20.s, z31.s, z0.s[1]\n"
    "fmla z19.s, z31.s, z0.s[2]\n"
    "fmla z18.s, z31.s, z0.s[3]\n"
    "mov z0.d, z10.d\n"
    "ld1w { z31.s }, p2/Z, [%x[params], #5, MUL VL]\n"
    "fmla z25.s, z30.s, z0.s[1]\n"
    "fmla z24.s, z30.s, z0.s[2]\n"
    "fmla z23.s, z30.s, z0.s[3]\n"
    "fmla z22.s, z30.s, z1.s[0]\n"
    "mov z0.d, z12.d\n"
    "mov z1.d, z13.d\n"
    "fmla z21.s, z30.s, z0.s[1]\n"
    "fmla z20.s, z30.s, z0.s[2]\n"
    "fmla z19.s, z30.s, z0.s[3]\n"
    "fmla z18.s, z30.s, z1.s[0]\n"
    "mov z0.d, z10.d\n"
    "ld1w { z30.s }, p2/Z, [%x[params], #6, MUL VL]\n"
    "mov z1.d, z11.d\n"
    "fmla z25.s, z29.s, z0.s[2]\n"
    "fmla z24.s, z29.s, z0.s[3]\n"
    "fmla z23.s, z29.s, z1.s[0]\n"
    "fmla z22.s, z29.s, z1.s[1]\n"
    "mov z0.d, z12.d\n"
    "mov z1.d, z13.d\n"
    "fmla z21.s, z29.s, z0.s[2]\n"
    "fmla z20.s, z29.s, z0.s[3]\n"
    "fmla z19.s, z29.s, z1.s[0]\n"
    "fmla z18.s, z29.s, z1.s[1]\n"
    "mov z0.d, z10.d\n"
    "ld1w { z29.s }, p2/Z, [%x[params], #7, MUL VL]\n"
    "mov z1.d, z11.d\n"
    "fmla z25.s, z28.s, z0.s[3]\n"
    "fmla z24.s, z28.s, z1.s[0]\n"
    "fmla z23.s, z28.s, z1.s[1]\n"
    "fmla z22.s, z28.s, z1.s[2]\n"
    "mov z1.d, z13.d\n"
    "mov z0.d, z12.d\n"
    "fmla z20.s, z28.s, z1.s[0]\n"
    "fmla z19.s, z28.s, z1.s[1]\n"
    "fmla z18.s, z28.s, z1.s[2]\n"
    "mov z1.d, z11.d\n"
    "fmla z21.s, z28.s, z0.s[3]\n"
    "fmla z25.s, z27.s, z1.s[0]\n"
    "fmla z24.s, z27.s, z1.s[1]\n"
    "fmin z25.s, p1/M, z25.s, z16.s\n"
    "fmax z25.s, p1/M, z25.s, z17.s\n"
    "fmla z23.s, z27.s, z1.s[2]\n"
    "fmla z22.s, z27.s, z1.s[3]\n"
    "mov z1.d, z13.d\n"
    "fmin z24.s, p1/M, z24.s, z16.s\n"
    "fmla z21.s, z27.s, z1.s[0]\n"
    "fmla z20.s, z27.s, z1.s[1]\n"
    "fmin z23.s, p1/M, z23.s, z16.s\n"
    "fmin z22.s, p1/M, z22.s, z16.s\n"
    "fmla z19.s, z27.s, z1.s[2]\n"
    "fmla z18.s, z27.s, z1.s[3]\n"
    "fmin z21.s, p1/M, z21.s, z16.s\n"
    "fmin z20.s, p1/M, z20.s, z16.s\n"
    "fmin z19.s, p1/M, z19.s, z16.s\n"
    "fmin z18.s, p1/M, z18.s, z16.s\n"
    "st1w { z25.s }, p0, [x27, x10, LSL #2]\n"
    "ld1w { z25.s }, p2/Z, [%x[params], #4, MUL VL]\n"
    "addvl %x[params], %x[params], #16\n"
    "fmax z24.s, p1/M, z24.s, z17.s\n"
    "st1w { z24.s }, p0, [x26, x10, LSL #2]\n"
    "mov z24.d, z25.d\n"
    "fmax z23.s, p1/M, z23.s, z17.s\n"
    "fmax z22.s, p1/M, z22.s, z17.s\n"
    "st1w { z23.s }, p0, [x25, x10, LSL #2]\n"
    "mov z23.d, z25.d\n"
    "fmax z21.s, p1/M, z21.s, z17.s\n"
    "fmax z20.s, p1/M, z20.s, z17.s\n"
    "st1w { z22.s }, p0, [x24, x10, LSL #2]\n"
    "mov z22.d, z25.d\n"
    "fmax z19.s, p1/M, z19.s, z17.s\n"
    "fmax z18.s, p1/M, z18.s, z17.s\n"
    "st1w { z21.s }, p0, [x23, x10, LSL #2]\n"
    "mov z21.d, z25.d\n"
    "st1w { z20.s }, p0, [x22, x10, LSL #2]\n"
    "mov z20.d, z25.d\n"
    "ld1w { z28.s }, p2/Z, [%x[params], #-8, MUL VL]\n"
    "ld1w { z27.s }, p2/Z, [%x[params], #-7, MUL VL]\n"
    "st1w { z19.s }, p0, [x21, x10, LSL #2]\n"
    "mov z19.d, z25.d\n"
    "addvl %x[params], %x[params], #-6\n"
    "st1w { z18.s }, p0, [x20, x10, LSL #2]\n"
    "incw x10\n"
    "mov z18.d, z25.d\n"
    "b.any 1b\n"
    : [params] "+&r" (params)
    : [channel_multiplier] "r" (n_output_channels), [clamps] "r" (minmax_vals), [inptrs] "r" (inptrs), [outptrs] "r" (outptrs)
    : "cc", "memory", "p0", "p1", "p2", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "z0", "z1", "z2", "z3", "z4", "z5", "z6", "z7", "z8", "z9", "z10", "z11", "z12", "z13", "z16", "z17", "z18", "z19", "z20", "z21", "z22", "z23", "z24", "z25", "z27", "z28", "z29", "z30", "z31"
  );
}

}  // namespace depthwise
}  // namespace arm_conv

#endif  // defined(ARM_COMPUTE_ENABLE_SVE)
