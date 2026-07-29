# SPDX-License-Identifier: GPL-2.0+

ifeq ($(CONFIG_TPL_BUILD),y)
obj-y += early_sfc.o
obj-y += tpl/
else
obj-y += cpu.o
ifeq ($(CONFIG_SPL_BUILD),y)
obj-y += early_sfc.o
obj-y += spl/
endif
endif
