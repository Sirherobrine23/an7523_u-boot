/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Reconstructed from main.o.
 * This file preserves section bytes, symbols and ELF relocations.
 * Replace raw encodings with equivalent mnemonics as they are audited.
 */

.globl kernelsp
.comm kernelsp,4,4
.globl init_task_union
.comm init_task_union,2560,4

.section .text,"ax",@progbits
.balign 16
.set mips16
__recon_main_1_start:
.globl start_spram
.type start_spram, @function
start_spram:
.byte 0xe3, 0x64, 0x00, 0x18, 0x00, 0x00, 0x04, 0x67, 0x01, 0x70, 0x03, 0x61, 0x00, 0x18, 0x00, 0x00
.byte 0x00, 0x65, 0x00, 0x18, 0x00, 0x00, 0x00, 0x65, 0x03, 0xb2, 0x40, 0x9a, 0xc0, 0xea, 0x63, 0x64
.byte 0xa0, 0xe8, 0x00, 0x65, 0x80, 0x02, 0xb0, 0xbf, 0x00, 0x65, 0x00, 0x65, 0x00, 0x65, 0x00, 0x65
.reloc __recon_main_1_start + 0x2, R_MIPS16_26, spram_preprocess
.reloc __recon_main_1_start + 0xc, R_MIPS16_26, main
.reloc __recon_main_1_start + 0x12, R_MIPS16_26, spram_postprocess
.set nomips16

.section .data,"aw",@progbits
.balign 16
__recon_main_3_start:

.section .bss,"aw",@nobits
.balign 16
__recon_main_4_start:

