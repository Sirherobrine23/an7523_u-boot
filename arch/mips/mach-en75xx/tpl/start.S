/* SPDX-License-Identifier: GPL-2.0+ */
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <mach/en7512.h>

	.set noreorder

ENTRY(_start)
	b	reset
	 mtc0	zero, CP0_COUNT

	/*
	 * TCBoot image descriptor. The magic is byte-reversed in the vendor
	 * format, while section offsets are stored as big-endian words.
	 */
	.org	0x0c
	.byte	0x36, 0x35, 0x37, 0x38
	.word	EN7512_DDR_BLOB_OFFSET
	.word	EN7512_DDR_BLOB_OFFSET + EN7512_DDR_BLOB_SIZE
	.word	EN7512_SPL_IMAGE_OFFSET
	.word	EN7512_UBOOT_IMAGE_OFFSET

	/* The vendor reset code begins at offset 0x48. */
	.org	0x48
reset:
	/* Disable interrupts and force KSEG0 uncached before DDR exists. */
	mfc0	t0, CP0_STATUS
	li	t1, ~(ST0_IE | ST0_KSU)
	and	t0, t0, t1
	li	t1, ST0_BEV | ST0_ERL
	or	t0, t0, t1
	mtc0	t0, CP0_STATUS
	mtc0	zero, CP0_CAUSE
	mtc0	zero, CP0_COMPARE

	mfc0	t0, CP0_CONFIG
	li	t1, ~7
	and	t0, t0, t1
	ori	t0, t0, 2
	mtc0	t0, CP0_CONFIG
	ehb

	/* Arbiter and static-memory mappings copied from TCBoot. */
	li	t0, EN7512_SYSCTL_BASE
	li	t1, 0x80071f1e
	sw	t1, 0x20(t0)
	li	t1, 0x00071f1f
	sw	t1, 0x24(t0)
	li	t1, 0x80050000
	sw	t1, 0x34(t0)

	li	t0, EN7512_SMC_BASE
	li	t1, 0x102d1040
	sw	t1, 0x00(t0)
	li	t1, 0x200028d0
	sw	t1, 0x14(t0)

	/* Disable the interrupt controller. */
	li	t0, EN7512_INTC_BASE
	sw	zero, 0x00(t0)
	sw	zero, 0x04(t0)

	/* Enable PBUS access to the 48 KiB FE SRAM. */
	li	t0, EN7512_FE_SRAM_SEL
	lw	t1, 0(t0)
	ori	t1, t1, 1
	sw	t1, 0(t0)
	sync

	li	sp, EN7512_TPL_STACK_ADDR
	li	fp, EN7512_TPL_STACK_ADDR

	la	t9, tpl_main
	jalr	t9
	 nop

1:	b	1b
	 nop
END(_start)

LEAF(en7512_run_ddr_blob)
	li	t0, EN7512_TPL_SAVED_RA
	sw	ra, 0(t0)

	la	t1, 1f
	li	t0, EN7512_SCREG_WR0
	sw	t1, 0(t0)
	sync

	li	t9, EN7512_DDR_BLOB_ADDR
	jr	t9
	 nop
1:
	li	t0, EN7512_SCREG_WR0
	sw	zero, 0(t0)
	li	t0, EN7512_TPL_SAVED_RA
	lw	ra, 0(t0)
	jr	ra
	 nop
END(en7512_run_ddr_blob)
