/*
 * (C) Copyright ASPEED Technology Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <debug_uart.h>
#include <spl.h>
#include <dm.h>
#include <mmc.h>
#include <xyzModem.h>
#include <asm/io.h>
#include <asm/arch/aspeed_verify.h>

DECLARE_GLOBAL_DATA_PTR;

#define AST_BOOTMODE_SPI	0
#define AST_BOOTMODE_EMMC	1
#define AST_BOOTMODE_UART	2

u32 aspeed_bootmode(void);
void aspeed_mmc_init(void);

void board_init_f(ulong dummy)
{
#ifndef CONFIG_SPL_TINY
	spl_early_init();
	timer_init();
	preloader_console_init();
	dram_init();
	aspeed_mmc_init();
#endif
}

#ifdef CONFIG_SPL_BOARD_INIT
void spl_board_init(void)
{
	struct udevice *dev;

	if (uclass_get_device_by_driver(UCLASS_MISC,
				DM_GET_DRIVER(aspeed_hace),
				&dev)) {
		debug("Warning: HACE initialization failure\n");
	}
}
#endif

u32 spl_boot_device(void)
{
#if IS_ENABLED(CONFIG_ASPEED_LOADERS)
	switch (aspeed_bootmode()) {
	case AST_BOOTMODE_EMMC:
		return (IS_ENABLED(CONFIG_ASPEED_SECURE_BOOT))?
			ASPEED_SECBOOT_DEVICE_MMC : ASPEED_BOOT_DEVICE_MMC;
	case AST_BOOTMODE_SPI:
		return (IS_ENABLED(CONFIG_ASPEED_SECURE_BOOT))?
			ASPEED_SECBOOT_DEVICE_RAM : ASPEED_BOOT_DEVICE_RAM;
	case AST_BOOTMODE_UART:
		return (IS_ENABLED(CONFIG_ASPEED_SECURE_BOOT))?
			ASPEED_SECBOOT_DEVICE_UART : ASPEED_BOOT_DEVICE_UART;
	default:
		break;
	}
#else
	switch (aspeed_bootmode()) {
	case AST_BOOTMODE_EMMC:
		return BOOT_DEVICE_MMC1;
	case AST_BOOTMODE_SPI:
		return BOOT_DEVICE_RAM;
	case AST_BOOTMODE_UART:
		return BOOT_DEVICE_UART;
	}
#endif

	return BOOT_DEVICE_NONE;
}

void board_boot_order(u32 *spl_boot_list)
{
	spl_boot_list[0] = spl_boot_device();
	spl_boot_list[1] = ASPEED_BOOT_DEVICE_UART;
}

#ifdef CONFIG_SPL_OS_BOOT
int spl_start_uboot(void)
{
	/* boot linux */
	return 0;
}
#endif

int board_fit_config_name_match(const char *name)
{
	/* we always use the default configuration */
	debug("%s: %s\n", __func__, name);
	return 0;
}

struct image_header *spl_get_load_buffer(ssize_t offset, size_t size)
{
	return (struct image_header *)(CONFIG_SYS_LOAD_ADDR);
}
