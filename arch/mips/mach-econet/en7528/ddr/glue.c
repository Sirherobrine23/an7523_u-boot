// SPDX-License-Identifier: GPL-2.0-or-later
/* Minimal runtime used by the reconstructed EN7528 DDR calibration code. */

typedef unsigned char u8;
typedef unsigned int u32;
#define REG32(addr) (*(volatile u32 *)(addr))
#define REG8(addr)  (*(volatile u8 *)(addr))

#define ECONET_SYS_GLOBAL_PARM  0xbfb00284u
#define ECONET_TIMER_BASE       0xbfbf0100u
#define ECONET_TIMER_CTL        (ECONET_TIMER_BASE + 0x00)
#define ECONET_TIMER1_LDV       (ECONET_TIMER_BASE + 0x0c)
#define ECONET_TIMER1_VLR       (ECONET_TIMER_BASE + 0x10)
#define ECONET_UART_BASE        0xbfbf0000u

static unsigned int get_hclk_mhz(void)
{
	u32 mhz = (REG32(ECONET_SYS_GLOBAL_PARM) >> 10) & 0x3ff;

	return mhz ? mhz : 250;
}

static void serial_outc(char c)
{
	while (!(REG8(ECONET_UART_BASE + 0x17) & 0x20))
		;
	REG8(ECONET_UART_BASE + 0x03) = (u8)c;
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
		u8 n = (val >> (i * 4)) & 0xf;
		serial_outc(n < 10 ? '0' + n : 'a' + n - 10);
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

void time_polling_init(void)
{
	u32 hclk = get_hclk_mhz();
	u32 ctl;

	REG32(ECONET_TIMER1_LDV) = hclk * 1000u * 10u / 2u;
	ctl = REG32(ECONET_TIMER_CTL);
	ctl |= (1u << 1) | (1u << 9);
	REG32(ECONET_TIMER_CTL) = ctl;
}

void pause_polling(int usec)
{
	u32 hclk = get_hclk_mhz();
	u32 ldv = REG32(ECONET_TIMER1_LDV);
	u32 last = REG32(ECONET_TIMER1_VLR);
	u32 target = (u32)usec * (hclk / 2u);
	u32 elapsed = 0;

	while (elapsed < target) {
		u32 now = REG32(ECONET_TIMER1_VLR);
		elapsed += last >= now ? last - now : ldv - now + last;
		last = now;
	}
}

int spram_preprocess(void)
{
	time_polling_init();
	return 0;
}

int spram_postprocess(void)
{
	return 0;
}
