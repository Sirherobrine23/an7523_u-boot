# SPDX-License-Identifier: GPL-2.0+

ifeq ($(CONFIG_TPL_BUILD),y)
obj-y += early_sfc.o
obj-y += tpl/
else
obj-y += cpu.o
obj-$(CONFIG_TARGET_EN751221) += en751221/
obj-$(CONFIG_TARGET_EN7528)   += en7528/
ifeq ($(CONFIG_SPL_BUILD),y)
obj-y += early_sfc.o
obj-y += spl/
endif
endif
