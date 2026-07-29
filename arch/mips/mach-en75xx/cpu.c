// SPDX-License-Identifier: GPL-2.0+

#include <init.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <mach/en7512.h>

DECLARE_GLOBAL_DATA_PTR;

int dram_init(void)
{
	u32 val = __raw_readl((void __iomem *)EN7512_REG_SAVE_INFO);
	u32 size_mb = val & EN7512_SAVE_DRAM_MASK;

	if (!size_mb)
		size_mb = 128;

	gd->ram_size = (phys_size_t)size_mb << 20;
	return 0;
}

int print_cpuinfo(void)
{
	u32 val = __raw_readl((void __iomem *)EN7512_REG_SAVE_INFO);
	u32 clk = (val & EN7512_SAVE_CLK_MASK) >> EN7512_SAVE_CLK_SHIFT;

	printf("CPU:   EcoNet/Airoha EN7512/EN7521 MIPS34K\n");
	if (clk)
		printf("Clock: %u MHz\n", clk);
	else
		printf("Clock: unknown\n");

	return 0;
}

ulong notrace get_tbclk(void)
{
	u32 val = __raw_readl((void __iomem *)EN7512_REG_SAVE_INFO);
	u32 clk = (val & EN7512_SAVE_CLK_MASK) >> EN7512_SAVE_CLK_SHIFT;

	/* CP0 Count advances at half the system clock on the MIPS34K. */
	if (clk)
		return (ulong)clk * 500000;

	return CONFIG_SYS_MIPS_TIMER_FREQ;
}

void _machine_restart(void)
{
	__raw_writel(0x80000000, (void __iomem *)EN7512_RESET_CONTROL);

	for (;;)
		;
}
