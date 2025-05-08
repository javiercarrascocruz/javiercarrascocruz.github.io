---
layout:     post
categories: tech
title:      Linux Drivers 2 - Basic layout and Device Model
date:       2025-01-23 18:00:00 +0100
summary:    General structure and key concepts
permalink:  /:title
image:      /images/posts/2025-01-23-ldd2-basics/ldd2-basics.webp
tags:       devicetree drivers linux soc i2c
---

In this episode we are going to actually write a device driver and interact with a device. What a milestone! And more importantly, we will clarify the modern Linux device model and the general structure of a device driver. And last, but not least, this first device driver will serve as a good base to build upon in the next episodes. For those who still don't have any target hardware, I have added an example that you can test on your computer and grasp some key concepts that will stay with us for the whole series.

For those who already have real hardware in their hands, we will first check that it is ready to be used. Then we will discuss the basic layout of a device driver in great detail to understand why things are the way they are, and finally we will take a look at some simple drivers for both I2C and platform devices. I hope I managed to mix theory and praxis for everyone to get something valuable out of this article!

I will assume that you have read the [first episode](/linux-drivers-hardware), and it would be great if you have ever compiled Linux, but not mandatory. Ready? Let's go...

---
<h2 class="content-heading">Content:</h2>

* TOC
{:toc}
---

### 0. Is the device alive?

First, you should make sure that the device is operational, or otherwise you will be looking for non-existent bugs in the code, and that will be highly time-consuming. Depending on the device, there are several approaches to make sure that it is properly powered and connected to the system. For example, your device may provide some indicators like LEDs connected to the power supply. That's nice to have in a prototype, but seldom the case unless you got a breakout board or evaluation kit, where they are rather common. Let's assume we have none of that.

For those who have some hardware background, the most reliable way is measuring continuity and voltage levels with proper equipment like a multimeter and/or an oscilloscope. The former is very cheap, and all tinkerers should have one. Just make sure that there are no short circuits, and then check that the connections are alright. Using colored wires will also help, as you will identify wrong connections much faster. If you can power on the device with a dedicated power supply before connecting it to the system, that's always a safer approach, especially if you limit the amount of current the supply delivers. After the previous checks (no short circuits and everything is connected as it should), you should be ready to switch on the system and measure the power supplies to make sure that they are stable and have the right levels. If they don't, the device might be demanding too much power, and you will have to reconsider your power subsystem. Having said that, if you start small i.e. with simple devices, that problem will be very rare.

If you don't know how to measure signals, that's (often) not the end of the world. An evaluation board is usually ready to be connected without setting fire to the entire system, and low-power devices will be seldom able to damage anything... maybe a GPIO/fuse/regulator if you connected something wrong 😆

Now let's assume that everything is properly powered and connected. Linux does not know anything about the device, does it? Yet there are tools that can tell you if there is *something* connected to the system. For example, I showed you how to detect I2C devices [in this article](/i2c-on-linux), and you will find other tools for different communication protocols. In that article I also mentioned another interesting option for serial buses, a logic analyzer, that is also a very powerful while debugging.

If your device should do something *visual* upon initialization (e.g. a display with backlight), you can simply connect the reset/enable pins to GPIOs and control them from userspace (very easy to do with most SoCs, trivial with Raspberry Pi). If you see some signs of life, you are on the right way!

And remember, if you see smoke, switch off the system immediately! It will be too late for some pieces of the system, but hopefully you just burnt some cheap regulator that you could replace without major issues. Adding fuses to your designs (many SoCs have them already) is always a wise idea, especially if the current consumption is expected to be high. The devices I am going to use are all low-power, and that won't be an issue, but keep it in mind for your particular case. And please: always be careful while working with electric systems. I have gotten a few electric shocks in my life, and although they were never severe, I don't miss getting another one ⚡

Great, now we are all set to focus on the software.

### 1. Driver layout

Let'see first divide a simple driver into general sections to get the picture:

#### 1.1. License

TL;DR; Boilerplate and one-liner: copy and paste at the top of the driver.

The license at the top of the file specifies the legal terms under which the code can be used, modified, and distributed. The SPDX-License-Identifier is a standardized format that declares the license of the file, making it machine-readable and ensuring consistency across projects. In this case, GPL-2.0+ means the code is licensed under the GNU General Public License version 2 or later, which is commonly used in open-source projects like the Linux kernel.

#### 1.2. Description

This section provides a brief overview of the functionality or purpose of the code. It often includes references to relevant documentation, such as datasheets or application notes, to help developers understand the hardware or software being supported. Additionally, it typically includes a copyright notice and contact information for the author or maintainer. It is not always provided, but it does not hurt, it is informative, and easy to implement 😅

#### 1.3. Headers

TL;DR; The headers you need, like in any C program. Apply the same rules you would apply to any C program.

A list of the header files necessary for the code to interact with specific kernel or library functions. These files may provide access to APIs for hardware communication, memory management, data structures, or other system resources that the code requires. There is a tendency to add too many headers (not too few because the kernel bots will catch that before the patch gets applied), often due to copying and pasting without paying much attention. Even if some required headers are nested in those that have already been added, sometimes you will see them explicitly added to this section. But as guards are used in all headers, that is not so critical, and as far as I can tell, very few people really cares.

As we are attempting to write clean code, I would recommend you to add only what you need and check if you are not including anything twice, because if we want to explicitly add all headers we need, we would have to go all the way down to the mos basic headers, and that would be silly.

#### 1.4. Definitions

This section defines constants, macros, and bit masks used throughout the code. These definitions simplify working with hardware registers and other low-level aspects by providing meaningful names for values and operations. It may also include other macros for manipulating specific bits or fields within a register, making the code more readable and maintainable.

#### 1.5. Global data

Often global structures that define key data models used throughout the code. These structures typically represent devices, configurations, or other entities the code interacts with. They may include fields for hardware-specific parameters, function pointers for initialization or handling tasks, and other necessary information. These structures help organize the code and provide a flexible way to extend functionality for different hardware or use cases.

#### 1.6. Functions

TL;DR; the core of the driver, where we bring the hardware to life.

The functions section contains the code that implements the driver’s behavior. Each function is responsible for a specific task, such as initializing hardware, handling interrupts, reading or writing data, and managing power state. Some of the most typical ones that you will find here are the **probe** function, that we will discuss soon, and the **resume/suspend** functions to manage power. What you will not find so often (at least not explicitly) in mainline Linux drivers, even though those are the only functions that 95% of the online sources talk about, are the **init** and **exit** functions. Keep on reading to know why.

These functions are typically `static` (if they are needed somewhere else, they should be moved to a subsystem/driver core and declared in a header file), and named to reflect their purpose, more ore less in the form "driver_name_purpose()". That pattern is useful for debugging purposes (i.e. unequivocal and easy to find) as well as to avoid conflicts if they are ever exported.

#### 1.7. Driver initialization

In this section, the driver is initialized by assigning key components such as the driver’s name, the probe function, and other necessary operations like power management functions. The initialization section also typically registers the driver with the kernel using structures like platform_driver or i2c_driver. These structures tell the kernel how to interact with the driver and specify which functions to call at various stages of the device lifecycle, like when the device is detected, initialized, or removed. Around this section you will usually find the **compatible** strings and the device-specific data assignments e.g. if the driver supports multiple devices with slight differences.

#### 1.8. Module macros

The macros section includes essential metadata like the author, description, and license of the driver. Common macros such as `MODULE_AUTHOR`, `MODULE_DESCRIPTION`, and `MODULE_LICENSE` provide important information about the driver, helping maintain consistency and visibility. These macros are also used by the kernel build system to include the module's metadata and ensure proper licensing and attribution. In some cases you will find macros like `MODULE_IMPORT_NS` and variants of that, which are used to include required namespaces. We will cover that in this series, but not in this episode to keep things manageable.

---

This pattern is not carved in stone, but as you'll see, it tends to repeat quite frequently, especially among new drivers. Sometimes more complex drivers are divided into multiple files (e.g. a core, per-bus-files, and a header file), and some of these sections will be spread across them, but in general, even in those cases the layout will be pretty similar.

### 2. Probe and remove functions? No init and exit?

Let's discuss a very common misconception among beginners before we carry on. If you have very basic experience with kernel modules, or you have followed trivial tutorials about *your first device driver*, you might be wondering why I am renaming the `init` and `exit` functions. The `init` function is called when the module is loaded, and the `exit` function when it is unloaded. Those `remove` and `probe` functions will be the same with some sanity checks on top, right? I have even talked to one person who had written two device drivers, and still believed that `init` and `probe` were equivalent. Well... they are not, I am afraid.

In the Linux kernel, traditional drivers often use `init` and `exit` functions to handle module loading and unloading, respectively. These functions are called when the driver is inserted into or removed from the kernel, and they are responsible for setting up or tearing down global resources, such as registering devices or subsystems. However, modern device drivers that integrate with the Linux device model primarily rely on probe and remove functions, which are tied to specific devices rather than the driver as a whole.

The probe function is called when the kernel matches a device to the driver, and it handles device-specific initialization, like allocating resources or initializing hardware. The remove function complements this by releasing those resources when the device is removed or the driver is unbound. While `init` and `exit` still exist and often handle driver-wide tasks (like registering with a bus subsystem or cleaning up global data), they now commonly delegate per-device initialization and cleanup to probe and remove. This separation allows for dynamic device management, hot-plugging, and better scalability in modern systems.

I will mention that there is some [official documentation](https://docs.kernel.org/driver-api/driver-model/overview.html) about this topic, but it is a bit scarce in details for beginners, and it then gets into function descriptions, so please keep on reading.

In summary, you will not find `__init` and `__exit` attributes in many modern drivers, only in subsystem cores, and in drivers that want to accomplish some common task that will affect all instances of the driver. For example, many devices use Cyclic Redundancy Check (CRC) to detect data corruption. If the polynomial used to build the CRC table is the same for all devices that the driver supports, why should every call to `probe()` generate a new CRC table? We could instead generate it once in the `init` function, and have it available as a global table for all instances. Is that only theory? No, let's see an example from the ad74115.c (an ADC/DAC under IIO):

```c
#define AD74115_CRC_POLYNOMIAL			0x7
DECLARE_CRC8_TABLE(ad74115_crc8_table);
//...
static int __init ad74115_register_driver(struct spi_driver *spi)
{
	crc8_populate_msb(ad74115_crc8_table, AD74115_CRC_POLYNOMIAL);

	return spi_register_driver(spi);
}
```

The `init` function just populates the CRC table, and then registers the driver. Easy! And as the global variable will be released automatically when the module gets unloaded, there's not even need for an explicit `exit` function. Will you never find `exit` functions out of subsystem core-level code and old drivers? Sometimes you will, if you have to explicitly release some resources you allocated in the `init` function.

As you can see, the driver is registered for a specific bus (SPI), as that is the Linux driver model, where drivers are bound to devices through a common bus they both support. The bus acts as an intermediary, matching drivers and devices based on compatibility. For instance, in this case, the SPI bus ensures the ad74413r driver is only loaded for devices that communicate using the SPI protocol. What if there is no bus? Then the driver uses the **platform bus**, a generic mechanism on Linux to handle devices that are not attached to a physical bus.

I know, this chapter had a little bit of theory, and you might still be a bit confused. If you did not get these concepts yet, don't worry, we will revisit them again later when I provide an example that you can test on your computer without additional hardware. But first, let's see an example of a simplified upstream driver, and how to build it.

### 3. Our first driver template

We know what a typical driver should look like, and we have understood why we will have a `probe` function, and maybe no `init` function. Then we should be ready to write our first, simple driver!

For our first driver, we are going to use a very simple layout with all the sections we discussed, but reduced to the bare minimum to be able to communicate with a real device. In this case, I will use the **Unbranded dummy1234**, a completely new device that communicates via I2C, that is only able to provide its ID, which is stored in an internal register.

Believe it or not, it just took 66 (including 13 empty lines!) lines of code to implement a Linux device driver able to communicate with real hardware over I2C. And that includes the license, a multi-line description, and even some dispensable pointers to show basic memory allocation and have a data struct ready to add more interesting stuff in the future.

The following snippet is the code of our first driver template with some comments inline. If you would like to copy the clean version without comments, you will find it [here](/code/ldd2/dummy1234.c).

```c
//---------------------------- 1. LICENSE -----------------------------------//
// SPDX-License-Identifier: GPL-2.0+
//---------------------------- 2. DESCRIPTION -------------------------------//
/*
 * UNBRANDED DUMMY1234 ID Provider
 *
 * Copyright (c) 2025, Hacker Bikepacker <hacker.bikepacker@gmail.com>
 */
//---------------------------- 3. HEADERs -----------------------------------//
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/module.h>

//---------------------------- 4. DEFINITIONS -------------------------------//
#define DUMMY1234_REG_ID 0x14

//---------------------------- 5. GLOBAL DATA -------------------------------//

//These pointers are not really necessary, but somehow convenient to only pass
//a pointer to this struct around.
struct dummy1234_data {
	struct i2c_client *client;
	struct device *dev;
};

//---------------------------- 6. FUNCTIONS ---------------------------------//
static void dummy1234_read_id(struct dummy1234_data *data)
{
	int ret;
	u8 reg;

	//We (master) receive 1 byte (the register) from the slave (dummy1234)
	ret = i2c_master_recv(data->client, &reg, 1);
	if (ret < 0) {
		//don't use printk(), and if possible, pr_() either.
		//dev_{dbg,info,notice,warn,err,alert,crit,emerg}() produce better logs
		//because they are bound to a device (data->dev)
		dev_err(data->dev, "failed to read ID\n");
		return;
	}
	dev_info(data->dev, "ID = 0x%02x\n", reg);
}

//Our entry point, the initialization is carried out here
static int dummy1234_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct dummy1234_data *data;

	//Allocate memory for the data struct. devm_ means "device managed",
	//and when the device is gone, the memory will be automatically released.
	//GFP_KERNEL is a GFP (Get Free Pages) flag for typical kernel-internal
	//allocation. It is the most common flag in such drivers.
	data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->client = client;
	data->dev = dev;

	dummy1234_read_id(data);

	return 0;
}

//---------------------------- 7. DRIVER INITIALIZATION ---------------------//

//This string will be used to match an i2c device with the driver. More info
//about this mechanism in my article "I2C on Linux".
//The empty element at the end is a guard to identify the end of the array.
static const struct i2c_device_id dummy1234_id[] = {
	{ "dummy1234" },
	{ }
};
MODULE_DEVICE_TABLE(i2c, dummy1234_id);

//This i2c driver is initiaized here: module name, probe, i2c, and i2c ID.
//We will initialize more members in the future ;)
static struct i2c_driver dummy1234_driver = {
	.driver = {
		.name = "dummy1234",
	},
	.probe = dummy1234_probe,
	.id_table = dummy1234_id,
};
module_i2c_driver(dummy1234_driver);

//---------------------------- 8. MODULE MACROS -----------------------------//
MODULE_AUTHOR("Hacker Bikepacker <hacker.bikepacker@gmail.com>");
MODULE_DESCRIPTION("DUMMY1234 ID provider");
//For simplicity GPL is always used even if it is GPL v2
MODULE_LICENSE("GPL");
```

### 4. Kconfig and Makefile

Forget it, we are not going to write our own `Makefile`, and we won't need `uname -r`. That's again something you usually need for out-of-tree code to compile against a certain kernel. For in-tree code like ours (you still don't know, but our code is indeed in-tree), there is already an infrastructure that we can (i.e. must) use.

In a nutshell, there is a hierarchical structure of Makefiles (i.e. building rules) that *Kbuild* (the kernel builder) follows to, depending on the kernel configuration (by means of the Kconfig files), include certain objects into the building process (built-in, or as a loadable module). We, driver developers, are more interested in the Kconfig and Makefile within the folder our driver resides. For example, if our driver will reside under `drivers/misc`, we will have to edit the Kconfig and Makefile under that path to add the building rule and the configuration option to include our driver. Given that `dummy1234` has a rather random functionality, we will actually add it to that subfolder.

Ok, so what do we have to do to make things work and build our driver? Usually not more than opening the two files, copying from an existing driver, pasting, and renaming. First, we will add the configuration option to `drivers/misc/Kconfig`: a relatively short entry with the following structure:

```none
config [MODULE]
	(usually) tristate "Title"
	depends on [MODULE] (one dependency per line)
	select [MODULE] (one dependency per line)
	default [condition] (only if there is a clear default)
	help
	  Say Y here if you want to build a driver for the [device]

	  To compile this driver as a module, choose M here: the
	  module will be called [module_name].
```

The fields are self-explanatory, yet there are some things to consider:
 - `tristate` means that the config accepts three possible states: not selected (n), module (m), and yes (y: built-in). Most drivers are `tristate` because they can be modules and built in, but there are more possibilities like `bool` (yes/no), `int`, or string. If they must be built-in in all cases, then you won't even find an entry in the `Kconfig`, because it will be configured like that in the `Makefile`.
 - `module_name` is the name we assigned when we initialized the driver, in our case `dummy1234`.

Our driver only depends on I2C to run, and it does not select any additional module. There is no special need for a default, and we do want to have a `tristate` config. Therefore, we can just add the following entry to the `Kconfig` in alphabetical order:

```none
config DUMMY1234
	tristate "DUMMY1234 ID provider"
	depends on I2C
	help
	  Say Y here if you want to build a driver for the Unbranded
	  Dummy1234 ID provider.

	  To compile this driver as a module, choose M here: the
	  module will be called dummy1234.
```

As you can see, the explanation was more heavy going than the pretty straightforward implementation 😂

What about the `Makefile`? Good news: in many cases, it is a single-liner. Just copy the rule from a different driver, rename the bit after `CONFIG_` to what you added after `config` in the `Kconfig`, and rename the object accordingly:

```makefile
obj-$(CONFIG_DUMMY1234)		+= dummy1234.o
```

After adding this line, we will be ready to configure our module: our `tristate` `CONFIG_DUMMY1234` can be either m (obj-m, build a module, what you usually see in a Makefile for out-of-tree modules), n (not included), or y (obj-y: built-in). That's all we need to know for now, but if you are interested in the build process and the elements involved, please read the [official documentation](https://docs.kernel.org/kbuild/makefiles.html) for more detailed information.

### 5. Building the module

We are all set to build the kernel! To keep things simple, we are going to build a very small kernel with minimal configuration that will build in a few seconds:

1. Set all configurations to **n**: `make allnoconfig`. We just want to build our driver as fast as possible!
2. Start menuconfig (old-school user interface): `make menuconfig`.
3. Select the only dependency we have, `I2C`. You can either navigate through the menus (`Device Drivers` → `I2C support --->` → `I2C support`), or simply look for it: press **/**, type `I2C` (or `i2c`, it is not case-sensitive) and press the number you see on the left. In this case, it will be 1 because it is the first entry on the list.
4. Select `DUMMY1234`. Feel free to try **m** (you will also have to select `MODULES` for that) and **y**.
5. **Save** and **Exit**.

The moment of truth:
```none
$ `make -j$(nproc)`
...
CC      drivers/misc/dummy1234.o
...
Kernel: arch/x86/boot/bzImage is ready
```

Happy days! You could also try `make M=drivers/misc/ clean && make C=1 -j$(nproc)` to clean the `misc/` directory and check the quality of your code with the `sparse` static analysis tool. Then you will see an additional line in the logs like `CHECK   drivers/misc/dummy1234.c`. Another useful tool is **smatch**, which I covered in [this article](/smatch).

### 5. Testing the driver

In order to cover all hardware combinations, or at least the most common ones, this section (which is an add-on, not really part of the scope of the article) would have to be way longer than the rest of the sections combined. Therefore, I will use this section to test the driver as it is in my simple setup with a Raspberry Pi, and I will give you some hints to build something similar for your particular case. Moreover, I wisely chose an I2C device for the example to offer much more detailed information thanks to [my article about I2C](/i2c-on-linux) that I previously mentioned, and that will be handy here as well.

The first thing we are going to do is build a proper kernel tailored for the target system. I have explained the process for a Raspberry Pi [here](/device-driver-development-with-rpi-setup), so I won't repeat myself. If you are using a different platform, you will need to find out how to build and load your own kernel for it. If you have a common platform, it will not be that difficult, and you will have to do it anyway for your budding career as a Linux developer 😉

Once you have built the kernel with the new little driver (built-in or as a module), you will be ready to test if it works. In my case, I have selected the module as built-in (**y**, **m** will work too), and I have connected an I2C device (it does not matter which one, and I decided not to advertise any specific brand if they don't support me) whose address is 0x29, and stores its ID in the register 0x14 (hence why I chose that number for the simple driver). According to the datasheet, 0x01 should be read from that register:

<figure>
    <center><img src="/images/posts/2025-01-23-ldd2-basics/id-reg.webp"
         alt="ID register"></center>
</figure>

Let's see what we get!

#### 5.1. Via sysfs attributes

If you know nothing about devicetrees and overlays, you should learn about those topics as soon as possible, and I have these two articles for you to read: [devicetree for Raspberry Pi](/device-driver-development-with-rpi-device-tree) and [dt-bindings](/dt-bindings). For now, we will tell the system about the device manually, via `sysfs` attributes.

For a detailed description of the following steps, please visit my article about I2C on Linux I mentioned before. Basically, I am checking that the driver gets assigned to the device, and that it reads the ID from the device. Of course, you could read any register as long as the device is ready to operate without previous configurations.

```sh
$ i2cdetect -y 1
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:                         -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- 29 -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --
pi@raspberrypi:~ $ dmesg | grep dummy1234
pi@raspberrypi:~ $ sudo bash -c "echo dummy1234 0x29 > /sys/bus/i2c/devices/i2c-1/new_device"
pi@raspberrypi:~ $ i2cdetect -y 1
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:                         -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- UU -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --
pi@raspberrypi:~ $ dmesg | grep dummy1234
[  117.016840] dummy1234 1-0029: ID = 0x01
[  117.016995] i2c i2c-1: new_device: Instantiated device dummy1234 at 0x29
```

Wonderful! But do we have to manually register the device every time? Obviously not, and the devicetree is the standard solution in embedded systems.

#### 5.2. Via devicetree (DT)

I already mentioned the documents you have to read first if you know nothing about this topic, so I will just show you the node you need to add to your DT under the I2C bus node for our simple device:

```
dummy1234: dummy1234@29 {
	compatible = "dummy1234";
	reg = <0x29>;
	status = "okay";
};
```
Use the right I2C address for your device, load your new DT, and the driver will be automatically matched with the device. Cool!

If you have some experience with DTs, you might be wondering why I did not add an `of_device_id` table to list the `compatible`, which in this case would be "unbranded,dummy1234". The answer is for simplicity. The `i2c_device_id` table will be used as a fallback, so it will work anyway, but a proper driver should have both tables, and if the manufacturer has not been added to the list yet, the series with the driver should first include a patch that adds it to avoid warnings when compiling the bindings. We will discuss such topics in another article.

#### 5.3. Suggestions for different hardware

If you do have a device to play with, and it does not have an I2C protocol, I would recommend you to go for the DT approach without thinking too much about other options. You will save time twice: first because you won't have to learn how to access the device from userspace, and then because eventually you will need a DT node anyway, either for production or just for ease of use. As I said before, one of your first tasks after bringing up your *SBC* should be learning how to load a new DT and its support for DT overlays.

For devices that use a different protocol like SPI, you will be able to adapt the simple driver by using SPI structures and functions, but the general structure will look very similar.If your device does not have a bus, the next section will help.

### 6. Revisiting init/exit vs probe/remove and platform devices

If you have made it to this section, you basically know how to program a Linux device driver, and you should be ready to add more complexity and functionality. I will help you with that in the next episodes, but before we come to an end, I would like to give you another simple example that you can try on your computer without any additional hardware. I hope it will help you understand better the difference between `init` and `probe`, `exit` and `remove`, and what a platform device is.

This time I have programmed an example with two kernel modules: a device generator, and a device driver. The former will make use of `init` and `exit` to register and unregister a platform device, and the latter will contain the `probe` and `remove` functions associated with a matched device (the one that the other module registered).

The device generator looks more like the kernel modules of the trivial examples you might have already seen: it has entry and exit functions, and some operations are carried out within them. But these operations do something more interesting than just calling `printk()`: they register and unregister a platform device. As this example is intended to be run with no additional hardware, a platform device is great because there is no need for a real bus to connect with, and we don't need to simulate hardware either.

But please, don't leap to the conclusion that *platform stuff* is only meant for legacy drivers, *misc* and *virtual* hardware. There are many modern devices (especially in embedded systems) that don't have access to a communication bus, and they only need a couple of signals (e.g. a reset signal) to operate. For example, some on-chip USB hubs partially fall into this category, and they require two different devices to work: a *platform* device, and a *USB* device. I have added support for such devices to the mainline kernel in the past, and we will discuss such cases in a future episode. Memory-mapped devices often fall into this category as well, and you will see that even your PC has multiple platform devices under `/sys/devices/platform/`. If you want to learn more about platform devices and drivers, take a look at the [official documentation](https://docs.kernel.org/driver-api/driver-model/platform.html).

Now let's get back to our example. This is the code of the platform device generator, which by now should be self-explanatory:

```c
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
```

The second module is basically the one we programmed for an I2C device, but adapted for a platform device. Key for the matching is the `device_id`, which is the name of the device the other module generates. It also includes a `remove` function to illustrate when it comes into action, and that will clearly show when it would be necessary:

```c
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
```

As you can see, most of the code is either boilerplate, or some renaming to work with a platform device. Easy!

I have created a simple `Makefile` as well, that you can use to play around without wasting any time:

```none
KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

obj-m := dummy1234.o dummy1234_dev.o

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean

load-dummy1234:
	sudo insmod dummy1234.ko

unload-dummy1234:
	sudo rmmod dummy1234

load-dummy1234-dev:
	sudo insmod dummy1234_dev.ko

unload-dummy1234-dev:
	sudo rmmod dummy1234_dev

.PHONY: all clean load-dummy1234 unload-dummy1234 load-dummy1234-dev unload-dummy1234-dev
```

These modules will be built against the kernel your machine is running. If you have not installed its kernel headers yet, that's a simple task:

`sudo apt install linux-headers-$(uname -r)`

The commands to build, load and unload the modules are also straightforward:
1. Compile both modules: `make`
2. Load device generator `make load-dummy1234-plat-dev`
3. Load device driver `make load-dummy1234-plat-drv`
4. Unload device generator `make unload-dummy1234-plat-dev`
5. Unload device driver `make unload-dummy1234-plat-drv`
6. Clean generated modules: `make clean`

Alright, let's see them in action! I will first compile both modules, then load the device driver, `dummy1234_plat_drv.ko` (.ko = kernel object), and then `dummy1234_plat_dev.ko`, also checking if the device really appears under `/sys/devices/platform/` with some attributes. Finally, I will unload the two modules in the inverse order I loaded them:

```sh
$ make
make -C /lib/modules/6.8.0-51-generic/build M=/home/jc/dummy1234 modules
make[1]: Entering directory '/usr/src/linux-headers-6.8.0-51-generic'
warning: the compiler differs from the one used to build the kernel
  The kernel was built by: x86_64-linux-gnu-gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
  You are using:           gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
  CC [M]  /home/jc/dummy1234/dummy1234_plat_drv.o
  CC [M]  /home/jc/dummy1234/dummy1234_plat_dev.o
  MODPOST /home/jc/dummy1234/Module.symvers
  CC [M]  /home/jc/dummy1234/dummy1234_plat_drv.mod.o
  LD [M]  /home/jc/dummy1234/dummy1234_plat_drv.ko
  BTF [M] /home/jc/dummy1234/dummy1234_plat_drv.ko
Skipping BTF generation for /home/jc/dummy1234/dummy1234_plat_drv.ko due to unavailability of vmlinux
  CC [M]  /home/jc/dummy1234/dummy1234_plat_dev.mod.o
  LD [M]  /home/jc/dummy1234/dummy1234_plat_dev.ko
  BTF [M] /home/jc/dummy1234/dummy1234_plat_dev.ko
Skipping BTF generation for /home/jc/dummy1234/dummy1234_plat_dev.ko due to unavailability of vmlinux
make[1]: Leaving directory '/usr/src/linux-headers-6.8.0-51-generic'

$ make load-dummy1234-plat-drv
sudo insmod dummy1234_plat_drv.ko

$ make load-dummy1234-plat-dev
sudo insmod dummy1234_plat_dev.ko

$ ll /sys/devices/platform/dummy1234_device/
drwxr-xr-x root root   0 B  Wed Jan 22 21:49:33 2025  .
drwxr-xr-x root root   0 B  Wed Jan 22 18:44:56 2025  ..
.rw-r--r-- root root 4.0 KB Wed Jan 22 21:49:53 2025  driver_override
.r--r--r-- root root 4.0 KB Wed Jan 22 21:49:53 2025  modalias
drwxr-xr-x root root   0 B  Wed Jan 22 21:49:53 2025  power
lrwxrwxrwx root root   0 B  Wed Jan 22 21:49:53 2025  subsystem ⇒ ../../../bus/platform
.rw-r--r-- root root 4.0 KB Wed Jan 22 21:49:53 2025  uevent

$ make unload-dummy1234-plat-dev
sudo rmmod dummy1234_plat_dev

$ make unload-dummy1234-plat-drv
sudo rmmod dummy1234_plat_drv
```

These are the messages you will see coming with `journalctl`:

```sh
$ journalctl -f | grep dummy1234
TTY=pts/4 ; PWD=/home/jc/dummy1234 ; USER=root ; COMMAND=/usr/sbin/insmod dummy1234_plat_drv.ko
TTY=pts/4 ; PWD=/home/jc/dummy1234 ; USER=root ; COMMAND=/usr/sbin/insmod dummy1234_plat_dev.ko
dummy1234_init: registering dummy1234_device
dummy1234_driver dummy1234_device: Calling dummy1234_probe :D
dummy1234_driver dummy1234_device: ID = 1234 (dummy)
TTY=pts/4 ; PWD=/home/jc/dummy1234 ; USER=root ; COMMAND=/usr/sbin/rmmod dummy1234_plat_dev
dummy1234_exit: unregistering dummy1234_device
dummy1234_driver dummy1234_device: Device removed!
TTY=pts/4 ; PWD=/home/jc/dummy1234 ; USER=root ; COMMAND=/usr/sbin/rmmod dummy1234_plat_drv
```

Everything worked as expected: the device driver was loaded, and the probe function was not called because there was no matching device yet. After creating the device (which could be found under `/sys/devices/platform/`), the probe function was triggered. Then, after unregistering the device (i.e. unloading the "device" module), the remove function within the device driver was triggered. Is it not great? You could have N devices, and the probe function would be triggered for every one of them, allocating per-device resources. That is the beauty of the Linux Device Model.

If you want to experiment with these modules, you can find them here to copy their code more easily: [dummy1234_plat_drv.c](code/ldd2/dummy1234_plat_drv.c), [dummy1234_plat_dev.c](code/ldd2/dummy1234_plat_dev.c), [Makefile](code/ldd2/Makefile).

---

This has been a pretty long article, but I wanted to settle the basics and key concepts for once and for all... until the device model in Linux changes 😆 If you have been able to follow this article until the end, you are definitely ready to write your own modern Linux device drivers, and you should be able to identify legacy drivers right away. From now on, I will focus on useful functionality for your drivers: power management, regmaps, interrupt handling, interactions with other subsystems, and much more. Stay tuned!
