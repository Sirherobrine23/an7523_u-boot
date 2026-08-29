/* SPDX-License-Identifier: GPL-2.0+ */
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <mach/boot.h>

	.set noreorder

ENTRY(_start)
	b	reset
	 mtc0	zero, CP0_COUNT

#ifdef CONFIG_TARGET_EN751221
	/*
	 * Preserve the legacy descriptor on EN751221. The mask ROM does not use
	 * these fields, but keeping them makes dumps recognizable by old tools.
	 */
	.org	0x0c
	.byte	0x36, 0x35, 0x37, 0x38
	.word	ECONET_DDR_BLOB_OFFSET
	.word	ECONET_DDR_BLOB_OFFSET + ECONET_DDR_BLOB_SIZE
	.word	ECONET_SPL_IMAGE_OFFSET
	.word	ECONET_UBOOT_IMAGE_OFFSET
#endif

	/* Vendor reset code leaves this area free and enters at offset 0x48. */
	.org	0x48
reset:
	/* Disable interrupts and force KSEG0 uncached before DRAM exists. */
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

	/* Arbiter and static-memory mappings shared by EN751221 and EN7580. */
	li	t0, ECONET_SYSCTL_BASE
	li	t1, 0x80071f1e
	sw	t1, 0x20(t0)
	li	t1, 0x00071f1f
	sw	t1, 0x24(t0)
	li	t1, 0x80050000
	sw	t1, 0x34(t0)

	li	t0, ECONET_SMC_BASE
	li	t1, 0x102d1040
	sw	t1, 0x00(t0)
	li	t1, 0x200028d0
	sw	t1, 0x14(t0)

	/* Disable the legacy interrupt controller. */
	li	t0, ECONET_INTC_BASE
	sw	zero, 0x00(t0)
	sw	zero, 0x04(t0)

	/* Enable PBUS access to the 48 KiB FE SRAM. */
	li	t0, ECONET_FE_SRAM_SEL
	lw	t1, 0(t0)
	ori	t1, t1, 1
	sw	t1, 0(t0)
	sync

	li	sp, ECONET_TPL_STACK_ADDR
	li	fp, ECONET_TPL_STACK_ADDR

	la	t9, tpl_main
	jalr	t9
	 nop

1:	b	1b
	 nop
END(_start)

LEAF(econet_run_ddr_blob)
	li	t0, ECONET_TPL_SAVED_RA
	sw	ra, 0(t0)

	la	t1, 1f
	li	t0, ECONET_SCREG_WR0
	sw	t1, 0(t0)
	sync

	li	t9, ECONET_DDR_BLOB_ADDR
	jr	t9
	 nop
1:
	li	t0, ECONET_SCREG_WR0
	sw	zero, 0(t0)
	li	t0, ECONET_TPL_SAVED_RA
	lw	ra, 0(t0)
	jr	ra
	 nop
END(econet_run_ddr_blob)
