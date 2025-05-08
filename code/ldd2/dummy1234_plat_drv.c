// SPDX-License-Identifier: GPL-2.0+
/*
 * UNBRANDED DUMMY1234 ID Provider
 *
 * Copyright (c) 2025, Hacker Bikepacker <hacker.bikepacker@gmail.com>
 */

#include <linux/device.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>

struct dummy1234_data {
	struct device *dev;
};

static void dummy1234_read_id(struct dummy1234_data *data)
{
	dev_info(data->dev, "ID = 1234 (dummy)\n");
}

static int dummy1234_probe(struct platform_device *pdev)
{
	struct dummy1234_data *data;

	dev_info(&pdev->dev, "Calling %s :D\n", __func__);

	data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->dev = &pdev->dev;

	dummy1234_read_id(data);

	return 0;
}

static int dummy1234_remove(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "Device removed!\n");

	return 0;
}

static const struct platform_device_id dummy1234_id_table[] = {
	{ "dummy1234_device" },
	{ }
};

MODULE_DEVICE_TABLE(platform, dummy1234_id_table);


static struct platform_driver dummy1234_driver = {
	.driver = {
		.name = "dummy1234_driver",
	},
	.probe = dummy1234_probe,
	.remove = dummy1234_remove,
	.id_table = dummy1234_id_table,
};

module_platform_driver(dummy1234_driver);

MODULE_AUTHOR("Hacker Bikepacker <hacker.bikepacker@gmail.com>");
MODULE_DESCRIPTION("DUMMY1234 ID provider");
MODULE_LICENSE("GPL");
