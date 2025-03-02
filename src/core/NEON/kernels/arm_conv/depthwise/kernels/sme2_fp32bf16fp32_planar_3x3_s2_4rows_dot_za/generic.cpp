/*
 * Copyright (c) 2022-2023 Arm Limited.
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

#if defined(ARM_COMPUTE_ENABLE_SME2)

#include <algorithm>
#include <cstddef>

namespace arm_conv {
namespace depthwise {

void sme2_fp32bf16fp32_planar_3x3_s2_4rows_dot_za_impl(
  const float *inptr,
  size_t ld_in_row,
  size_t ld_in_col,
  size_t ld_in_vl,
  unsigned int pad_top,
  unsigned int valid_input_rows,
  unsigned int pad_left,
  unsigned int valid_input_cols,
  const float *weights,
  const float *bias,
  float **outptrs,
  const size_t *outlds,
  const size_t *outvllds,
  unsigned int output_cols,
  unsigned int start_channel,
  unsigned int valid_channels,
  float act_min,
  float act_max
)
{
  struct Args
  {
    const float *inptr;
    size_t ld_in_vl;
    long unsigned int pad_top, pad_bottom, pad_left;
    const float *weights;
    const float *bias;
    long unsigned int input_cols, output_cols;
    float **outptrs;
    const size_t *ld_out_cols;
    const size_t *ld_out_vls;
    long unsigned int current_channel, n_channels;
    float clamp_min, clamp_max;
  };

  Args args = { inptr, ld_in_vl, pad_top, 9u - std::min(9u, pad_top + valid_input_rows), pad_left, weights, bias, valid_input_cols, output_cols, outptrs, outlds, outvllds, start_channel, valid_channels, act_min, act_max };

  __asm__ __volatile__(
    "ldr x6, [%x[args], %[offsetof_Args_pad_bottom]]\n"
    "mov x20, #0x9\n"
    ".inst 0xd503477f  // SMSTART ZA\n"
    "sub x20, x20, x6\n"
    "ldr x7, [%x[args], %[offsetof_Args_pad_top]]\n"
    "ptrue p2.b\n"
    "ld1rw { z27.s }, p2/Z, [%x[args], %[offsetof_Args_clamp_min]]\n"
    "ldr x17, [%x[args], %[offsetof_Args_n_channels]]\n"
    "whilelt p1.s, XZR, x17\n"
    "whilelt p9.s, XZR, x20\n"
    "ld1rw { z23.s }, p2/Z, [%x[args], %[offsetof_Args_clamp_max]]\n"
    "whilelt p8.s, XZR, x7\n"
    "eor p8.b, p2/Z, p8.b, p9.b\n"
    "ldr x16, [%x[args], %[offsetof_Args_current_channel]]\n"
    "1:"  // Channel loop
    "ldr x20, [%x[args], %[offsetof_Args_bias]]\n"
    "fmov z4.s, #0x0\n"
    "cbz x20, 2f\n"
    "ld1w { z4.s }, p1/Z, [x20, x16, LSL #2]\n"
    "2:"  // Load bias: Done
    "ldr x20, [%x[args], %[offsetof_Args_weights]]\n"
    "mov x21, x20\n"
    "ld1w { z19.s }, p2/Z, [x21]\n"
    "incb x21, ALL, MUL #3\n"
    "incb x20\n"
    "ld1w { z24.s }, p2/Z, [x21]\n"
    "incb x21, ALL, MUL #3\n"
    ".inst 0x658aaa69  // bfcvt z9.h, p2/M, z19.s\n"
    "ld1w { z12.s }, p2/Z, [x21]\n"
    "mov x21, x20\n"
    ".inst 0x648aab09  // bfcvtnt z9.h, p2/M, z24.s\n"
    "incb x20\n"
    "ld1w { z19.s }, p2/Z, [x21]\n"
    "incb x21, ALL, MUL #3\n"
    ".inst 0x658aa983  // bfcvt z3.h, p2/M, z12.s\n"
    ".inst 0x658aaa62  // bfcvt z2.h, p2/M, z19.s\n"
    "ld1w { z24.s }, p2/Z, [x21]\n"
    "incb x21, ALL, MUL #3\n"
    "ldr x15, [%x[args], %[offsetof_Args_input_cols]]\n"
    ".inst 0x648aab02  // bfcvtnt z2.h, p2/M, z24.s\n"
    "ld1w { z12.s }, p2/Z, [x21]\n"
    "mov x21, x20\n"
    ".inst 0x658aa980  // bfcvt z0.h, p2/M, z12.s\n"
    "ldr x14, [%x[args], %[offsetof_Args_inptr]]\n"
    "ld1w { z19.s }, p2/Z, [x21]\n"
    "incb x21, ALL, MUL #3\n"
    ".inst 0x658aaa6a  // bfcvt z10.h, p2/M, z19.s\n"
    "sub x20, x15, #0x1\n"
    "ld1w { z24.s }, p2/Z, [x21]\n"
    "incb x21, ALL, MUL #3\n"
    "orr x23, x20, %x[ld_in_col], LSL #18\n"
    "mov z5.d, z4.d\n"
    "ld1w { z12.s }, p2/Z, [x21]\n"
    "orr x23, x17, x23, LSL #20\n"
    "mov x22, #0x9\n"
    "mov z6.d, z4.d\n"
    "add x21, x7, x6\n"
    "lsl x20, %x[ld_in_row], #0x2\n"
    "mov z7.d, z4.d\n"
    ".inst 0x648aab0a  // bfcvtnt z10.h, p2/M, z24.s\n"
    ".inst 0x658aa981  // bfcvt z1.h, p2/M, z12.s\n"
    "mov x8, #0x0\n"
    "ldr x13, [%x[args], %[offsetof_Args_output_cols]]\n"
    "lsl x23, x23, #0x2\n"
    "sub x22, x22, x21\n"
    "madd x20, x20, x7, x14\n"
    "3:"  // Issue prefetches
    "subs x22, x22, #0x1\n"
    ".inst 0xf8b74a9c  // rprfm pldstrm, x23, [x20]\n"
    "add x20, x20, %x[ld_in_col], LSL #2\n"
    "bgt 3b\n"
    "ldr x11, [%x[args], %[offsetof_Args_outptrs]]\n"
    "lsl x20, %x[ld_in_row], #0x2\n"
    "msub x14, x7, x20, x14\n"
    ".inst 0xc0040c80  // mova za.d[x8, #0], { z4.d-z7.d }\n"
    "ldr x20, [%x[args], %[offsetof_Args_ld_out_cols]]\n"
    ".inst 0xc0040c81  // mova za.d[x8, #1], { z4.d-z7.d }\n"
    "mov x22, #0x2\n"
    "ldp x10, x9, [x11], #0x10\n"
    ".inst 0xc0040c82  // mova za.d[x8, #2], { z4.d-z7.d }\n"
    "ldp x28, x27, [x20], #0x10\n"
    "ldr x21, [%x[args], %[offsetof_Args_pad_left]]\n"
    "ldp x26, x25, [x11], #0x10\n"
    "ldp x24, x23, [x20], #0x10\n"
    "cbz x21, 5f\n"
    "cmp x21, x22\n"
    "csel x20, x21, x22, LT\n"
    "sub x21, x21, x20\n"
    "sub x22, x22, x20\n"
    "cbz x21, 5f\n"
    ".inst 0xc0060c1c  // mova { z28.d-z31.d }, za.d[x8, #0]\n"
    "and x22, x21, #0x1\n"
    "add x21, x21, #0x1\n"
    ".inst 0xc1b7cb7c  // fclamp { z28.s-z31.s }, z27.s, z23.s\n"
    "lsr x21, x21, #0x1\n"
    "sub x13, x13, x21\n"
    "4:"  // Left padding
    "subs x21, x21, #0x1\n"
    "st1w { z28.s }, p1, [x10]\n"
    "add x10, x10, x28, LSL #2\n"
    "st1w { z29.s }, p1, [x9]\n"
    "add x9, x9, x27, LSL #2\n"
    "st1w { z30.s }, p1, [x26]\n"
    "add x26, x26, x24, LSL #2\n"
    "st1w { z31.s }, p1, [x25]\n"
    "add x25, x25, x23, LSL #2\n"
    "bgt 4b\n"
    "5:"  // Left padding: End
    "adds XZR, x7, x6\n"
    "bne 10f\n"
    "cbz x22, 8f\n"
    "cmp x22, #0x1\n"
    "sub x15, x15, x22\n"
    "beq 7f\n"
    "6:"  // Unpadded: 2 priming loads
    "add x20, x14, %x[ld_in_row], LSL #2\n"
    "ld1w { z16.s }, p1/Z, [x14]\n"
    ".inst 0x658aaa0b  // bfcvt z11.h, p2/M, z16.s\n"
    "add x14, x14, %x[ld_in_col], LSL #2\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa0b  // bfcvtnt z11.h, p2/M, z16.s\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0c  // bfcvt z12.h, p2/M, z16.s\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa0c  // bfcvtnt z12.h, p2/M, z16.s\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0d  // bfcvt z13.h, p2/M, z16.s\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa0d  // bfcvtnt z13.h, p2/M, z16.s\n"
    "ld1w { z22.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaace  // bfcvt z14.h, p2/M, z22.s\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa0e  // bfcvtnt z14.h, p2/M, z16.s\n"
    ".inst 0xc1391170  // bfdot za.s[x8, 0], { z11.h-z14.h }, z9.h\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    ".inst 0x658aaa0f  // bfcvt z15.h, p2/M, z16.s\n"
    ".inst 0xc1331190  // bfdot za.s[x8, 0], { z12.h-z15.h }, z3.h\n"
    "7:"  // Unpadded: 1 priming loads
    "add x20, x14, %x[ld_in_row], LSL #2\n"
    "ld1w { z16.s }, p1/Z, [x14]\n"
    ".inst 0x658aaa0b  // bfcvt z11.h, p2/M, z16.s\n"
    "add x14, x14, %x[ld_in_col], LSL #2\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa0b  // bfcvtnt z11.h, p2/M, z16.s\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0c  // bfcvt z12.h, p2/M, z16.s\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa0c  // bfcvtnt z12.h, p2/M, z16.s\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0d  // bfcvt z13.h, p2/M, z16.s\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa0d  // bfcvtnt z13.h, p2/M, z16.s\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0e  // bfcvt z14.h, p2/M, z16.s\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa0e  // bfcvtnt z14.h, p2/M, z16.s\n"
    ".inst 0xc1321170  // bfdot za.s[x8, 0], { z11.h-z14.h }, z2.h\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    ".inst 0x658aaa0f  // bfcvt z15.h, p2/M, z16.s\n"
    ".inst 0xc1301190  // bfdot za.s[x8, 0], { z12.h-z15.h }, z0.h\n"
    "8:"  // Unpadded: 0 priming loads
    "cmp x15, #0x2\n"
    "blt 16f\n"
    "add x21, x14, %x[ld_in_row], LSL #2\n"
    "ld1w { z16.s }, p1/Z, [x14]\n"
    ".inst 0x658aaa0b  // bfcvt z11.h, p2/M, z16.s\n"
    "sub x15, x15, #0x2\n"
    "ld1w { z16.s }, p1/Z, [x21]\n"
    "add x21, x21, %x[ld_in_row], LSL #2\n"
    "sub x13, x13, #0x1\n"
    ".inst 0x648aaa0b  // bfcvtnt z11.h, p2/M, z16.s\n"
    "ld1w { z16.s }, p1/Z, [x21]\n"
    "add x21, x21, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0c  // bfcvt z12.h, p2/M, z16.s\n"
    "lsr x20, x15, #0x1\n"
    "ld1w { z16.s }, p1/Z, [x21]\n"
    "add x21, x21, %x[ld_in_row], LSL #2\n"
    "cmp x20, x13\n"
    ".inst 0x648aaa0c  // bfcvtnt z12.h, p2/M, z16.s\n"
    "ld1w { z16.s }, p1/Z, [x21]\n"
    "add x21, x21, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0d  // bfcvt z13.h, p2/M, z16.s\n"
    "csel x22, x20, x13, LT\n"
    "ld1w { z16.s }, p1/Z, [x21]\n"
    "add x21, x21, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa0d  // bfcvtnt z13.h, p2/M, z16.s\n"
    "add x14, x14, %x[ld_in_col], LSL #2\n"
    "ld1w { z16.s }, p1/Z, [x21]\n"
    "add x21, x21, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0e  // bfcvt z14.h, p2/M, z16.s\n"
    "and x15, x15, #0x1\n"
    "ld1w { z16.s }, p1/Z, [x21]\n"
    "add x21, x21, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa0e  // bfcvtnt z14.h, p2/M, z16.s\n"
    "sub x13, x13, x22\n"
    "ld1w { z16.s }, p1/Z, [x21]\n"
    ".inst 0x658aaa0f  // bfcvt z15.h, p2/M, z16.s\n"
    "cbz x22, 15f\n"
    "9:"  // Unpadded: Main loop
    "add x21, x14, %x[ld_in_row], LSL #2\n"
    "ld1w { z16.s }, p1/Z, [x14]\n"
    ".inst 0xc13a1170  // bfdot za.s[x8, 0], { z11.h-z14.h }, z10.h\n"
    "add x14, x14, %x[ld_in_col], LSL #2\n"
    "ld1w { z18.s }, p1/Z, [x21]\n"
    "add x21, x21, %x[ld_in_row], LSL #2\n"
    ".inst 0xc1391171  // bfdot za.s[x8, 1], { z11.h-z14.h }, z9.h\n"
    ".inst 0x658aaa0b  // bfcvt z11.h, p2/M, z16.s\n"
    "ld1w { z16.s }, p1/Z, [x21]\n"
    "add x21, x21, %x[ld_in_row], LSL #2\n"
    ".inst 0xc1311190  // bfdot za.s[x8, 0], { z12.h-z15.h }, z1.h\n"
    "add x20, x14, %x[ld_in_row], LSL #2\n"
    "ld1w { z17.s }, p1/Z, [x21]\n"
    "add x21, x21, %x[ld_in_row], LSL #2\n"
    ".inst 0xc1331191  // bfdot za.s[x8, 1], { z12.h-z15.h }, z3.h\n"
    ".inst 0x658aaa0c  // bfcvt z12.h, p2/M, z16.s\n"
    "ld1w { z16.s }, p1/Z, [x21]\n"
    "add x21, x21, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0d  // bfcvt z13.h, p2/M, z16.s\n"
    ".inst 0x648aaa4b  // bfcvtnt z11.h, p2/M, z18.s\n"
    "ld1w { z16.s }, p1/Z, [x21]\n"
    "add x21, x21, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa2c  // bfcvtnt z12.h, p2/M, z17.s\n"
    ".inst 0x648aaa0d  // bfcvtnt z13.h, p2/M, z16.s\n"
    "ld1w { z16.s }, p1/Z, [x21]\n"
    "add x21, x21, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0e  // bfcvt z14.h, p2/M, z16.s\n"
    ".inst 0xc0060c1c  // mova { z28.d-z31.d }, za.d[x8, #0]\n"
    "ld1w { z16.s }, p1/Z, [x21]\n"
    "add x21, x21, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa0e  // bfcvtnt z14.h, p2/M, z16.s\n"
    "add x8, x8, #0x1\n"
    "ld1w { z16.s }, p1/Z, [x14]\n"
    ".inst 0xc1321170  // bfdot za.s[x8, 0], { z11.h-z14.h }, z2.h\n"
    ".inst 0x658aaa0b  // bfcvt z11.h, p2/M, z16.s\n"
    "subs x22, x22, #0x1\n"
    "ld1w { z20.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0xc1b7cb7c  // fclamp { z28.s-z31.s }, z27.s, z23.s\n"
    "st1w { z28.s }, p1, [x10]\n"
    "ld1w { z16.s }, p1/Z, [x21]\n"
    ".inst 0x658aaa0f  // bfcvt z15.h, p2/M, z16.s\n"
    ".inst 0xc1301190  // bfdot za.s[x8, 0], { z12.h-z15.h }, z0.h\n"
    "add x10, x10, x28, LSL #2\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0c  // bfcvt z12.h, p2/M, z16.s\n"
    "st1w { z29.s }, p1, [x9]\n"
    "ld1w { z19.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    "add x9, x9, x27, LSL #2\n"
    "st1w { z30.s }, p1, [x26]\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0d  // bfcvt z13.h, p2/M, z16.s\n"
    "add x26, x26, x24, LSL #2\n"
    "ld1w { z18.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    "st1w { z31.s }, p1, [x25]\n"
    "add x25, x25, x23, LSL #2\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0e  // bfcvt z14.h, p2/M, z16.s\n"
    ".inst 0xc0040c82  // mova za.d[x8, #2], { z4.d-z7.d }\n"
    "ld1w { z17.s }, p1/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa8b  // bfcvtnt z11.h, p2/M, z20.s\n"
    ".inst 0x648aaa6c  // bfcvtnt z12.h, p2/M, z19.s\n"
    "ld1w { z16.s }, p1/Z, [x20]\n"
    ".inst 0x648aaa4d  // bfcvtnt z13.h, p2/M, z18.s\n"
    ".inst 0x648aaa2e  // bfcvtnt z14.h, p2/M, z17.s\n"
    "add x14, x14, %x[ld_in_col], LSL #2\n"
    ".inst 0x658aaa0f  // bfcvt z15.h, p2/M, z16.s\n"
    "bgt 9b\n"
    "b 15f\n"
    "10:"  // Padded
    "cbz x22, 13f\n"
    "cmp x22, #0x1\n"
    "sub x15, x15, x22\n"
    "beq 12f\n"
    "11:"  // Padded: 2 priming loads
    "mov x12, #0x0\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z16.s }, p0/Z, [x14]\n"
    ".inst 0x658aaa0b  // bfcvt z11.h, p2/M, z16.s\n"
    "add x20, x14, %x[ld_in_row], LSL #2\n"
    ".inst 0x25704500  // psel p0.s, p1.s/Z, p8.s[w12, #1]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x648aaa0b  // bfcvtnt z11.h, p2/M, z16.s\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25b04500  // psel p0.s, p1.s/Z, p8.s[w12, #2]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x658aaa0c  // bfcvt z12.h, p2/M, z16.s\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25f04500  // psel p0.s, p1.s/Z, p8.s[w12, #3]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x648aaa0c  // bfcvtnt z12.h, p2/M, z16.s\n"
    "mov x12, #0x4\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0d  // bfcvt z13.h, p2/M, z16.s\n"
    ".inst 0x25704500  // psel p0.s, p1.s/Z, p8.s[w12, #1]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa0d  // bfcvtnt z13.h, p2/M, z16.s\n"
    ".inst 0x25b04500  // psel p0.s, p1.s/Z, p8.s[w12, #2]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0e  // bfcvt z14.h, p2/M, z16.s\n"
    ".inst 0x25f04500  // psel p0.s, p1.s/Z, p8.s[w12, #3]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "mov x12, #0x8\n"
    ".inst 0x648aaa0e  // bfcvtnt z14.h, p2/M, z16.s\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x658aaa0f  // bfcvt z15.h, p2/M, z16.s\n"
    ".inst 0xc1391170  // bfdot za.s[x8, 0], { z11.h-z14.h }, z9.h\n"
    "add x14, x14, %x[ld_in_col], LSL #2\n"
    ".inst 0xc1331190  // bfdot za.s[x8, 0], { z12.h-z15.h }, z3.h\n"
    "12:"  // Padded: 1 priming loads
    "mov x12, #0x0\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z16.s }, p0/Z, [x14]\n"
    ".inst 0x658aaa0b  // bfcvt z11.h, p2/M, z16.s\n"
    "add x20, x14, %x[ld_in_row], LSL #2\n"
    ".inst 0x25704500  // psel p0.s, p1.s/Z, p8.s[w12, #1]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x648aaa0b  // bfcvtnt z11.h, p2/M, z16.s\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25b04500  // psel p0.s, p1.s/Z, p8.s[w12, #2]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x658aaa0c  // bfcvt z12.h, p2/M, z16.s\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25f04500  // psel p0.s, p1.s/Z, p8.s[w12, #3]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x648aaa0c  // bfcvtnt z12.h, p2/M, z16.s\n"
    "mov x12, #0x4\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0d  // bfcvt z13.h, p2/M, z16.s\n"
    ".inst 0x25704500  // psel p0.s, p1.s/Z, p8.s[w12, #1]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa0d  // bfcvtnt z13.h, p2/M, z16.s\n"
    ".inst 0x25b04500  // psel p0.s, p1.s/Z, p8.s[w12, #2]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0e  // bfcvt z14.h, p2/M, z16.s\n"
    ".inst 0x25f04500  // psel p0.s, p1.s/Z, p8.s[w12, #3]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "mov x12, #0x8\n"
    ".inst 0x648aaa0e  // bfcvtnt z14.h, p2/M, z16.s\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x658aaa0f  // bfcvt z15.h, p2/M, z16.s\n"
    ".inst 0xc1321170  // bfdot za.s[x8, 0], { z11.h-z14.h }, z2.h\n"
    "add x14, x14, %x[ld_in_col], LSL #2\n"
    ".inst 0xc1301190  // bfdot za.s[x8, 0], { z12.h-z15.h }, z0.h\n"
    "13:"  // Padded: 0 priming loads
    "cmp x15, #0x2\n"
    "blt 16f\n"
    "mov x12, #0x0\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z16.s }, p0/Z, [x14]\n"
    ".inst 0x658aaa0b  // bfcvt z11.h, p2/M, z16.s\n"
    "add x20, x14, %x[ld_in_row], LSL #2\n"
    ".inst 0x25704500  // psel p0.s, p1.s/Z, p8.s[w12, #1]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x648aaa0b  // bfcvtnt z11.h, p2/M, z16.s\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25b04500  // psel p0.s, p1.s/Z, p8.s[w12, #2]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x658aaa0c  // bfcvt z12.h, p2/M, z16.s\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25f04500  // psel p0.s, p1.s/Z, p8.s[w12, #3]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x648aaa0c  // bfcvtnt z12.h, p2/M, z16.s\n"
    "mov x12, #0x4\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0d  // bfcvt z13.h, p2/M, z16.s\n"
    ".inst 0x25704500  // psel p0.s, p1.s/Z, p8.s[w12, #1]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa0d  // bfcvtnt z13.h, p2/M, z16.s\n"
    ".inst 0x25b04500  // psel p0.s, p1.s/Z, p8.s[w12, #2]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0e  // bfcvt z14.h, p2/M, z16.s\n"
    ".inst 0x25f04500  // psel p0.s, p1.s/Z, p8.s[w12, #3]\n"
    "mov x12, #0x8\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x648aaa0e  // bfcvtnt z14.h, p2/M, z16.s\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x658aaa0f  // bfcvt z15.h, p2/M, z16.s\n"
    "sub x15, x15, #0x2\n"
    "sub x13, x13, #0x1\n"
    "lsr x20, x15, #0x1\n"
    "cmp x20, x13\n"
    "csel x21, x20, x13, LT\n"
    "add x14, x14, %x[ld_in_col], LSL #2\n"
    "and x15, x15, #0x1\n"
    "sub x13, x13, x21\n"
    "cbz x21, 15f\n"
    "14:"  // Padded: Main loop
    "mov x12, #0x0\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z18.s }, p0/Z, [x14]\n"
    ".inst 0xc13a1170  // bfdot za.s[x8, 0], { z11.h-z14.h }, z10.h\n"
    "add x20, x14, %x[ld_in_row], LSL #2\n"
    ".inst 0x25704500  // psel p0.s, p1.s/Z, p8.s[w12, #1]\n"
    "ld1w { z19.s }, p0/Z, [x20]\n"
    ".inst 0xc1391171  // bfdot za.s[x8, 1], { z11.h-z14.h }, z9.h\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25b04500  // psel p0.s, p1.s/Z, p8.s[w12, #2]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0xc1311190  // bfdot za.s[x8, 0], { z12.h-z15.h }, z1.h\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25f04500  // psel p0.s, p1.s/Z, p8.s[w12, #3]\n"
    "ld1w { z17.s }, p0/Z, [x20]\n"
    ".inst 0xc1331191  // bfdot za.s[x8, 1], { z12.h-z15.h }, z3.h\n"
    "mov x12, #0x4\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa4b  // bfcvt z11.h, p2/M, z18.s\n"
    ".inst 0x658aaa0c  // bfcvt z12.h, p2/M, z16.s\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0d  // bfcvt z13.h, p2/M, z16.s\n"
    ".inst 0x25704500  // psel p0.s, p1.s/Z, p8.s[w12, #1]\n"
    "ld1w { z18.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa6b  // bfcvtnt z11.h, p2/M, z19.s\n"
    ".inst 0x25b04500  // psel p0.s, p1.s/Z, p8.s[w12, #2]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0e  // bfcvt z14.h, p2/M, z16.s\n"
    ".inst 0x25f04500  // psel p0.s, p1.s/Z, p8.s[w12, #3]\n"
    "mov x12, #0x8\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x648aaa2c  // bfcvtnt z12.h, p2/M, z17.s\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z17.s }, p0/Z, [x20]\n"
    ".inst 0x648aaa4d  // bfcvtnt z13.h, p2/M, z18.s\n"
    "mov x12, #0x0\n"
    "add x14, x14, %x[ld_in_col], LSL #2\n"
    ".inst 0x648aaa0e  // bfcvtnt z14.h, p2/M, z16.s\n"
    ".inst 0xc0060c1c  // mova { z28.d-z31.d }, za.d[x8, #0]\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z16.s }, p0/Z, [x14]\n"
    "add x20, x14, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa2f  // bfcvt z15.h, p2/M, z17.s\n"
    ".inst 0x25704500  // psel p0.s, p1.s/Z, p8.s[w12, #1]\n"
    "ld1w { z21.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0xc1b7cb7c  // fclamp { z28.s-z31.s }, z27.s, z23.s\n"
    ".inst 0x25b04500  // psel p0.s, p1.s/Z, p8.s[w12, #2]\n"
    "ld1w { z17.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    "st1w { z28.s }, p1, [x10]\n"
    ".inst 0x25f04500  // psel p0.s, p1.s/Z, p8.s[w12, #3]\n"
    "mov x12, #0x4\n"
    "ld1w { z20.s }, p0/Z, [x20]\n"
    "st1w { z29.s }, p1, [x9]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z19.s }, p0/Z, [x20]\n"
    "st1w { z30.s }, p1, [x26]\n"
    "add x8, x8, #0x1\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0xc1321170  // bfdot za.s[x8, 0], { z11.h-z14.h }, z2.h\n"
    ".inst 0x658aaa0b  // bfcvt z11.h, p2/M, z16.s\n"
    ".inst 0x25704500  // psel p0.s, p1.s/Z, p8.s[w12, #1]\n"
    "ld1w { z18.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0xc1301190  // bfdot za.s[x8, 0], { z12.h-z15.h }, z0.h\n"
    ".inst 0x25b04500  // psel p0.s, p1.s/Z, p8.s[w12, #2]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa2c  // bfcvt z12.h, p2/M, z17.s\n"
    ".inst 0x25f04500  // psel p0.s, p1.s/Z, p8.s[w12, #3]\n"
    "mov x12, #0x8\n"
    "ld1w { z17.s }, p0/Z, [x20]\n"
    ".inst 0x658aaa6d  // bfcvt z13.h, p2/M, z19.s\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    ".inst 0x658aaa0e  // bfcvt z14.h, p2/M, z16.s\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "subs x21, x21, #0x1\n"
    "add x10, x10, x28, LSL #2\n"
    "st1w { z31.s }, p1, [x25]\n"
    ".inst 0xc0040c82  // mova za.d[x8, #2], { z4.d-z7.d }\n"
    "add x9, x9, x27, LSL #2\n"
    "add x26, x26, x24, LSL #2\n"
    ".inst 0x648aaaab  // bfcvtnt z11.h, p2/M, z21.s\n"
    ".inst 0x648aaa8c  // bfcvtnt z12.h, p2/M, z20.s\n"
    "add x25, x25, x23, LSL #2\n"
    ".inst 0x648aaa4d  // bfcvtnt z13.h, p2/M, z18.s\n"
    ".inst 0x648aaa2e  // bfcvtnt z14.h, p2/M, z17.s\n"
    "add x14, x14, %x[ld_in_col], LSL #2\n"
    ".inst 0x658aaa0f  // bfcvt z15.h, p2/M, z16.s\n"
    "bgt 14b\n"
    "15:"  // Main loop tail
    "mov x12, #0x0\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z17.s }, p0/Z, [x14]\n"
    ".inst 0xc13a1170  // bfdot za.s[x8, 0], { z11.h-z14.h }, z10.h\n"
    "add x20, x14, %x[ld_in_row], LSL #2\n"
    ".inst 0x25704500  // psel p0.s, p1.s/Z, p8.s[w12, #1]\n"
    "ld1w { z19.s }, p0/Z, [x20]\n"
    ".inst 0xc1391171  // bfdot za.s[x8, 1], { z11.h-z14.h }, z9.h\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25b04500  // psel p0.s, p1.s/Z, p8.s[w12, #2]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0xc1311190  // bfdot za.s[x8, 0], { z12.h-z15.h }, z1.h\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25f04500  // psel p0.s, p1.s/Z, p8.s[w12, #3]\n"
    "ld1w { z18.s }, p0/Z, [x20]\n"
    ".inst 0xc1331191  // bfdot za.s[x8, 1], { z12.h-z15.h }, z3.h\n"
    "mov x12, #0x4\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa2b  // bfcvt z11.h, p2/M, z17.s\n"
    ".inst 0x658aaa0c  // bfcvt z12.h, p2/M, z16.s\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0d  // bfcvt z13.h, p2/M, z16.s\n"
    ".inst 0x25704500  // psel p0.s, p1.s/Z, p8.s[w12, #1]\n"
    "ld1w { z17.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa6b  // bfcvtnt z11.h, p2/M, z19.s\n"
    ".inst 0x25b04500  // psel p0.s, p1.s/Z, p8.s[w12, #2]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0e  // bfcvt z14.h, p2/M, z16.s\n"
    ".inst 0x25f04500  // psel p0.s, p1.s/Z, p8.s[w12, #3]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "mov x12, #0x8\n"
    ".inst 0x648aaa4c  // bfcvtnt z12.h, p2/M, z18.s\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    ".inst 0x648aaa2d  // bfcvtnt z13.h, p2/M, z17.s\n"
    ".inst 0x648aaa0e  // bfcvtnt z14.h, p2/M, z16.s\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0xc0060c1c  // mova { z28.d-z31.d }, za.d[x8, #0]\n"
    "add x8, x8, #0x1\n"
    ".inst 0x658aaa0f  // bfcvt z15.h, p2/M, z16.s\n"
    ".inst 0xc1321170  // bfdot za.s[x8, 0], { z11.h-z14.h }, z2.h\n"
    ".inst 0xc1b7cb7c  // fclamp { z28.s-z31.s }, z27.s, z23.s\n"
    "st1w { z28.s }, p1, [x10]\n"
    "add x10, x10, x28, LSL #2\n"
    "st1w { z29.s }, p1, [x9]\n"
    "add x9, x9, x27, LSL #2\n"
    ".inst 0xc0040c82  // mova za.d[x8, #2], { z4.d-z7.d }\n"
    "add x14, x14, %x[ld_in_col], LSL #2\n"
    "st1w { z30.s }, p1, [x26]\n"
    "add x26, x26, x24, LSL #2\n"
    ".inst 0xc1301190  // bfdot za.s[x8, 0], { z12.h-z15.h }, z0.h\n"
    "st1w { z31.s }, p1, [x25]\n"
    "add x25, x25, x23, LSL #2\n"
    "16:"  // Main loop skip tail
    "cbz x15, 17f\n"  // Skip remainder inputs
    "mov x12, #0x0\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z16.s }, p0/Z, [x14]\n"
    ".inst 0x658aaa0b  // bfcvt z11.h, p2/M, z16.s\n"
    "add x20, x14, %x[ld_in_row], LSL #2\n"
    ".inst 0x25704500  // psel p0.s, p1.s/Z, p8.s[w12, #1]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x648aaa0b  // bfcvtnt z11.h, p2/M, z16.s\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25b04500  // psel p0.s, p1.s/Z, p8.s[w12, #2]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x658aaa0c  // bfcvt z12.h, p2/M, z16.s\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25f04500  // psel p0.s, p1.s/Z, p8.s[w12, #3]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x648aaa0c  // bfcvtnt z12.h, p2/M, z16.s\n"
    "mov x12, #0x4\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0d  // bfcvt z13.h, p2/M, z16.s\n"
    ".inst 0x25704500  // psel p0.s, p1.s/Z, p8.s[w12, #1]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x648aaa0d  // bfcvtnt z13.h, p2/M, z16.s\n"
    ".inst 0x25b04500  // psel p0.s, p1.s/Z, p8.s[w12, #2]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x658aaa0e  // bfcvt z14.h, p2/M, z16.s\n"
    ".inst 0x25f04500  // psel p0.s, p1.s/Z, p8.s[w12, #3]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    "mov x12, #0x8\n"
    ".inst 0x648aaa0e  // bfcvtnt z14.h, p2/M, z16.s\n"
    "add x20, x20, %x[ld_in_row], LSL #2\n"
    ".inst 0x25304500  // psel p0.s, p1.s/Z, p8.s[w12]\n"
    "ld1w { z16.s }, p0/Z, [x20]\n"
    ".inst 0x658aaa0f  // bfcvt z15.h, p2/M, z16.s\n"
    ".inst 0xc13a1170  // bfdot za.s[x8, 0], { z11.h-z14.h }, z10.h\n"
    "sub x13, x13, #0x1\n"
    ".inst 0xc1311190  // bfdot za.s[x8, 0], { z12.h-z15.h }, z1.h\n"
    ".inst 0xc1391171  // bfdot za.s[x8, 1], { z11.h-z14.h }, z9.h\n"
    ".inst 0xc0060c1c  // mova { z28.d-z31.d }, za.d[x8, #0]\n"
    ".inst 0xc1b7cb7c  // fclamp { z28.s-z31.s }, z27.s, z23.s\n"
    "st1w { z28.s }, p1, [x10]\n"
    "add x10, x10, x28, LSL #2\n"
    ".inst 0xc1331191  // bfdot za.s[x8, 1], { z12.h-z15.h }, z3.h\n"
    "add x8, x8, #0x1\n"
    "st1w { z29.s }, p1, [x9]\n"
    "add x9, x9, x27, LSL #2\n"
    "st1w { z30.s }, p1, [x26]\n"
    "add x26, x26, x24, LSL #2\n"
    ".inst 0xc0040c82  // mova za.d[x8, #2], { z4.d-z7.d }\n"
    "st1w { z31.s }, p1, [x25]\n"
    "add x25, x25, x23, LSL #2\n"
    "17:"  // Tail input: End
    "cbz x13, 19f\n"
    "18:"  // Right padding loop
    ".inst 0xc0060c1c  // mova { z28.d-z31.d }, za.d[x8, #0]\n"
    "add x8, x8, #0x1\n"
    "subs x13, x13, #0x1\n"
    ".inst 0xc1b7cb7c  // fclamp { z28.s-z31.s }, z27.s, z23.s\n"
    "st1w { z28.s }, p1, [x10]\n"
    "add x10, x10, x28, LSL #2\n"
    ".inst 0xc0040c82  // mova za.d[x8, #2], { z4.d-z7.d }\n"
    "st1w { z29.s }, p1, [x9]\n"
    "add x9, x9, x27, LSL #2\n"
    "st1w { z30.s }, p1, [x26]\n"
    "add x26, x26, x24, LSL #2\n"
    "st1w { z31.s }, p1, [x25]\n"
    "add x25, x25, x23, LSL #2\n"
    "bgt 18b\n"
    "19:"  // End
    "ldr x20, [%x[args], %[offsetof_Args_weights]]\n"
    "incb x20, ALL, MUL #9\n"
    "str x20, [%x[args], %[offsetof_Args_weights]]\n"
    "incw x16\n"
    "ldr x20, [%x[args], %[offsetof_Args_ld_in_vl]]\n"
    "whilelt p1.s, x16, x17\n"
    "ldr x14, [%x[args], %[offsetof_Args_inptr]]\n"
    "add x14, x14, x20, LSL #2\n"
    "str x14, [%x[args], %[offsetof_Args_inptr]]\n"
    "ldr x11, [%x[args], %[offsetof_Args_outptrs]]\n"
    "ldr x24, [%x[args], %[offsetof_Args_ld_out_vls]]\n"
    "ldp x23, x22, [x11, #0x0]\n"
    "ldp x21, x20, [x24, #0x0]\n"
    "add x23, x23, x21, LSL #2\n"
    "add x22, x22, x20, LSL #2\n"
    "stp x23, x22, [x11, #0x0]\n"
    "ldp x23, x22, [x11, #0x10]\n"
    "ldp x21, x20, [x24, #0x10]\n"
    "add x23, x23, x21, LSL #2\n"
    "add x22, x22, x20, LSL #2\n"
    "stp x23, x22, [x11, #0x10]\n"
    "b.any 1b\n"
    ".inst 0xd503467f  // SMSTOP\n"
    :
    : [args] "r" (&args), [ld_in_col] "r" (ld_in_col), [ld_in_row] "r" (ld_in_row), [offsetof_Args_bias] "I" (offsetof(Args, bias)), [offsetof_Args_clamp_max] "I" (offsetof(Args, clamp_max)), [offsetof_Args_clamp_min] "I" (offsetof(Args, clamp_min)), [offsetof_Args_current_channel] "I" (offsetof(Args, current_channel)), [offsetof_Args_inptr] "I" (offsetof(Args, inptr)), [offsetof_Args_input_cols] "I" (offsetof(Args, input_cols)), [offsetof_Args_ld_in_vl] "I" (offsetof(Args, ld_in_vl)), [offsetof_Args_ld_out_cols] "I" (offsetof(Args, ld_out_cols)), [offsetof_Args_ld_out_vls] "I" (offsetof(Args, ld_out_vls)), [offsetof_Args_n_channels] "I" (offsetof(Args, n_channels)), [offsetof_Args_outptrs] "I" (offsetof(Args, outptrs)), [offsetof_Args_output_cols] "I" (offsetof(Args, output_cols)), [offsetof_Args_pad_bottom] "I" (offsetof(Args, pad_bottom)), [offsetof_Args_pad_left] "I" (offsetof(Args, pad_left)), [offsetof_Args_pad_top] "I" (offsetof(Args, pad_top)), [offsetof_Args_weights] "I" (offsetof(Args, weights))
    : "cc", "memory", "p0", "p1", "p2", "p3", "p4", "p5", "p6", "p7", "p8", "p9", "p10", "p11", "p12", "p13", "p14", "p15", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "z0", "z1", "z2", "z3", "z4", "z5", "z6", "z7", "z8", "z9", "z10", "z11", "z12", "z13", "z14", "z15", "z16", "z17", "z18", "z19", "z20", "z21", "z22", "z23", "z24", "z25", "z26", "z27", "z28", "z29", "z30", "z31"
  );
}

}  // namespace depthwise
}  // namespace arm_conv

#endif  // defined(ARM_COMPUTE_ENABLE_SME2)
