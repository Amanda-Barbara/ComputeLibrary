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
#include "../../bfloat.hpp"

namespace arm_gemm {

void sve_ffinterleaved_bf16fp32_mmla_8x3VL(
    const bfloat16 *Apanel,
    const bfloat16 *Bpanel,
    size_t B_stride,
    float *Cpanel,
    int ablocks,
    size_t N,
    int K) {

    struct KernelArgs {
        size_t K = {};
        const bfloat16 *Bpanel = {};
        size_t N = {};
        size_t B_stride = {};
        const bfloat16 *cur_B_ptr = {};
    } ka;

    ka.K = (K/4) - 1;
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
      "cntw x23, ALL, MUL #2\n"
      "add x22, x26, x20, LSL #1\n"
      "add x21, x22, x20, LSL #1\n"
      "add x20, x21, x20, LSL #1\n"
      "cmp x25, x23\n"
      "str x20, [%x[args_ptr], %[offsetof_cur_B_ptr]]\n"
      "mov %x[Apanel], x24\n"
      "bgt 3f\n"
      "decw x23\n"
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
      "ld1h { z4.h }, p0/Z, [x26]\n"
      "mov z11.b, #0x0\n"
      "mov z12.b, #0x0\n"
      "ld1rqh { z0.h }, p0/Z, [%x[Apanel]]\n"
      "mov z13.b, #0x0\n"
      "mov z14.b, #0x0\n"
      "ld1rqh { z1.h }, p0/Z, [%x[Apanel], #16]\n"
      "mov z15.b, #0x0\n"
      "mov z16.b, #0x0\n"
      "ld1h { z5.h }, p0/Z, [x26, #1, MUL VL]\n"
      "mov z17.b, #0x0\n"
      "mov z18.b, #0x0\n"
      "ld1rqh { z2.h }, p0/Z, [%x[Apanel], #32]\n"
      "mov z19.b, #0x0\n"
      "mov z20.b, #0x0\n"
      "addvl x26, x26, #2\n"
      "mov z21.b, #0x0\n"
      "mov z22.b, #0x0\n"
      "add %x[Apanel], %x[Apanel], #0x30\n"
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
      "ld1rqh { z3.h }, p0/Z, [%x[Apanel]]\n"
      ".inst 0x6464e408  // bfmmla z8.s, z0.h, z4.h\n"
      ".inst 0x6465e40b  // bfmmla z11.s, z0.h, z5.h\n"
      ".inst 0x6464e42e  // bfmmla z14.s, z1.h, z4.h\n"
      ".inst 0x6465e431  // bfmmla z17.s, z1.h, z5.h\n"
      "ld1h { z6.h }, p0/Z, [x22]\n"
      ".inst 0x6464e454  // bfmmla z20.s, z2.h, z4.h\n"
      ".inst 0x6465e457  // bfmmla z23.s, z2.h, z5.h\n"
      "ld1h { z7.h }, p0/Z, [x22, #1, MUL VL]\n"
      ".inst 0x6464e47a  // bfmmla z26.s, z3.h, z4.h\n"
      ".inst 0x6465e47d  // bfmmla z29.s, z3.h, z5.h\n"
      "ld1h { z4.h }, p0/Z, [x21]\n"
      "ld1h { z5.h }, p0/Z, [x21, #1, MUL VL]\n"
      ".inst 0x6466e409  // bfmmla z9.s, z0.h, z6.h\n"
      ".inst 0x6467e40c  // bfmmla z12.s, z0.h, z7.h\n"
      ".inst 0x6466e42f  // bfmmla z15.s, z1.h, z6.h\n"
      ".inst 0x6467e432  // bfmmla z18.s, z1.h, z7.h\n"
      "sub x20, x20, #0x2\n"
      ".inst 0x6466e455  // bfmmla z21.s, z2.h, z6.h\n"
      ".inst 0x6467e458  // bfmmla z24.s, z2.h, z7.h\n"
      "cmp x20, #0x2\n"
      ".inst 0x6466e47b  // bfmmla z27.s, z3.h, z6.h\n"
      ".inst 0x6467e47e  // bfmmla z30.s, z3.h, z7.h\n"
      "ld1h { z6.h }, p0/Z, [x26]\n"
      ".inst 0x6464e40a  // bfmmla z10.s, z0.h, z4.h\n"
      ".inst 0x6465e40d  // bfmmla z13.s, z0.h, z5.h\n"
      "ld1rqh { z0.h }, p0/Z, [%x[Apanel], #16]\n"
      ".inst 0x6464e430  // bfmmla z16.s, z1.h, z4.h\n"
      ".inst 0x6465e433  // bfmmla z19.s, z1.h, z5.h\n"
      "ld1rqh { z1.h }, p0/Z, [%x[Apanel], #32]\n"
      ".inst 0x6464e456  // bfmmla z22.s, z2.h, z4.h\n"
      ".inst 0x6465e459  // bfmmla z25.s, z2.h, z5.h\n"
      "ld1h { z7.h }, p0/Z, [x26, #1, MUL VL]\n"
      ".inst 0x6464e47c  // bfmmla z28.s, z3.h, z4.h\n"
      ".inst 0x6465e47f  // bfmmla z31.s, z3.h, z5.h\n"
      "ld1rqh { z2.h }, p0/Z, [%x[Apanel], #48]\n"
      "ld1rqh { z3.h }, p0/Z, [%x[Apanel], #64]\n"
      ".inst 0x6466e408  // bfmmla z8.s, z0.h, z6.h\n"
      ".inst 0x6467e40b  // bfmmla z11.s, z0.h, z7.h\n"
      ".inst 0x6466e42e  // bfmmla z14.s, z1.h, z6.h\n"
      ".inst 0x6467e431  // bfmmla z17.s, z1.h, z7.h\n"
      "ld1h { z4.h }, p0/Z, [x22, #2, MUL VL]\n"
      ".inst 0x6466e454  // bfmmla z20.s, z2.h, z6.h\n"
      ".inst 0x6467e457  // bfmmla z23.s, z2.h, z7.h\n"
      "ld1h { z5.h }, p0/Z, [x22, #3, MUL VL]\n"
      ".inst 0x6466e47a  // bfmmla z26.s, z3.h, z6.h\n"
      ".inst 0x6467e47d  // bfmmla z29.s, z3.h, z7.h\n"
      "ld1h { z6.h }, p0/Z, [x21, #2, MUL VL]\n"
      "ld1h { z7.h }, p0/Z, [x21, #3, MUL VL]\n"
      ".inst 0x6464e409  // bfmmla z9.s, z0.h, z4.h\n"
      ".inst 0x6465e40c  // bfmmla z12.s, z0.h, z5.h\n"
      ".inst 0x6464e42f  // bfmmla z15.s, z1.h, z4.h\n"
      ".inst 0x6465e432  // bfmmla z18.s, z1.h, z5.h\n"
      "addvl x22, x22, #4\n"
      ".inst 0x6464e455  // bfmmla z21.s, z2.h, z4.h\n"
      ".inst 0x6465e458  // bfmmla z24.s, z2.h, z5.h\n"
      "addvl x21, x21, #4\n"
      ".inst 0x6464e47b  // bfmmla z27.s, z3.h, z4.h\n"
      ".inst 0x6465e47e  // bfmmla z30.s, z3.h, z5.h\n"
      "ld1h { z4.h }, p0/Z, [x26, #2, MUL VL]\n"
      ".inst 0x6466e40a  // bfmmla z10.s, z0.h, z6.h\n"
      ".inst 0x6467e40d  // bfmmla z13.s, z0.h, z7.h\n"
      "ld1rqh { z0.h }, p0/Z, [%x[Apanel], #80]\n"
      ".inst 0x6466e430  // bfmmla z16.s, z1.h, z6.h\n"
      ".inst 0x6467e433  // bfmmla z19.s, z1.h, z7.h\n"
      "ld1rqh { z1.h }, p0/Z, [%x[Apanel], #96]\n"
      ".inst 0x6466e456  // bfmmla z22.s, z2.h, z6.h\n"
      ".inst 0x6467e459  // bfmmla z25.s, z2.h, z7.h\n"
      "ld1h { z5.h }, p0/Z, [x26, #3, MUL VL]\n"
      ".inst 0x6466e47c  // bfmmla z28.s, z3.h, z6.h\n"
      ".inst 0x6467e47f  // bfmmla z31.s, z3.h, z7.h\n"
      "ld1rqh { z2.h }, p0/Z, [%x[Apanel], #112]\n"
      "add %x[Apanel], %x[Apanel], #0x80\n"
      "addvl x26, x26, #4\n"
      "bge 4b\n"
      "5:"  // main loop skip
      "ld1rqh { z3.h }, p0/Z, [%x[Apanel]]\n"
      ".inst 0x6464e408  // bfmmla z8.s, z0.h, z4.h\n"
      ".inst 0x6465e40b  // bfmmla z11.s, z0.h, z5.h\n"
      ".inst 0x6464e42e  // bfmmla z14.s, z1.h, z4.h\n"
      ".inst 0x6465e431  // bfmmla z17.s, z1.h, z5.h\n"
      "ld1h { z6.h }, p0/Z, [x22]\n"
      ".inst 0x6464e454  // bfmmla z20.s, z2.h, z4.h\n"
      ".inst 0x6465e457  // bfmmla z23.s, z2.h, z5.h\n"
      "ld1h { z7.h }, p0/Z, [x22, #1, MUL VL]\n"
      ".inst 0x6464e47a  // bfmmla z26.s, z3.h, z4.h\n"
      ".inst 0x6465e47d  // bfmmla z29.s, z3.h, z5.h\n"
      "ld1h { z4.h }, p0/Z, [x21]\n"
      "ld1h { z5.h }, p0/Z, [x21, #1, MUL VL]\n"
      ".inst 0x6466e409  // bfmmla z9.s, z0.h, z6.h\n"
      ".inst 0x6467e40c  // bfmmla z12.s, z0.h, z7.h\n"
      ".inst 0x6466e42f  // bfmmla z15.s, z1.h, z6.h\n"
      ".inst 0x6467e432  // bfmmla z18.s, z1.h, z7.h\n"
      "add %x[Apanel], %x[Apanel], #0x10\n"
      ".inst 0x6466e455  // bfmmla z21.s, z2.h, z6.h\n"
      ".inst 0x6467e458  // bfmmla z24.s, z2.h, z7.h\n"
      "addvl x22, x22, #2\n"
      ".inst 0x6466e47b  // bfmmla z27.s, z3.h, z6.h\n"
      ".inst 0x6467e47e  // bfmmla z30.s, z3.h, z7.h\n"
      "addvl x21, x21, #2\n"
      ".inst 0x6464e40a  // bfmmla z10.s, z0.h, z4.h\n"
      ".inst 0x6465e40d  // bfmmla z13.s, z0.h, z5.h\n"
      ".inst 0x6464e430  // bfmmla z16.s, z1.h, z4.h\n"
      ".inst 0x6465e433  // bfmmla z19.s, z1.h, z5.h\n"
      ".inst 0x6464e456  // bfmmla z22.s, z2.h, z4.h\n"
      ".inst 0x6465e459  // bfmmla z25.s, z2.h, z5.h\n"
      ".inst 0x6464e47c  // bfmmla z28.s, z3.h, z4.h\n"
      ".inst 0x6465e47f  // bfmmla z31.s, z3.h, z5.h\n"
      "cbz x20, 6f\n"
      "ld1h { z6.h }, p0/Z, [x26]\n"
      "ld1rqh { z0.h }, p0/Z, [%x[Apanel]]\n"
      ".inst 0x6466e408  // bfmmla z8.s, z0.h, z6.h\n"
      "ld1rqh { z1.h }, p0/Z, [%x[Apanel], #16]\n"
      "ld1h { z7.h }, p0/Z, [x26, #1, MUL VL]\n"
      ".inst 0x6467e40b  // bfmmla z11.s, z0.h, z7.h\n"
      "ld1rqh { z2.h }, p0/Z, [%x[Apanel], #32]\n"
      "ld1rqh { z3.h }, p0/Z, [%x[Apanel], #48]\n"
      ".inst 0x6466e42e  // bfmmla z14.s, z1.h, z6.h\n"
      ".inst 0x6467e431  // bfmmla z17.s, z1.h, z7.h\n"
      ".inst 0x6466e454  // bfmmla z20.s, z2.h, z6.h\n"
      "ld1h { z4.h }, p0/Z, [x22]\n"
      ".inst 0x6467e457  // bfmmla z23.s, z2.h, z7.h\n"
      ".inst 0x6466e47a  // bfmmla z26.s, z3.h, z6.h\n"
      "ld1h { z5.h }, p0/Z, [x22, #1, MUL VL]\n"
      ".inst 0x6467e47d  // bfmmla z29.s, z3.h, z7.h\n"
      "ld1h { z6.h }, p0/Z, [x21]\n"
      "ld1h { z7.h }, p0/Z, [x21, #1, MUL VL]\n"
      ".inst 0x6464e409  // bfmmla z9.s, z0.h, z4.h\n"
      ".inst 0x6465e40c  // bfmmla z12.s, z0.h, z5.h\n"
      "add %x[Apanel], %x[Apanel], #0x40\n"
      ".inst 0x6464e42f  // bfmmla z15.s, z1.h, z4.h\n"
      ".inst 0x6465e432  // bfmmla z18.s, z1.h, z5.h\n"
      ".inst 0x6464e455  // bfmmla z21.s, z2.h, z4.h\n"
      ".inst 0x6465e458  // bfmmla z24.s, z2.h, z5.h\n"
      ".inst 0x6464e47b  // bfmmla z27.s, z3.h, z4.h\n"
      ".inst 0x6465e47e  // bfmmla z30.s, z3.h, z5.h\n"
      ".inst 0x6466e40a  // bfmmla z10.s, z0.h, z6.h\n"
      ".inst 0x6467e40d  // bfmmla z13.s, z0.h, z7.h\n"
      ".inst 0x6466e430  // bfmmla z16.s, z1.h, z6.h\n"
      ".inst 0x6467e433  // bfmmla z19.s, z1.h, z7.h\n"
      ".inst 0x6466e456  // bfmmla z22.s, z2.h, z6.h\n"
      ".inst 0x6467e459  // bfmmla z25.s, z2.h, z7.h\n"
      ".inst 0x6466e47c  // bfmmla z28.s, z3.h, z6.h\n"
      ".inst 0x6467e47f  // bfmmla z31.s, z3.h, z7.h\n"
      "6:"  // multiply loop done
      "decw x25, ALL, MUL #3\n"
      "uzp1 z4.d, z8.d, z11.d\n"
      "uzp2 z8.d, z8.d, z11.d\n"
      "uzp1 z11.d, z9.d, z12.d\n"
      "uzp2 z9.d, z9.d, z12.d\n"
      "st1w { z4.s }, p0, [%x[Cpanel]]\n"
      "uzp1 z12.d, z10.d, z13.d\n"
      "uzp2 z10.d, z10.d, z13.d\n"
      "st1w { z11.s }, p0, [%x[Cpanel], #1, MUL VL]\n"
      "st1w { z12.s }, p0, [%x[Cpanel], #2, MUL VL]\n"
      "uzp1 z13.d, z14.d, z17.d\n"
      "uzp2 z14.d, z14.d, z17.d\n"
      "st1w { z8.s }, p0, [%x[Cpanel], #3, MUL VL]\n"
      "uzp1 z17.d, z15.d, z18.d\n"
      "cmp x25, XZR\n"
      "st1w { z9.s }, p0, [%x[Cpanel], #4, MUL VL]\n"
      "uzp2 z15.d, z15.d, z18.d\n"
      "uzp1 z18.d, z16.d, z19.d\n"
      "st1w { z10.s }, p0, [%x[Cpanel], #5, MUL VL]\n"
      "uzp2 z16.d, z16.d, z19.d\n"
      "uzp1 z19.d, z20.d, z23.d\n"
      "st1w { z13.s }, p0, [%x[Cpanel], #6, MUL VL]\n"
      "uzp2 z20.d, z20.d, z23.d\n"
      "uzp1 z23.d, z21.d, z24.d\n"
      "st1w { z17.s }, p0, [%x[Cpanel], #7, MUL VL]\n"
      "addvl %x[Cpanel], %x[Cpanel], #16\n"
      "uzp2 z21.d, z21.d, z24.d\n"
      "st1w { z18.s }, p0, [%x[Cpanel], #-8, MUL VL]\n"
      "uzp1 z24.d, z22.d, z25.d\n"
      "uzp2 z22.d, z22.d, z25.d\n"
      "st1w { z14.s }, p0, [%x[Cpanel], #-7, MUL VL]\n"
      "uzp1 z25.d, z26.d, z29.d\n"
      "uzp2 z26.d, z26.d, z29.d\n"
      "st1w { z15.s }, p0, [%x[Cpanel], #-6, MUL VL]\n"
      "uzp1 z29.d, z27.d, z30.d\n"
      "uzp2 z27.d, z27.d, z30.d\n"
      "st1w { z16.s }, p0, [%x[Cpanel], #-5, MUL VL]\n"
      "uzp1 z30.d, z28.d, z31.d\n"
      "uzp2 z28.d, z28.d, z31.d\n"
      "st1w { z19.s }, p0, [%x[Cpanel], #-4, MUL VL]\n"
      "st1w { z23.s }, p0, [%x[Cpanel], #-3, MUL VL]\n"
      "st1w { z24.s }, p0, [%x[Cpanel], #-2, MUL VL]\n"
      "st1w { z20.s }, p0, [%x[Cpanel], #-1, MUL VL]\n"
      "st1w { z21.s }, p0, [%x[Cpanel]]\n"
      "st1w { z22.s }, p0, [%x[Cpanel], #1, MUL VL]\n"
      "st1w { z25.s }, p0, [%x[Cpanel], #2, MUL VL]\n"
      "st1w { z29.s }, p0, [%x[Cpanel], #3, MUL VL]\n"
      "st1w { z30.s }, p0, [%x[Cpanel], #4, MUL VL]\n"
      "st1w { z26.s }, p0, [%x[Cpanel], #5, MUL VL]\n"
      "st1w { z27.s }, p0, [%x[Cpanel], #6, MUL VL]\n"
      "st1w { z28.s }, p0, [%x[Cpanel], #7, MUL VL]\n"
      "addvl %x[Cpanel], %x[Cpanel], #8\n"
      "bgt 2b\n"
      "subs %x[ablocks], %x[ablocks], #0x1\n"
      "bne 1b\n"
      : [Apanel] "+&r" (Apanel), [Cpanel] "+&r" (Cpanel), [ablocks] "+&r" (ablocks)
      : [args_ptr] "r" (&ka), [offsetof_B_stride] "I" (offsetof(KernelArgs, B_stride)), [offsetof_Bpanel] "I" (offsetof(KernelArgs, Bpanel)), [offsetof_K] "I" (offsetof(KernelArgs, K)), [offsetof_N] "I" (offsetof(KernelArgs, N)), [offsetof_cur_B_ptr] "I" (offsetof(KernelArgs, cur_B_ptr))
      : "cc", "memory", "p0", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "z0", "z1", "z2", "z3", "z4", "z5", "z6", "z7", "z8", "z9", "z10", "z11", "z12", "z13", "z14", "z15", "z16", "z17", "z18", "z19", "z20", "z21", "z22", "z23", "z24", "z25", "z26", "z27", "z28", "z29", "z30", "z31"
    );
}

} // namespace arm_gemm
#endif // ARM_COMPUTE_ENABLE_SVE
