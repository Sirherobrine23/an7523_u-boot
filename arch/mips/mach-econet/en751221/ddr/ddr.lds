/* SPDX-License-Identifier: GPL-2.0-or-later */
ENTRY(start)
SECTIONS
{
	. = 0x9fa32800;
	.init : { *(.init) } = 0
	.text : { *(.text) *(.gnu.warning) } = 0
	_etext = .;
	.data : {
		_fdata = .;
		*(.rodata) *(.rodata1) *(.rodata.str1.4) *(.data)
		_edata = .;
	}
	__bss_start = .;
	_fbss = .;
	.sbss : { *(.sbss) *(.scommon) }
	.bss : { *(.dynbss) *(.bss) *(COMMON) . = ALIGN(4); _end = .; } = 0
}
