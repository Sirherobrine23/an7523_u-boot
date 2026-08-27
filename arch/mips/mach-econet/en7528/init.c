// SPDX-License-Identifier: GPL-2.0
/*
 * Author: Matheus Sampaio Queiroga <srherobrine20@gmail.com>
 * Author: Mikhail Kshevetskiy <mikhail.kshevetskiy@iopsys.eu>
 */
#include <fdt_support.h>
#include <init.h>
#include <sysreset.h>
#include <asm/global_data.h>
#include <asm/system.h>
#include <linux/bitops.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/sizes.h>

DECLARE_GLOBAL_DATA_PTR;

#define EN7528_RESET_CONTROL				0xbfb00040UL
#define EN7528_SYS_GLOBAL_PARM				0xbfb00284UL
#define EN7528_SYS_GLOBAL_DRAM_SIZE_MASK	GENMASK(31, 20)
#define EN7528_SYS_GLOBAL_DRAM_SIZE_SHIFT	20

int print_cpuinfo(void)
{
	printf("CPU:   Airoha EN7528\n");
	return 0;
}

int dram_init(void)
{
	u32 value, size_mb;

	/*
	 * The vendor first stage stores the calibrated DRAM size in MiB in the
	 * upper 12 bits of SYS_GLOBAL_PARM.  EN751221 uses the lower 12 bits of
	 * the same register because its boot code is built big-endian, whereas
	 * EN7528 is little-endian.
	 */
	value = readl((void __iomem *)EN7528_SYS_GLOBAL_PARM);
	size_mb = (value & EN7528_SYS_GLOBAL_DRAM_SIZE_MASK) >>
		  EN7528_SYS_GLOBAL_DRAM_SIZE_SHIFT;

	debug("EN7528 DRAM: global-param=%08x size=%u MiB\n",
	      value, size_mb);

	if (size_mb < 32 || size_mb > 512) {
		printf("Invalid EN7528 calibrated DRAM size: %u MiB\n",
		       size_mb);
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
	writel(0x80000000, (void __iomem *)EN7528_RESET_CONTROL);
	while (1) {
		/* loop forever */
	}
}
