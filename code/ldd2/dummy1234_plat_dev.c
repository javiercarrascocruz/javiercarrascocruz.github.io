// SPDX-License-Identifier: GPL-2.0+
/*
 * UNBRANDED DUMMY1234 Device Generator
 *
 * Copyright (c) 2025, Hacker Bikepacker <hacker.bikepacker@gmail.com>
 */

#include <linux/device.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>

static struct platform_device *dummy1234_dev;
static const char *device_name = "dummy1234_device";

static int __init dummy1234_init(void)
{
	pr_info("%s: registering %s\n", __func__, device_name);

	dummy1234_dev = platform_device_register_simple(device_name, -1, NULL, 0);
	if (IS_ERR(dummy1234_dev)) {
		pr_err("failed to register %s\n", device_name);
		return PTR_ERR(dummy1234_dev);
	}

	return 0;
}

static void __exit dummy1234_exit(void)
{
	pr_info("%s: unregistering %s\n", __func__, device_name);

	platform_device_unregister(dummy1234_dev);
}

module_init(dummy1234_init);
module_exit(dummy1234_exit);

MODULE_AUTHOR("Hacker Bikepacker <hacker.bikepacker@gmail.com>");
MODULE_DESCRIPTION("DUMMY1234 DEVICE GENERATOR");
MODULE_LICENSE("GPL");
