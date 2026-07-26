// SPDX-License-Identifier: GPL-2.0+
/*
 * bin2mac - read a raw 6-byte MAC address from memory and store it as a
 * formatted "xx:xx:xx:xx:xx:xx" string into an environment variable.
 */

#include <command.h>
#include <env.h>
#include <net.h>
#include <mapmem.h>
#include <vsprintf.h>

static int do_bin2mac(struct cmd_tbl *cmdtp, int flag, int argc,
		       char *const argv[])
{
	unsigned long addr, size;
	const char *env_name;
	u8 enetaddr[ARP_HLEN];
	void *buf;
	int ret;

	if (argc != 4)
		return CMD_RET_USAGE;

	env_name = argv[1];
	addr = hextoul(argv[2], NULL);
	size = hextoul(argv[3], NULL);

	if (size != ARP_HLEN) {
		printf("bin2mac: size must be %d bytes\n", ARP_HLEN);
		return CMD_RET_FAILURE;
	}

	buf = map_sysmem(addr, size);
	memcpy(enetaddr, buf, ARP_HLEN);
	unmap_sysmem(buf);

	if (!is_valid_ethaddr(enetaddr)) {
		printf("bin2mac: %pM is not a valid MAC address\n", enetaddr);
		return CMD_RET_FAILURE;
	}

	ret = eth_env_set_enetaddr(env_name, enetaddr);
	if (ret) {
		printf("bin2mac: failed to set '%s' (%d)\n", env_name, ret);
		return CMD_RET_FAILURE;
	}

	printf("%s=%pM\n", env_name, enetaddr);

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	bin2mac, 4, 0, do_bin2mac,
	"convert a binary MAC address in memory to an environment variable",
	"<env_name> <address> <size>\n"
	"    - read <size> (must be 6) bytes of a raw MAC address from\n"
	"      <address> and store it formatted as \"xx:xx:xx:xx:xx:xx\"\n"
	"      into environment variable <env_name>, e.g.:\n"
	"      bin2mac ethaddr 0x80000000 6"
);
