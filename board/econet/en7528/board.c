// SPDX-License-Identifier: GPL-2.0+

#include <dm.h>
#include <init.h>
#include <linux/errno.h>
#include <stdio.h>

extern U_BOOT_DRIVER(airoha_eth);

int board_init(void)
{
	return 0;
}

int board_late_init(void)
{
	struct udevice *dev;
	int ret;

	if (!IS_ENABLED(CONFIG_AIROHA_ETH))
		return 0;

	/* Probe only after the serial console is available for diagnostics. */
	ret = uclass_get_device_by_driver(UCLASS_MISC,
					  DM_DRIVER_GET(airoha_eth), &dev);
	if (ret && ret != -ENODEV)
		printf("EN7528 Ethernet probe failed: %d\n", ret);

	return 0;
}
