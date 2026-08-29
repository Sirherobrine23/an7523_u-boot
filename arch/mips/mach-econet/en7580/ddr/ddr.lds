/* SPDX-License-Identifier: GPL-2.0-or-later */
ENTRY(start_spram)
SECTIONS
{
	. = 0x9fa30000;
	.text : { *(.text) *(.text.*) } = 0
	.rodata : { *(.rodata) *(.rodata.*) }
	.data : { *(.data) *(.data.*) }
	.got : { *(.got) *(.got.plt) }
	__bss_start = .;
	.sbss : { *(.sbss) *(.scommon) }
	.bss : { *(.bss) *(COMMON) . = ALIGN(4); _end = .; } = 0
}
