/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Reconstructed from setup.o.
 * This file preserves section bytes, symbols and ELF relocations.
 * Replace raw encodings with equivalent mnemonics as they are audited.
 */

.section .text,"ax",@progbits
.balign 16
__recon_setup_1_start:
.globl init_arch
.type init_arch, @function
init_arch:
.byte 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
.globl setup_arch
.type setup_arch, @function
setup_arch:
.byte 0x03, 0xe0, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00
.reloc __recon_setup_1_start + 0x0, R_MIPS_26, start_spram

.section .data,"aw",@progbits
.balign 16
__recon_setup_3_start:

.section .bss,"aw",@nobits
.balign 16
__recon_setup_4_start:

