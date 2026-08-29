// SPDX-License-Identifier: GPL-2.0+

#include <fdt_support.h>
#include <init.h>
#include <sysreset.h>
#include <asm/global_data.h>
#include <asm/system.h>
#include <linux/bitops.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/sizes.h>
#include <mach/en7580.h>
#include <soc/airoha/pkgids.h>

DECLARE_GLOBAL_DATA_PTR;

#define EN7580_RESET_CONTROL		0xbfb00040UL
#define NP_SCU_BASE			((void __iomem *)CKSEG1ADDR(0x1fb00000))

int print_cpuinfo(void)
{
	u32 hir = get_pkg_mem(NP_SCU_BASE);
	u32 pdidr = get_pdidr_mem(NP_SCU_BASE);
	u32 pkgid = get_pkgid_mem(NP_SCU_BASE);
	const char *soc_name = airoha_soc_name_from_regs(hir, pkgid, pdidr);

	if (pkgid != END_PACKAGE_ID)
		printf("SoC:   Airoha %s\n", soc_name);
	else
		printf("SoC:   Airoha EN7580\n");

	return 0;
}

int dram_init(void)
{
	u32 value, size_mb;

	/*
	 * The vendor RAM-training stage records the calibrated DRAM size in
	 * MiB in SYS_GLOBAL_PARM[31:20]. Keep this contract while the training
	 * code is still provided by the GPL relocatable objects.
	 */
	value = readl((void __iomem *)EN7580_SYS_GLOBAL_PARM);
	size_mb = (value & EN7580_DRAM_SIZE_MASK) >> EN7580_DRAM_SIZE_SHIFT;

	debug("EN7580 DRAM: global-param=%08x size=%u MiB\n", value, size_mb);

	if (size_mb < 32 || size_mb > 2048) {
		printf("Invalid EN7580 calibrated DRAM size: %u MiB\n", size_mb);
		return -EINVAL;
	}

	gd->ram_size = (phys_size_t)size_mb * SZ_1M;
	return 0;
}

int dram_init_banksize(void)
{
	int bank;

	gd->bd->bi_dram[0].start = gd->ram_base;
	gd->bd->bi_dram[0].size = gd->ram_size;

	for (bank = 1; bank < CONFIG_NR_DRAM_BANKS; bank++) {
		gd->bd->bi_dram[bank].start = 0;
		gd->bd->bi_dram[bank].size = 0;
	}

	return 0;
}

#ifdef CONFIG_OF_SYSTEM_SETUP
int ft_system_setup(void *blob, struct bd_info *bd)
{
	u64 start[1] = { bd->bi_dram[0].start };
	u64 size[1] = { bd->bi_dram[0].size };

	return fdt_fixup_memory_banks(blob, start, size, 1);
}
#endif

void _machine_restart(void)
{
	writel(0x80000000, (void __iomem *)EN7580_RESET_CONTROL);
	for (;;)
		;
}
