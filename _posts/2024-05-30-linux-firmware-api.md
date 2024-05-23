---
layout:     post
title:      Linux Kernel Development - Firmware API
date:       2024-05-30 00:00:00
summary:    Making drivers load firmware for their devices
permalink:  /:title
tags:       contributor drivers kernel linux
---

The vast majority of devices supported by the Linux kernel require little to no intervention to start running: maybe powering up (e.g. via regulators), de-asserting a reset or accessing a few configuration registers. But a number of them are complex enough to require their own firmware to offer full (or custom) functionality. Some of such devices expect a pre-programmed persistent memory connected to them, but others provide some mechanism to receive their firmware from a software application. A few devices even offer both possibilities.

The first option is obviously simpler to develop: some extra hardware, and the firmware loading will just work. But extra hardware means extra costs, and that is often a key factor, let alone if you are designing an embedded device. Moreover, updating the attached memory with new versions might be more complex than updating the device itself.

On the other hand, programming a custom tool to load firmware for a specific device sounds wearisome as well as dangerous: accessing hardware from userspace, interfering with the device driver... and even if the manufacturer provides a firmware loader (seldom open source), it will not be easy to integrate with the rest of the system (including the device driver), especially if you are working with embedded systems.  There must be a better way to do it.

Fortunately, the kernel provides an API to integrate firmware updates into device drivers, offering update automation, fallback mechanisms, and even control via *sysfs*. No nasty hacks or closed-source tools required! In this article I will discuss how the Firmware API works, and how you can integrate it in your device driver development.

#### Content:

1. [How does it work?](#1-how-does-it-work)
2. [Real example: TPS6598x PD controller driver](#2-real-example-tps6598x-pd-controller-driver)
3. [Other things to consider](#3-other-things-to-consider)

---

<span style="font-size:0.85em;"><u>Note:</u> the [official documentation](https://www.kernel.org/doc/html/latest/driver-api/firmware/index.html) is a much more complete source. It covers many more uses cases (like [UEFI support](https://www.kernel.org/doc/html/latest/driver-api/firmware/efi/index.html)), but it might be a bit overwhelming if you know nothing about this magic. Here I just summarized the key points to grasp the mechanism at a glance and understand a real example with mainline kernel code.</span>

---

### 1. How does it work?

The basic mechanism is very simple:

#### 1.1. Fetch

First, we have to provide the file name. The name could simply be a hard-coded string in the driver code (several drivers do that), but there are other options. In the example we are going to see, the file name is a property in the devicetree. That kind of breaks the rule that the devicetree describes hardware, but it increases flexibility. Actually, there are other cases where the rule becomes blurry, like with the ubiquitous *wakeup-source* property. But that is not the topic we are covering here, and I did not make the decision anyway, so let's move on. To be honest, my first idea was using the devicetree, then hard-coding the name, and in the end someone sent the devicetree solution before I sent anything upstream :grin:

We will do that by means of the following call:

```c
request_firmware(&fw, name, dev);
```
where *name* is the string with the file name, and *dev* the device for which the firmware is being loaded (e.g. a *struct device \*dev* in a driver that represents the device, often embedded into a driver-specific *struct*).

And what is `fw`? The object that the API will populate (hence why it is passed by reference):

```c
const struct firmware *fw;

// from include/linux/firmware.h:
struct firmware {
	size_t size;
	const u8 *data;

	/* firmware loader private fields */
	void *priv;
};
```

As you can see, it is a very simple structure, and we only care about the data itself (const u8 \*data) and its size (size_t size).

We don't have to provide the file location, because the API defines where it should be stored:

* fw_path_para - module parameter - default is empty so this is ignored

* /lib/firmware/updates/UTS_RELEASE/

* /lib/firmware/updates/

* /lib/firmware/UTS_RELEASE/

* /lib/firmware/

Of course, you should have placed the binary there before, probably when you built your rootfs. Unless you have a good reason to pass the path as a parameter, the /lib/firmware paths are often preferred.

If the file is found in any of those locations (the list is ordered by priority, so the first match will stop the fetch), `fw` will be populated with the binary data and its size in bytes. We will use that information to implement the update mechanism.

---

<span style="font-size:0.85em;"><u>Risky tip:</u> you could also include the firmware in the Linux image (i.e. built-in). You will find how to do it [here](https://www.kernel.org/doc/html/latest/driver-api/firmware/built-in-fw.html), but be careful: you don't want to include proprietary firmware for legal reasons. Moreover, it has some other drawbacks like having to rebuild the kernel for a new firmware, but maybe it fits your needs (e.g. speed or early availability).</span>

---

#### 1.2. Data transmission

The update mechanism is strongly device-dependent, and the firmware API cannot know all mechanisms for all devices. Instead, the update implementation will be up to you, following the device application notes. You will have to send the firmware to the device over the supported protocol (typically, but not only, I2C and SPI). Don't worry, the update mechanisms are usually very simple, and no rocket science is required. Sometimes a single command to trigger a burst transmission or a simple loop to send multiple packets will be enough.

#### 1.3. Cleanup

Once we are done with the firmware update, we will not need `fw` anymore, so we can release the data with a simple call:

```c
release_firmware(fw);
```

### 2. Real example: TPS6598x PD controller driver

Let's put everything into action and <u>develop</u> some functionality for a device driver! This time we are going to play with a TPS65987 PD controller, whose driver I mentioned in [my last article](/extending-drivers) where we learned about device-specific data. This device requires the firmware to be loaded every time it gets powered up, and we will integrate the upload mechanism into the driver's probe function.

---

<span style="font-size:0.85em;">By the way, while I was writing that previous article I found that the data retrieval from this driver was suboptimal, and I sent [this simple patch](https://lore.kernel.org/linux-usb/ZjiVBkkdUAZ0B%2Fvb@kuha.fi.intel.com/) upstream to simplify it. Writing articles is fun, and sometimes even useful :grin: Actually I have contributed several times to this driver ([adding the reset signal](https://lore.kernel.org/linux-usb/20230912-topic-tps6598x_reset-v2-0-02a12e2ec50a@wolfvision.net/) or fixing a couple of bugs like [this one](https://lore.kernel.org/linux-usb/20240429-tps6598x_fix_event_handling-v3-1-4e8e58dce489@wolfvision.net/), ported to stable kernels too), and some more patches are getting ready for submission, but the relevant contribution that matters here is the [implementation of the firmware transmission](https://lore.kernel.org/linux-usb/20231207-tps6598x_update-v2-0-f3cfcde6d890@wolfvision.net/) for this device.</span>

---

We already know that the update mechanism is device specific, and we also know (from the previous article, check it out!) how to provide the driver with device-specific data and functions. Let's use our "deep knowledge" to fill the same steps we saw in the previous section, this time with real kernel code.

#### 2.1. Fetch

This part is common to any device the driver might support. The only difference might be the file name, which (as I mentioned before) is defined in the devicetree. Therefore, we can have a single wrapper to populate our firmware structure and make the data available for its transmission to the device. This wrapper only reads the file name, and calls `request_firmware()` like we did before:

```c
/*
 * We don't care much about the tps object, it's the top structure that contains
 * struct device *dev.
 */
static int tps_request_firmware(struct tps6598x *tps, const struct firmware **fw)
{
	const char *firmware_name;
	int ret;

	// Function to read the firmware name from the devicetree
	ret = device_property_read_string(tps->dev, "firmware-name",
					  &firmware_name);
	if (ret)
		return ret;

	ret = request_firmware(fw, firmware_name, tps->dev);
	if (ret) {
		dev_err(tps->dev, "failed to retrieve \"%s\"\n", firmware_name);
		return ret;
	}

	// To make sure that the file is not empty
	if ((*fw)->size == 0) {
		release_firmware(*fw);
		ret = -EINVAL;
	}

	return ret;
}
```

At this point, and if no errors occurred (file name not found in devicetree, file not found or empty), we should have *fw* ready for the next step.

#### 2.2 Data transmission

This is the device-specific part. The tps6598x driver currently supports firmware updates for two different devices: the tps25750 and the tps6598x. Although both communicate via I2C, their update mechanisms are completely different. Therefore, the cleanest solution is adding per-device update functions that work with the *fw* object to meet their unique requirements (again, check out my article about extending drivers to support new devices). Here I am going to show you the (relevant part of the) implementation for the tps6598x, which is the one I know better. You can then take a look at the implementation for the tps25750, which might be relevant for you if your device supports bursts (e.g. sending the whole firmware at once).

The tps6598x requires a "start" command to enter in patch mode, and a "complete" command to enter normal operation after the update. That is irrelevant for the data transmission, so I will simplify the real function a little bit. We will focus on what really matter for this article.

Data transfers are carried out by means of the "download" command ("PTCd"), which accepts up to 64 bytes of data. The firmware size for this device is usually around 16 KB, which means that several "download" commands will have to be sent. A bit tedious, but the device does not offer any other option apart from connecting an SPI flash memory. Alright, let's program a loop to send the data:

```c
static int tps6598x_apply_patch(struct tps6598x *tps)
{
	// variable declaration

	// the wrapper from the previous step
	ret = tps_request_firmware(tps, &fw, &firmware_name);
	if (ret)
		return ret;

	// "start" command: if it is ok, update can start

	/* ------- RELEVANT STUFF (simplified) ------- */
	bytes_left = fw->size;
	while (bytes_left) {
		in_len = min(bytes_left, TPS_MAX_LEN);
		ret = exec_cmd(tps, "PTCd", in_len, fw->data + copied_bytes);
		if (ret) {
			dev_err(tps->dev, "Patch download failed (%d)\n", ret);
			goto release_fw;
		}
		copied_bytes += in_len;
		bytes_left -= in_len;
	}
	/* ------------------------------------------ */

	// "complete" command: if it is ok, update succeeded

	dev_info(tps->dev, "Firmware update succeeded\n");

	// firmware release, next step ;)

	return ret;
};
```

The algorithm is very simple: send *fw->size* bytes in packets of 64 bytes from *fw->data* until there are no bytes left. If the last packet is smaller than 64 bytes, send whatever is left. If there is nothing left to send, happy days!

#### 2.3. Cleanup

No tricks or surprises, just `release_firmware(fw)` and a label to reach the release in the error paths (the *goto* from the previous step):

```c
release_fw:
	release_firmware(fw);
```

Done!

### 3. Other things to consider

If you have a candidate to profit from the kernel firmware API, read the device datasheet and application notes **very carefully** before you even start thinking about the implementation. Not only because the transfer mechanism might be very complex (that would be <u>more fun!</u>), but also because you might find several hardware-related challenges that could require a new hardware design. Typical examples are boot sequences and bootstrap pins to access the update mode (the tps6598x has such pins).

Another issue that you should keep in mind is timing. If the device driver tries to access a file from a filesystem that is still not available, it will fail. Why would that even happen? If your driver starts running before the filesystem with the firmware gets mounted, you will probably experience that. In such cases, you could consider either including the firmware file in your initramfs, or building the device driver as a module to store it in the same filesystem. Another option would be a [fallback mechanism](https://www.kernel.org/doc/html/latest/driver-api/firmware/fallback-mechanisms.html), which would provide a trigger via *sysfs*. There is even an additional API called [Firmware Upload API](https://docs.kernel.org/driver-api/firmware/fw_upload.html) to increase control over firmware updates via *sysfs* and monitor its status. I never used this API myself, but apparently you could use it to automate firmware updates from userspace. As I said, the API is much broader than what I can cover in a single article.

But not all points to consider are so negative/dangerous! The API is very well documented, and there are other functions that might fit your needs better than the simple `request_firmware()` we saw in this article (there are a bunch of alternatives to request firmware for different use cases, all documented [here](https://www.kernel.org/doc/html/latest/driver-api/firmware/request_firmware.html)), so you will probably find an accurate solution for your needs (maybe an asynchronous version?). Furthermore, you will find a lot of examples in different kernel subsystems (like bluetooth or net/wireless) to "get inspired" from.

The last thing I would like to mention should be obvious, but still: if you invest time to extend a driver in order to get firmware update support, share your work with the community. Apart from the typical reasons why you should do it (improvements, fixes, updates), relying on an out-of-tree solution can lead to surprises if you update the kernel, and the driver gets an update mechanism that does not comply with yours (like the hard-coded filename against the devicetree property). I have written [**a complete series**](/kernel-contributor-1) about how to become a Linux kernel contributor, no excuses! Be wise and collaborate with the community :wink:

 <br/><br/>
Enjoy and share knowledge!
