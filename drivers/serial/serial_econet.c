// SPDX-License-Identifier: GPL-2.0+
/* Airoha UART */

#include <dm.h>
#include <errno.h>
#include <serial.h>
#include <asm/io.h>
#include <linux/kernel.h>
#include <linux/types.h>

#define UART_RBR	0x03
#define UART_THR	0x03
#define UART_IER	0x07
#define UART_FCR	0x0b
#define UART_LCR	0x0f
#define UART_MCR	0x13
#define UART_LSR	0x17
#define UART_MISCC	0x27
#define UART_XYD	0x2c

#define UART_LCR_DLAB	0x80
#define UART_LCR_8N1	0x03
#define UART_LSR_DR	0x01
#define UART_LSR_THRE	0x20

struct en75xx_serial_plat {
	void __iomem *base;
};

static void en75xx_serial_hw_init(struct en75xx_serial_plat *plat,
				  int baudrate)
{
	void __iomem *base = plat->base;
	u32 div_x;

	/*
	 * EN7512/EN7521 use Y=65000 and X=baud*13/25 with the fixed
	 * 20 MHz UART source. This reproduces the vendor divider table.
	 */
	if (baudrate <= 0)
		baudrate = CONFIG_BAUDRATE;
	div_x = DIV_ROUND_CLOSEST((u32)baudrate * 13, 25);
	if (div_x > 0xffff)
		div_x = 0xffff;

	writeb(UART_LCR_DLAB, base + UART_LCR);
	__raw_writel((div_x << 16) | 65000, base + UART_XYD);
	writeb(1, base + UART_RBR);
	writeb(0, base + UART_IER);
	writeb(UART_LCR_8N1, base + UART_LCR);
	writeb(0x0f, base + UART_FCR);
	writeb(0, base + UART_MCR);
	writeb(0, base + UART_MISCC);
	writeb(0, base + UART_IER);
}

static int en75xx_serial_putc(struct udevice *dev, const char ch)
{
	struct en75xx_serial_plat *plat = dev_get_plat(dev);

	if (!(readb(plat->base + UART_LSR) & UART_LSR_THRE))
		return -EAGAIN;

	writeb(ch, plat->base + UART_THR);
	return 0;
}

static int en75xx_serial_getc(struct udevice *dev)
{
	struct en75xx_serial_plat *plat = dev_get_plat(dev);

	if (!(readb(plat->base + UART_LSR) & UART_LSR_DR))
		return -EAGAIN;

	return readb(plat->base + UART_RBR);
}

static int en75xx_serial_pending(struct udevice *dev, bool input)
{
	struct en75xx_serial_plat *plat = dev_get_plat(dev);
	u8 lsr = readb(plat->base + UART_LSR);

	if (input)
		return !!(lsr & UART_LSR_DR);

	return !(lsr & UART_LSR_THRE);
}

static int en75xx_serial_setbrg(struct udevice *dev, int baudrate)
{
	struct en75xx_serial_plat *plat = dev_get_plat(dev);

	en75xx_serial_hw_init(plat, baudrate);
	return 0;
}

static int en75xx_serial_of_to_plat(struct udevice *dev)
{
	struct en75xx_serial_plat *plat = dev_get_plat(dev);

	plat->base = dev_remap_addr(dev);
	if (!plat->base)
		return -EINVAL;

	return 0;
}

static int en75xx_serial_probe(struct udevice *dev)
{
	struct en75xx_serial_plat *plat = dev_get_plat(dev);

	en75xx_serial_hw_init(plat, CONFIG_BAUDRATE);
	return 0;
}

static const struct dm_serial_ops en75xx_serial_ops = {
	.putc = en75xx_serial_putc,
	.pending = en75xx_serial_pending,
	.getc = en75xx_serial_getc,
	.setbrg = en75xx_serial_setbrg,
};

static const struct udevice_id en75xx_serial_ids[] = {
	{ .compatible = "econet,en7512-uart" },
	{ }
};

U_BOOT_DRIVER(serial_en75xx) = {
	.name = "serial_en75xx",
	.id = UCLASS_SERIAL,
	.of_match = en75xx_serial_ids,
	.of_to_plat = en75xx_serial_of_to_plat,
	.plat_auto = sizeof(struct en75xx_serial_plat),
	.probe = en75xx_serial_probe,
	.ops = &en75xx_serial_ops,
	.flags = DM_FLAG_PRE_RELOC,
};
