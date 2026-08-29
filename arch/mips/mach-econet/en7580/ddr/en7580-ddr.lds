/* SPDX-License-Identifier: GPL-2.0+ */
OUTPUT_ARCH(mips)
ENTRY(start_spram)

SECTIONS
{
	. = 0x9fa30000;

	.text : {
		*main.o(.text*)
		*(.text*)
	}

	.rodata : {
		*(.rodata*)
		*(.rdata*)
	}

	.data : {
		*(.data*)
	}

	.got : {
		*(.got*)
	}

	. = ALIGN(16);
	__bss_start = .;
	.bss : {
		*(.bss*)
		*(COMMON)
	}
	__bss_end = .;
	_end = .;

	/DISCARD/ : {
		*(.comment)
		*(.pdr)
		*(.reginfo)
		*(.MIPS.abiflags)
		*(.gnu.attributes)
	}
}
