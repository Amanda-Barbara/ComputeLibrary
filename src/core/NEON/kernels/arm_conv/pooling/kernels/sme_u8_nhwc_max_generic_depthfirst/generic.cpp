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

#if defined(ARM_COMPUTE_ENABLE_SME)

#include <cstdint>

namespace arm_conv {
namespace pooling {


void sme_u8_nhwc_max_generic_depthfirst_impl(
  const uint64_t,
  const uint64_t n_valid_cells,
  uint64_t n_channels,
  const uint8_t *const *const inptrs,
  uint8_t *outptr
)
{
  __asm__ __volatile__(
    ".inst 0xd503477f  // SMSTART ZA\n"
    "mov x9, #0x0\n"
    "cntb x28\n"
    "cntb x27, ALL, MUL #2\n"
    "cntb x26, ALL, MUL #3\n"
    "whilelt p4.b, x9, %x[n_channels]\n"
    "whilelt p3.b, x28, %x[n_channels]\n"
    "whilelt p2.b, x27, %x[n_channels]\n"
    "whilelt p1.b, x26, %x[n_channels]\n"
    "ptrue p0.b\n"
    "b.none 7f\n"
    "1:"  // 4-vectors of channels
    "lsr x25, %x[n_valid_cells], #0x2\n"
    "mov z4.b, #0x0\n"
    "mov z3.b, #0x0\n"
    "mov x20, %x[inptrs]\n"
    "mov z2.b, #0x0\n"
    "mov z1.b, #0x0\n"
    "cbz x25, 4f\n"
    "ldp x24, x23, [x20, #0x0]\n"
    "subs x25, x25, #0x1\n"
    "ld1b { z0.b }, p4/Z, [x24, x9]\n"
    "ldp x22, x21, [x20, #0x10]\n"
    "add x20, x20, #0x20\n"
    "ld1b { z31.b }, p4/Z, [x23, x9]\n"
    "ld1b { z23.b }, p4/Z, [x22, x9]\n"
    "ld1b { z30.b }, p4/Z, [x21, x9]\n"
    "ld1b { z18.b }, p3/Z, [x24, x28]\n"
    "ld1b { z29.b }, p3/Z, [x23, x28]\n"
    "ld1b { z22.b }, p3/Z, [x22, x28]\n"
    "ld1b { z28.b }, p3/Z, [x21, x28]\n"
    "ld1b { z17.b }, p2/Z, [x24, x27]\n"
    "ld1b { z27.b }, p2/Z, [x23, x27]\n"
    "ld1b { z21.b }, p2/Z, [x22, x27]\n"
    "ld1b { z26.b }, p2/Z, [x21, x27]\n"
    "ld1b { z16.b }, p1/Z, [x24, x26]\n"
    "ld1b { z25.b }, p1/Z, [x23, x26]\n"
    "ld1b { z20.b }, p1/Z, [x22, x26]\n"
    "ld1b { z24.b }, p1/Z, [x21, x26]\n"
    "beq 3f\n"
    "2:"  // 4-vectors of channels: 4 inputs loop
    "movprfx z19, z0\n umax z19.b, p0/M, z19.b, z31.b\n"
    "umax z23.b, p0/M, z23.b, z30.b\n"
    "ldp x24, x23, [x20, #0x0]\n"
    "subs x25, x25, #0x1\n"
    "umax z18.b, p0/M, z18.b, z29.b\n"
    "umax z22.b, p0/M, z22.b, z28.b\n"
    "ldp x22, x21, [x20, #0x10]\n"
    "add x20, x20, #0x20\n"
    "umax z17.b, p0/M, z17.b, z27.b\n"
    "umax z21.b, p0/M, z21.b, z26.b\n"
    "ld1b { z0.b }, p4/Z, [x24, x9]\n"
    "umax z16.b, p0/M, z16.b, z25.b\n"
    "umax z20.b, p0/M, z20.b, z24.b\n"
    "ld1b { z31.b }, p4/Z, [x23, x9]\n"
    "umax z19.b, p0/M, z19.b, z23.b\n"
    "umax z18.b, p0/M, z18.b, z22.b\n"
    "ld1b { z23.b }, p4/Z, [x22, x9]\n"
    "umax z17.b, p0/M, z17.b, z21.b\n"
    "umax z16.b, p0/M, z16.b, z20.b\n"
    "ld1b { z30.b }, p4/Z, [x21, x9]\n"
    "umax z4.b, p0/M, z4.b, z19.b\n"
    "umax z3.b, p0/M, z3.b, z18.b\n"
    "ld1b { z18.b }, p3/Z, [x24, x28]\n"
    "umax z2.b, p0/M, z2.b, z17.b\n"
    "umax z1.b, p0/M, z1.b, z16.b\n"
    "ld1b { z29.b }, p3/Z, [x23, x28]\n"
    "ld1b { z22.b }, p3/Z, [x22, x28]\n"
    "ld1b { z28.b }, p3/Z, [x21, x28]\n"
    "ld1b { z17.b }, p2/Z, [x24, x27]\n"
    "ld1b { z27.b }, p2/Z, [x23, x27]\n"
    "ld1b { z21.b }, p2/Z, [x22, x27]\n"
    "ld1b { z26.b }, p2/Z, [x21, x27]\n"
    "ld1b { z16.b }, p1/Z, [x24, x26]\n"
    "ld1b { z25.b }, p1/Z, [x23, x26]\n"
    "ld1b { z20.b }, p1/Z, [x22, x26]\n"
    "ld1b { z24.b }, p1/Z, [x21, x26]\n"
    "bgt 2b\n"
    "3:"  // 4-vectors of channels: 4 inputs tail
    "movprfx z19, z0\n umax z19.b, p0/M, z19.b, z31.b\n"
    "umax z23.b, p0/M, z23.b, z30.b\n"
    "umax z18.b, p0/M, z18.b, z29.b\n"
    "umax z22.b, p0/M, z22.b, z28.b\n"
    "umax z17.b, p0/M, z17.b, z27.b\n"
    "umax z21.b, p0/M, z21.b, z26.b\n"
    "umax z16.b, p0/M, z16.b, z25.b\n"
    "umax z20.b, p0/M, z20.b, z24.b\n"
    "umax z19.b, p0/M, z19.b, z23.b\n"
    "umax z18.b, p0/M, z18.b, z22.b\n"
    "umax z17.b, p0/M, z17.b, z21.b\n"
    "umax z16.b, p0/M, z16.b, z20.b\n"
    "umax z4.b, p0/M, z4.b, z19.b\n"
    "umax z3.b, p0/M, z3.b, z18.b\n"
    "umax z2.b, p0/M, z2.b, z17.b\n"
    "umax z1.b, p0/M, z1.b, z16.b\n"
    "4:"  // 4-vectors of channels: After loop
    "ands x21, %x[n_valid_cells], #0x3\n"
    "beq 6f\n"
    "5:"  // 4-vectors of channels: Single input loop
    "ldr x24, [x20], #0x8\n"
    "ld1b { z0.b }, p4/Z, [x24, x9]\n"
    "subs x21, x21, #0x1\n"
    "umax z4.b, p0/M, z4.b, z0.b\n"
    "ld1b { z18.b }, p3/Z, [x24, x28]\n"
    "umax z3.b, p0/M, z3.b, z18.b\n"
    "ld1b { z17.b }, p2/Z, [x24, x27]\n"
    "umax z2.b, p0/M, z2.b, z17.b\n"
    "ld1b { z16.b }, p1/Z, [x24, x26]\n"
    "umax z1.b, p0/M, z1.b, z16.b\n"
    "bgt 5b\n"
    "6:"  // 4-vectors of channels: Single input loop: End
    "st1b { z4.b }, p4, [%x[outptr], x9]\n"
    "incb x9, ALL, MUL #4\n"
    "st1b { z3.b }, p3, [%x[outptr], x28]\n"
    "incb x28, ALL, MUL #4\n"
    "st1b { z2.b }, p2, [%x[outptr], x27]\n"
    "incb x27, ALL, MUL #4\n"
    "st1b { z1.b }, p1, [%x[outptr], x26]\n"
    "incb x26, ALL, MUL #4\n"
    "whilelt p1.b, x26, %x[n_channels]\n"
    "b.any 1b\n"
    "7:"  // Single vector of channels
    "whilelt p4.b, x9, %x[n_channels]\n"
    "b.none 14f\n"
    "8:"  // Single vector of channels: Loop
    "lsr x25, %x[n_valid_cells], #0x2\n"
    "mov z4.b, #0x0\n"
    "mov x20, %x[inptrs]\n"
    "cbz x25, 11f\n"
    "ldp x24, x23, [x20, #0x0]\n"
    "subs x25, x25, #0x1\n"
    "ld1b { z0.b }, p4/Z, [x24, x9]\n"
    "ldp x22, x21, [x20, #0x10]\n"
    "add x20, x20, #0x20\n"
    "ld1b { z31.b }, p4/Z, [x23, x9]\n"
    "ld1b { z23.b }, p4/Z, [x22, x9]\n"
    "ld1b { z30.b }, p4/Z, [x21, x9]\n"
    "beq 10f\n"
    "9:"  // Single vector of channels: Loop: 4 inputs loop
    "movprfx z19, z0\n umax z19.b, p0/M, z19.b, z31.b\n"
    "umax z23.b, p0/M, z23.b, z30.b\n"
    "ldp x24, x23, [x20, #0x0]\n"
    "subs x25, x25, #0x1\n"
    "umax z19.b, p0/M, z19.b, z23.b\n"
    "ldp x22, x21, [x20, #0x10]\n"
    "umax z4.b, p0/M, z4.b, z19.b\n"
    "add x20, x20, #0x20\n"
    "ld1b { z0.b }, p4/Z, [x24, x9]\n"
    "ld1b { z31.b }, p4/Z, [x23, x9]\n"
    "ld1b { z23.b }, p4/Z, [x22, x9]\n"
    "ld1b { z30.b }, p4/Z, [x21, x9]\n"
    "bgt 9b\n"
    "10:"  // Single vector of channels: Loop: 4 inputs tail
    "movprfx z19, z0\n umax z19.b, p0/M, z19.b, z31.b\n"
    "umax z23.b, p0/M, z23.b, z30.b\n"
    "umax z19.b, p0/M, z19.b, z23.b\n"
    "umax z4.b, p0/M, z4.b, z19.b\n"
    "11:"  // Single vector of channels: Loop: After loop
    "ands x21, %x[n_valid_cells], #0x3\n"
    "beq 13f\n"
    "12:"  // Single vector of channels: Loop: Single input loop
    "ldr x24, [x20], #0x8\n"
    "ld1b { z0.b }, p4/Z, [x24, x9]\n"
    "subs x21, x21, #0x1\n"
    "umax z4.b, p0/M, z4.b, z0.b\n"
    "bgt 12b\n"
    "13:"  // Single vector of channels: Loop: Single input loop: End
    "st1b { z4.b }, p4, [%x[outptr], x9]\n"
    "incb x9\n"
    "whilelt p4.b, x9, %x[n_channels]\n"
    "b.any 8b\n"
    "14:"  // End
    ".inst 0xd503467f  // SMSTOP\n"
    :
    : [inptrs] "r" (inptrs), [n_channels] "r" (n_channels), [n_valid_cells] "r" (n_valid_cells), [outptr] "r" (outptr)
    : "cc", "memory", "p0", "p1", "p2", "p3", "p4", "p5", "p6", "p7", "p8", "p9", "p10", "p11", "p12", "p13", "p14", "p15", "x9", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "z0", "z1", "z2", "z3", "z4", "z5", "z6", "z7", "z8", "z9", "z10", "z11", "z12", "z13", "z14", "z15", "z16", "z17", "z18", "z19", "z20", "z21", "z22", "z23", "z24", "z25", "z26", "z27", "z28", "z29", "z30", "z31"
  );
}

}  // namespace pooling
}  // namespace arm_conv

#endif  // defined(ARM_COMPUTE_ENABLE_SME)
