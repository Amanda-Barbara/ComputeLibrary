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
#ifdef ARM_COMPUTE_ENABLE_SVE

#include <cstddef>

namespace arm_gemm {

void sve_ffinterleaved_fp16_mla_8x3VL_a64fx(
    const __fp16 *Apanel,
    const __fp16 *Bpanel,
    size_t B_stride,
    __fp16 *Cpanel,
    int ablocks,
    size_t N,
    int K) {

    struct KernelArgs {
        size_t K = {};
        const __fp16 *Bpanel = {};
        size_t N = {};
        size_t B_stride = {};
        const __fp16 *cur_B_ptr = {};
    } ka;

    ka.K = (K/1) - 1;
    ka.Bpanel = Bpanel;
    ka.N = N;
    ka.B_stride = B_stride;

    __asm__ __volatile__(
      "ptrue p0.b\n"
      "1:"  // Height loop
      "ldr x26, [%x[args_ptr], %[offsetof_Bpanel]]\n"
      "ldr x25, [%x[args_ptr], %[offsetof_N]]\n"
      "str x26, [%x[args_ptr], %[offsetof_cur_B_ptr]]\n"
      "mov x24, %x[Apanel]\n"
      "2:"  // Width loop
      "ldr x26, [%x[args_ptr], %[offsetof_cur_B_ptr]]\n"
      "ldr x20, [%x[args_ptr], %[offsetof_B_stride]]\n"
      "cnth x23, ALL, MUL #2\n"
      "add x22, x26, x20, LSL #1\n"
      "add x21, x22, x20, LSL #1\n"
      "add x20, x21, x20, LSL #1\n"
      "cmp x25, x23\n"
      "str x20, [%x[args_ptr], %[offsetof_cur_B_ptr]]\n"
      "mov %x[Apanel], x24\n"
      "bgt 3f\n"
      "dech x23\n"
      "cmp x25, x23\n"
      "mov x21, x26\n"
      "bgt 3f\n"
      "mov x22, x26\n"
      "3:"  // B setup done
      "ldr x20, [%x[args_ptr], %[offsetof_K]]\n"
      "cmp x20, #0x2\n"
      "mov z8.b, #0x0\n"
      "mov z9.b, #0x0\n"
      "mov z10.b, #0x0\n"
      "ld1h { z0.h }, p0/Z, [x26]\n"
      "mov z11.b, #0x0\n"
      "mov z12.b, #0x0\n"
      "ld1h { z1.h }, p0/Z, [x22]\n"
      "mov z13.b, #0x0\n"
      "mov z14.b, #0x0\n"
      "ld1h { z2.h }, p0/Z, [x21]\n"
      "mov z15.b, #0x0\n"
      "mov z16.b, #0x0\n"
      "ld1rh { z3.h }, p0/Z, [%x[Apanel]]\n"
      "mov z17.b, #0x0\n"
      "mov z18.b, #0x0\n"
      "ld1rh { z4.h }, p0/Z, [%x[Apanel], #2]\n"
      "mov z19.b, #0x0\n"
      "mov z20.b, #0x0\n"
      "ld1rh { z5.h }, p0/Z, [%x[Apanel], #4]\n"
      "mov z21.b, #0x0\n"
      "mov z22.b, #0x0\n"
      "ld1rh { z6.h }, p0/Z, [%x[Apanel], #6]\n"
      "mov z23.b, #0x0\n"
      "mov z24.b, #0x0\n"
      "mov z25.b, #0x0\n"
      "mov z26.b, #0x0\n"
      "mov z27.b, #0x0\n"
      "mov z28.b, #0x0\n"
      "mov z29.b, #0x0\n"
      "mov z30.b, #0x0\n"
      "mov z31.b, #0x0\n"
      "blt 5f\n"
      "4:"  // main loop head
      "fmla z8.h, p0/M, z0.h, z3.h\n"
      "fmla z9.h, p0/M, z1.h, z3.h\n"
      "sub x20, x20, #0x2\n"
      "fmla z10.h, p0/M, z2.h, z3.h\n"
      "ld1rh { z3.h }, p0/Z, [%x[Apanel], #8]\n"
      "fmla z11.h, p0/M, z0.h, z4.h\n"
      "fmla z12.h, p0/M, z1.h, z4.h\n"
      "fmla z13.h, p0/M, z2.h, z4.h\n"
      "ld1rh { z4.h }, p0/Z, [%x[Apanel], #10]\n"
      "fmla z14.h, p0/M, z0.h, z5.h\n"
      "fmla z15.h, p0/M, z1.h, z5.h\n"
      "cmp x20, #0x2\n"
      "fmla z16.h, p0/M, z2.h, z5.h\n"
      "ld1rh { z5.h }, p0/Z, [%x[Apanel], #12]\n"
      "fmla z17.h, p0/M, z0.h, z6.h\n"
      "fmla z18.h, p0/M, z1.h, z6.h\n"
      "fmla z19.h, p0/M, z2.h, z6.h\n"
      "ld1rh { z6.h }, p0/Z, [%x[Apanel], #14]\n"
      "fmla z20.h, p0/M, z0.h, z3.h\n"
      "fmla z21.h, p0/M, z1.h, z3.h\n"
      "fmla z22.h, p0/M, z2.h, z3.h\n"
      "ld1rh { z3.h }, p0/Z, [%x[Apanel], #16]\n"
      "fmla z23.h, p0/M, z0.h, z4.h\n"
      "fmla z24.h, p0/M, z1.h, z4.h\n"
      "fmla z25.h, p0/M, z2.h, z4.h\n"
      "ld1rh { z4.h }, p0/Z, [%x[Apanel], #18]\n"
      "fmla z26.h, p0/M, z0.h, z5.h\n"
      "fmla z27.h, p0/M, z1.h, z5.h\n"
      "fmla z28.h, p0/M, z2.h, z5.h\n"
      "ld1rh { z5.h }, p0/Z, [%x[Apanel], #20]\n"
      "fmla z29.h, p0/M, z0.h, z6.h\n"
      "ld1h { z0.h }, p0/Z, [x26, #1, MUL VL]\n"
      "fmla z30.h, p0/M, z1.h, z6.h\n"
      "fmla z31.h, p0/M, z2.h, z6.h\n"
      "ld1h { z1.h }, p0/Z, [x22, #1, MUL VL]\n"
      "ld1h { z2.h }, p0/Z, [x21, #1, MUL VL]\n"
      "fmla z8.h, p0/M, z0.h, z3.h\n"
      "ld1rh { z6.h }, p0/Z, [%x[Apanel], #22]\n"
      "fmla z9.h, p0/M, z1.h, z3.h\n"
      "fmla z10.h, p0/M, z2.h, z3.h\n"
      "fmla z11.h, p0/M, z0.h, z4.h\n"
      "ld1rh { z3.h }, p0/Z, [%x[Apanel], #24]\n"
      "fmla z12.h, p0/M, z1.h, z4.h\n"
      "fmla z13.h, p0/M, z2.h, z4.h\n"
      "ld1rh { z4.h }, p0/Z, [%x[Apanel], #26]\n"
      "fmla z14.h, p0/M, z0.h, z5.h\n"
      "fmla z15.h, p0/M, z1.h, z5.h\n"
      "addvl x26, x26, #2\n"
      "fmla z16.h, p0/M, z2.h, z5.h\n"
      "ld1rh { z5.h }, p0/Z, [%x[Apanel], #28]\n"
      "fmla z17.h, p0/M, z0.h, z6.h\n"
      "fmla z18.h, p0/M, z1.h, z6.h\n"
      "fmla z19.h, p0/M, z2.h, z6.h\n"
      "ld1rh { z6.h }, p0/Z, [%x[Apanel], #30]\n"
      "addvl x22, x22, #2\n"
      "addvl x21, x21, #2\n"
      "add %x[Apanel], %x[Apanel], #0x20\n"
      "fmla z20.h, p0/M, z0.h, z3.h\n"
      "fmla z21.h, p0/M, z1.h, z3.h\n"
      "fmla z22.h, p0/M, z2.h, z3.h\n"
      "ld1rh { z3.h }, p0/Z, [%x[Apanel]]\n"
      "fmla z23.h, p0/M, z0.h, z4.h\n"
      "fmla z24.h, p0/M, z1.h, z4.h\n"
      "fmla z25.h, p0/M, z2.h, z4.h\n"
      "fmla z26.h, p0/M, z0.h, z5.h\n"
      "ld1rh { z4.h }, p0/Z, [%x[Apanel], #2]\n"
      "fmla z27.h, p0/M, z1.h, z5.h\n"
      "fmla z28.h, p0/M, z2.h, z5.h\n"
      "ld1rh { z5.h }, p0/Z, [%x[Apanel], #4]\n"
      "fmla z29.h, p0/M, z0.h, z6.h\n"
      "ld1h { z0.h }, p0/Z, [x26]\n"
      "fmla z30.h, p0/M, z1.h, z6.h\n"
      "fmla z31.h, p0/M, z2.h, z6.h\n"
      "ld1h { z1.h }, p0/Z, [x22]\n"
      "ld1h { z2.h }, p0/Z, [x21]\n"
      "ld1rh { z6.h }, p0/Z, [%x[Apanel], #6]\n"
      "bge 4b\n"
      "5:"  // main loop skip
      "fmla z8.h, p0/M, z0.h, z3.h\n"
      "fmla z9.h, p0/M, z1.h, z3.h\n"
      "addvl x26, x26, #1\n"
      "fmla z10.h, p0/M, z2.h, z3.h\n"
      "ld1rh { z3.h }, p0/Z, [%x[Apanel], #8]\n"
      "fmla z11.h, p0/M, z0.h, z4.h\n"
      "fmla z12.h, p0/M, z1.h, z4.h\n"
      "fmla z13.h, p0/M, z2.h, z4.h\n"
      "ld1rh { z4.h }, p0/Z, [%x[Apanel], #10]\n"
      "fmla z14.h, p0/M, z0.h, z5.h\n"
      "fmla z15.h, p0/M, z1.h, z5.h\n"
      "addvl x22, x22, #1\n"
      "fmla z16.h, p0/M, z2.h, z5.h\n"
      "ld1rh { z5.h }, p0/Z, [%x[Apanel], #12]\n"
      "fmla z17.h, p0/M, z0.h, z6.h\n"
      "fmla z18.h, p0/M, z1.h, z6.h\n"
      "fmla z19.h, p0/M, z2.h, z6.h\n"
      "ld1rh { z6.h }, p0/Z, [%x[Apanel], #14]\n"
      "fmla z20.h, p0/M, z0.h, z3.h\n"
      "fmla z21.h, p0/M, z1.h, z3.h\n"
      "addvl x21, x21, #1\n"
      "fmla z22.h, p0/M, z2.h, z3.h\n"
      "fmla z23.h, p0/M, z0.h, z4.h\n"
      "add %x[Apanel], %x[Apanel], #0x10\n"
      "fmla z24.h, p0/M, z1.h, z4.h\n"
      "fmla z25.h, p0/M, z2.h, z4.h\n"
      "fmla z26.h, p0/M, z0.h, z5.h\n"
      "fmla z27.h, p0/M, z1.h, z5.h\n"
      "fmla z28.h, p0/M, z2.h, z5.h\n"
      "fmla z29.h, p0/M, z0.h, z6.h\n"
      "fmla z30.h, p0/M, z1.h, z6.h\n"
      "fmla z31.h, p0/M, z2.h, z6.h\n"
      "cbz x20, 6f\n"
      "ld1h { z0.h }, p0/Z, [x26]\n"
      "ld1h { z1.h }, p0/Z, [x22]\n"
      "ld1h { z2.h }, p0/Z, [x21]\n"
      "ld1rh { z3.h }, p0/Z, [%x[Apanel]]\n"
      "fmla z8.h, p0/M, z0.h, z3.h\n"
      "ld1rh { z4.h }, p0/Z, [%x[Apanel], #2]\n"
      "ld1rh { z5.h }, p0/Z, [%x[Apanel], #4]\n"
      "fmla z9.h, p0/M, z1.h, z3.h\n"
      "ld1rh { z6.h }, p0/Z, [%x[Apanel], #6]\n"
      "fmla z10.h, p0/M, z2.h, z3.h\n"
      "fmla z11.h, p0/M, z0.h, z4.h\n"
      "fmla z12.h, p0/M, z1.h, z4.h\n"
      "fmla z13.h, p0/M, z2.h, z4.h\n"
      "ld1rh { z3.h }, p0/Z, [%x[Apanel], #8]\n"
      "fmla z14.h, p0/M, z0.h, z5.h\n"
      "fmla z15.h, p0/M, z1.h, z5.h\n"
      "ld1rh { z4.h }, p0/Z, [%x[Apanel], #10]\n"
      "fmla z16.h, p0/M, z2.h, z5.h\n"
      "fmla z17.h, p0/M, z0.h, z6.h\n"
      "ld1rh { z5.h }, p0/Z, [%x[Apanel], #12]\n"
      "fmla z18.h, p0/M, z1.h, z6.h\n"
      "fmla z19.h, p0/M, z2.h, z6.h\n"
      "ld1rh { z6.h }, p0/Z, [%x[Apanel], #14]\n"
      "fmla z20.h, p0/M, z0.h, z3.h\n"
      "fmla z21.h, p0/M, z1.h, z3.h\n"
      "add %x[Apanel], %x[Apanel], #0x10\n"
      "fmla z22.h, p0/M, z2.h, z3.h\n"
      "fmla z23.h, p0/M, z0.h, z4.h\n"
      "fmla z24.h, p0/M, z1.h, z4.h\n"
      "fmla z25.h, p0/M, z2.h, z4.h\n"
      "fmla z26.h, p0/M, z0.h, z5.h\n"
      "fmla z27.h, p0/M, z1.h, z5.h\n"
      "fmla z28.h, p0/M, z2.h, z5.h\n"
      "fmla z29.h, p0/M, z0.h, z6.h\n"
      "fmla z30.h, p0/M, z1.h, z6.h\n"
      "fmla z31.h, p0/M, z2.h, z6.h\n"
      "6:"  // multiply loop done
      "dech x25, ALL, MUL #3\n"
      "st1h { z8.h }, p0, [%x[Cpanel]]\n"
      "cmp x25, XZR\n"
      "st1h { z9.h }, p0, [%x[Cpanel], #1, MUL VL]\n"
      "st1h { z10.h }, p0, [%x[Cpanel], #2, MUL VL]\n"
      "st1h { z11.h }, p0, [%x[Cpanel], #3, MUL VL]\n"
      "st1h { z12.h }, p0, [%x[Cpanel], #4, MUL VL]\n"
      "st1h { z13.h }, p0, [%x[Cpanel], #5, MUL VL]\n"
      "st1h { z14.h }, p0, [%x[Cpanel], #6, MUL VL]\n"
      "st1h { z15.h }, p0, [%x[Cpanel], #7, MUL VL]\n"
      "addvl %x[Cpanel], %x[Cpanel], #16\n"
      "st1h { z16.h }, p0, [%x[Cpanel], #-8, MUL VL]\n"
      "st1h { z17.h }, p0, [%x[Cpanel], #-7, MUL VL]\n"
      "st1h { z18.h }, p0, [%x[Cpanel], #-6, MUL VL]\n"
      "st1h { z19.h }, p0, [%x[Cpanel], #-5, MUL VL]\n"
      "st1h { z20.h }, p0, [%x[Cpanel], #-4, MUL VL]\n"
      "st1h { z21.h }, p0, [%x[Cpanel], #-3, MUL VL]\n"
      "st1h { z22.h }, p0, [%x[Cpanel], #-2, MUL VL]\n"
      "st1h { z23.h }, p0, [%x[Cpanel], #-1, MUL VL]\n"
      "st1h { z24.h }, p0, [%x[Cpanel]]\n"
      "st1h { z25.h }, p0, [%x[Cpanel], #1, MUL VL]\n"
      "st1h { z26.h }, p0, [%x[Cpanel], #2, MUL VL]\n"
      "st1h { z27.h }, p0, [%x[Cpanel], #3, MUL VL]\n"
      "st1h { z28.h }, p0, [%x[Cpanel], #4, MUL VL]\n"
      "st1h { z29.h }, p0, [%x[Cpanel], #5, MUL VL]\n"
      "st1h { z30.h }, p0, [%x[Cpanel], #6, MUL VL]\n"
      "st1h { z31.h }, p0, [%x[Cpanel], #7, MUL VL]\n"
      "addvl %x[Cpanel], %x[Cpanel], #8\n"
      "bgt 2b\n"
      "subs %x[ablocks], %x[ablocks], #0x1\n"
      "bne 1b\n"
      : [Apanel] "+&r" (Apanel), [Cpanel] "+&r" (Cpanel), [ablocks] "+&r" (ablocks)
      : [args_ptr] "r" (&ka), [offsetof_B_stride] "I" (offsetof(KernelArgs, B_stride)), [offsetof_Bpanel] "I" (offsetof(KernelArgs, Bpanel)), [offsetof_K] "I" (offsetof(KernelArgs, K)), [offsetof_N] "I" (offsetof(KernelArgs, N)), [offsetof_cur_B_ptr] "I" (offsetof(KernelArgs, cur_B_ptr))
      : "cc", "memory", "p0", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "z0", "z1", "z2", "z3", "z4", "z5", "z6", "z8", "z9", "z10", "z11", "z12", "z13", "z14", "z15", "z16", "z17", "z18", "z19", "z20", "z21", "z22", "z23", "z24", "z25", "z26", "z27", "z28", "z29", "z30", "z31"
    );
}

} // namespace arm_gemm
#endif // ARM_COMPUTE_ENABLE_SVE
