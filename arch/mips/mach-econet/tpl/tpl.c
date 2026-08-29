// SPDX-License-Identifier: GPL-2.0+

#include <compiler.h>
#include <linux/byteorder/generic.h>
#include <linux/types.h>
#include <asm/io.h>
#include <mach/boot.h>

#define IH_MAGIC	0x27051956
#define IH_HDR_SIZE	64

struct econet_legacy_header {
	u32 magic;
	u32 hcrc;
	u32 time;
	u32 size;
	u32 load;
	u32 ep;
	u32 dcrc;
	u8 os;
	u8 arch;
	u8 type;
	u8 comp;
	u8 name[32];
};

static u32 get_be32(u32 val)
{
	return be32_to_cpu(val);
}

static void tpl_putc(u8 ch)
{
	void __iomem *thr = (void __iomem *)(ECONET_UART0_BASE + 0x03);
	void __iomem *lsr = (void __iomem *)(ECONET_UART0_BASE + 0x17);

	while (!(__raw_readb(lsr) & 0x20))
		;
	__raw_writeb(ch, thr);
}

static void tpl_hang(u8 code)
{
	tpl_putc(code);
	tpl_putc('\r');
	tpl_putc('\n');

	for (;;)
		;
}

static void tpl_uart_init(void)
{
	void __iomem *base = (void __iomem *)ECONET_UART0_BASE;

	__raw_writeb(0x80, base + 0x0f);
	__raw_writel(0xea00fde8, base + 0x2c);
	__raw_writeb(0x01, base + 0x03);
	__raw_writeb(0x00, base + 0x07);
	__raw_writeb(0x03, base + 0x0f);
	__raw_writeb(0x0f, base + 0x0b);
	__raw_writeb(0x00, base + 0x13);
	__raw_writeb(0x00, base + 0x27);
	__raw_writeb(0x00, base + 0x07);
}

void __noreturn tpl_main(void)
{
	struct econet_legacy_header *hdr =
		(struct econet_legacy_header *)ECONET_SPL_HEADER_ADDR;
	void (*entry)(void);
	u32 load, size, ep;
	int ret;

	tpl_uart_init();
	if (econet_sfc_init())
		tpl_hang('I');

	ret = econet_sfc_read(ECONET_DDR_BLOB_OFFSET,
			      (void *)ECONET_DDR_BLOB_ADDR,
			      ECONET_DDR_BLOB_SIZE);
	if (ret)
		tpl_hang('F');

	econet_run_ddr_blob();

	ret = econet_sfc_read(ECONET_SPL_IMAGE_OFFSET, hdr, IH_HDR_SIZE);
	if (ret || get_be32(hdr->magic) != IH_MAGIC)
		tpl_hang('H');

	size = get_be32(hdr->size);
	load = get_be32(hdr->load);
	ep = get_be32(hdr->ep);

	if (!size || (load & 0xe0000000) != 0x80000000 ||
	    (ep & 0xe0000000) != 0x80000000 || load + size < load)
		tpl_hang('L');

	/* Write through KSEG1 so no dirty cache lines hide the SPL image. */
	ret = econet_sfc_read(ECONET_SPL_IMAGE_OFFSET + IH_HDR_SIZE,
			      (void *)(load | 0x20000000), size);
	if (ret)
		tpl_hang('S');

	__asm__ volatile("sync" : : : "memory");
	entry = (void (*)(void))ep;
	entry();
	__builtin_unreachable();
}
