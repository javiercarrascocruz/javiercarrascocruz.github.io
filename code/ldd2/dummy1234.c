// SPDX-License-Identifier: GPL-2.0+
/*
 * UNBRANDED DUMMY1234 ID Provider
 *
 * Copyright (c) 2025, Hacker Bikepacker <hacker.bikepacker@gmail.com>
 */

#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/module.h>

#define DUMMY1234_REG_ID 0x14

struct dummy1234_data {
	struct i2c_client *client;
	struct device *dev;
};

static void dummy1234_read_id(struct dummy1234_data *data)
{
	int ret;
	u8 reg;

	ret = i2c_master_recv(data->client, &reg, 1);
	if (ret < 0) {
		dev_err(data->dev, "failed to read ID\n");
		return;
	}
	dev_info(data->dev, "ID = 0x%02x\n", reg);
}

static int dummy1234_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct dummy1234_data *data;

	data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->client = client;
	data->dev = dev;

	dummy1234_read_id(data);

	return 0;
}

static const struct i2c_device_id dummy1234_id[] = {
	{ "dummy1234" },
	{ }
};
MODULE_DEVICE_TABLE(i2c, dummy1234_id);

static struct i2c_driver dummy1234_driver = {
	.driver = {
		.name = "dummy1234",
	},
	.probe = dummy1234_probe,
	.id_table = dummy1234_id,
};
module_i2c_driver(dummy1234_driver);

MODULE_AUTHOR("Hacker Bikepacker <hacker.bikepacker@gmail.com>");
MODULE_DESCRIPTION("DUMMY1234 ID provider");
MODULE_LICENSE("GPL");
