// SPDX-License-Identifier: GPL-2.0+
/*
 * EcoNet/Airoha EN75xx SCU regmap helpers.
 *
 * EN7528 keeps the same split CHIP_SCU/SCU register layout used by EN7523:
 *   reg[0] - CHIP_SCU at 0x1fa20000
 *   reg[1] - SCU      at 0x1fb00000
 */

#include <dm/ofnode.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <mach/scu-regmap.h>

static struct regmap *airoha_scu_node_regmap_by_index(unsigned int index)
{
	static const char * const compatibles[] = {
		"airoha,en7528-scu",
		"airoha,en7523-scu",
	};
	struct regmap *map;
	ofnode node = ofnode_null();
	int err, i;

	for (i = 0; i < ARRAY_SIZE(compatibles); i++) {
		node = ofnode_by_compatible(ofnode_null(), compatibles[i]);
		if (ofnode_valid(node))
			break;
	}

	if (!ofnode_valid(node))
		return ERR_PTR(-EINVAL);

	err = regmap_init_mem_index(node, &map, index);
	if (err)
		return ERR_PTR(err);

	return map;
}

struct regmap *airoha_get_scu_regmap(void)
{
	return airoha_scu_node_regmap_by_index(1);
}

struct regmap *airoha_get_chip_scu_regmap(void)
{
	return airoha_scu_node_regmap_by_index(0);
}
