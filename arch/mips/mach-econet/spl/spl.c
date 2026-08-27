// SPDX-License-Identifier: GPL-2.0+

#include <errno.h>
#include <init.h>
#include <spl.h>
#include <spl_load.h>
#include <mach/en751221.h>

void __noreturn board_init_f(ulong dummy)
{
	spl_init();

	if (IS_ENABLED(CONFIG_SPL_SERIAL))
		preloader_console_init();

	board_init_r(NULL, 0);
}

static ulong en7512_spl_read(struct spl_load_info *load, ulong offset,
			     ulong count, void *buf)
{
	if (en7512_sfc_read(offset, buf, count))
		return 0;

	return count;
}

static int en7512_spl_load_image(struct spl_image_info *spl_image,
				 struct spl_boot_device *bootdev)
{
	struct spl_load_info load;

	if (en7512_sfc_init())
		return -EIO;

	spl_load_init(&load, en7512_spl_read, NULL, 1);
	return spl_load(spl_image, bootdev, &load, 0,
			EN7512_UBOOT_IMAGE_OFFSET);
}

SPL_LOAD_IMAGE_METHOD("EN7512 SFC", 0, BOOT_DEVICE_BOARD,
		      en7512_spl_load_image);

u32 spl_boot_device(void)
{
	return BOOT_DEVICE_BOARD;
}
