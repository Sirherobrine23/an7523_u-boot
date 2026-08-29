// SPDX-License-Identifier: GPL-2.0+
/*
 * Glue for the EN7580 RAM-training objects found in the XGZ030 GPL release.
 *
 * This is intentionally small. It supplies only the runtime services needed
 * by the relocatable DRAMC objects. SMP bring-up and the vendor boot-extension
 * path are left to U-Boot after DRAM is usable.
 */

typedef unsigned char u8;
typedef unsigned int u32;
#define REG32(addr)	(*(volatile u32 *)(addr))
#define REG8(addr)	(*(volatile u8 *)(addr))

#define EN7580_SYS_GLOBAL_PARM	0xbfb00284u
#define EN7580_TIMER_BASE	0xbfbf0100u
#define EN7580_TIMER_CTL	(EN7580_TIMER_BASE + 0x00)
#define EN7580_TIMER1_LDV	(EN7580_TIMER_BASE + 0x0c)
#define EN7580_TIMER1_VLR	(EN7580_TIMER_BASE + 0x10)
#define EN7580_UART_BASE	0xbfbf0000u

extern void init_system(int bootrom_ext);

void *memset(void *dst, int c, unsigned long len)
{
	u8 *p = dst;

	while (len--)
		*p++ = (u8)c;

	return dst;
}

int get_SYS_HCLK(void)
{
	u32 mhz = (REG32(EN7580_SYS_GLOBAL_PARM) >> 10) & 0x3ff;

	/* init_system() normally fills this field before eFuse accesses. */
	return mhz ? (int)mhz : 260;
}

static void serial_outc(char c)
{
	while (!(REG8(EN7580_UART_BASE + 0x17) & 0x20))
		;

	REG8(EN7580_UART_BASE + 0x03) = (u8)c;
}

void prom_puts(const char *str)
{
	while (*str) {
		if (*str == '\n')
			serial_outc('\r');
		serial_outc(*str++);
	}
}

void prom_print_hex(unsigned long val, int len)
{
	int i;

	for (i = len - 1; i >= 0; i--) {
		u8 nibble = (val >> (i * 4)) & 0xf;

		serial_outc(nibble < 10 ? '0' + nibble : 'a' + nibble - 10);
	}
}

void prom_print_dec(unsigned long val)
{
	char buf[11];
	int i = sizeof(buf);

	buf[--i] = '\0';
	do {
		buf[--i] = '0' + val % 10;
		val /= 10;
	} while (val && i);

	prom_puts(&buf[i]);
}

int prom_printf(const char *fmt, ...)
{
	/* pkgId.o only uses this for diagnostics; formatting is not required. */
	prom_puts(fmt);
	return 0;
}

void time_polling_init(void)
{
	u32 hclk = get_SYS_HCLK();
	u32 ctl;

	REG32(EN7580_TIMER1_LDV) = hclk * 1000u * 10u / 2u;
	ctl = REG32(EN7580_TIMER_CTL);
	ctl |= (1u << 1) | (1u << 9);
	REG32(EN7580_TIMER_CTL) = ctl;
}

void pause_polling(int usec)
{
	u32 hclk = get_SYS_HCLK();
	u32 ldv = REG32(EN7580_TIMER1_LDV);
	u32 last = REG32(EN7580_TIMER1_VLR);
	u32 target = (u32)usec * (hclk / 2u);
	u32 elapsed = 0;

	while (elapsed < target) {
		u32 now = REG32(EN7580_TIMER1_VLR);

		elapsed += last >= now ? last - now : ldv - now + last;
		last = now;
	}
}

int spram_preprocess(void)
{
	/*
	 * The XGZ030 system.o selects FPGA/ASIC clocks, SPI-controller ECC and
	 * DDR3/DDR4 package information from straps/eFuse before calibration.
	 */
	init_system(0);
	time_polling_init();
	return 0;
}

int spram_postprocess(void)
{
	/* CPU1-3 are intentionally left parked; U-Boot owns SMP bring-up. */
	return 0;
}

/*
 * Temporary bring-up fallback. The original EN7580 system code consults
 * eFuse/package data before selecting some clock and DRAM parameters. Replace
 * this with the native eFuse reader before using the port on unknown packages.
 */
int efuse_read_data(void)
{
	return 0;
}
