/* SPDX-License-Identifier: GPL-2.0+ */
#ifndef __EN75XX_AIROHA_SCU_REGMAP_H__
#define __EN75XX_AIROHA_SCU_REGMAP_H__

#include <regmap.h>

struct regmap *airoha_get_scu_regmap(void);
struct regmap *airoha_get_chip_scu_regmap(void);

#endif
