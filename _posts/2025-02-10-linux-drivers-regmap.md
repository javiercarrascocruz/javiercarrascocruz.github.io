---
layout:     post
categories: tech
title:      Linux Drivers 3 - Regmap in Detail
date:       2025-02-10 18:00:00 +0100
summary:    Standard API to access registers with very cool features
permalink:  /:title
image:      /images/posts/2025-02-10-ldd3-regmap/ldd3-regmap.webp
tags:       drivers linux soc
---

This third episode brings up a key feature for all Linux device drivers that read and write registers, either memory mapped or through a communication protocol. As you can imagine, that is something that a huge amount of device drivers need to do, and having a standard interface is more than welcome. Today, we are going to discuss that standard interface: regmap.

I will assume that you have read the [first](/linux-drivers-hardware) and [second](/linux-drivers-layout-and-device-model) episodes of this series, and you will need a basic driver that accesses to registers like the one I provided in the second episode, which I will use here to extend its functionality.

---
<h2 class="content-heading">Content:</h2>

* TOC
{:toc}
---

### 1. What is regmap, and why do I need it?

In the previous episode we managed provide our driver with I2C communication capabilities, and we were even able to read data from one device register. Not only that, the solution was very easy and intuitive. Are we not good to go? For a temporary solution, of course we are, but it has a number of drawbacks:

- **Repetitive code** – Each register access requires manually crafting write-read sequences, leading to duplication and potential inconsistencies.

- **Lack of abstraction** – There is no built-in support for handling endianness, register sizes, or multibyte operations, making the code more error-prone.

- **Error handling limitations** – The approach lacks automatic retries, structured logging, and standardized error handling, making failures harder to debug and recover from.

- **Missing additional functionalities** – Regmap provides extra features like register caching, and bit-wise operations, which will discuss soon.

Fortunately, regmap is the standard solution to all of them. Regmap is basically a level of abstraction on top of the direct register access and a series of features to optimize those accesses, increase reliability, readability, and scalability. Does it not sound like something you should be using? 😉

### 2. Regmap basic initialization

Now that we are done with the preface, it's time to start coding. In its simplest form, regmap requires very little configuration: a header and an instance of the `regmap_config` structure.

The header you have to add is `regmap.h` under `inclues/linux`:

```c
#include <linux/regmap.h>
```

The minimum struct initialization will consist of the following elements:
- `.reg_bits`: number of bits in the register address.
- `.val_bits`: number of bits in the register value.

For example, if we need 32-bit addresses to access the device registers, and they are 16 bits long, we will initialize these values like this:

```c
static const struct regmap_config dummy1234_regmap_config = {
	.reg_bits = 32,
	.val_bits = 16,
};
```

Although it is not mandatory, a name is almost always provided, and also the highest register address we can access:
- `.name`: usually `drivername` or `drivername_regmap`.
- `.max_register`: assign the last valid register address of your device.

That was easy, wasn't it? But we must have some means to tell what communication protocol the driver is going to use, right? Sure, but we are not going to do it at the structure level. Instead, we will use a specific regmap initialization function for the given protocol with the initialized struct as a parameter.

Let's see an example with our dummy1234 driver from the previous episode. The device registers can be addressed with 8 bits, and they are also 8 bits long. We also know that its communication protocol is I2C, so we already have the bare-minimum information to initialize regmap.

```c
#include <linux/regmap.h>

static const struct regmap_config dummy1234_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
};

static int dummy1234_probe(static struct i2c_client* client)
{
	strut regmap *regmap;

	regmap = devm_regmap_init_i2c(client, &dummy1234_regmap_config);
	//...
}
```

That's it! We have created a regmap configuration that could be used by any communication protocol: we would only need to use a different init function for each of them, as the registers are unique to the device, and they do not depend on the communication protocol. Moreover, that init function will be the only one that will depend on the communication protocol. The beauty of regmap is starting to shine 🌟

The driver is ready to use regmap. But how?

### 3. Register access: single, bulk, and multi-register

Our `dummy1234` driver was able to read the device ID from a register via I2C, but it used I2C-specific functions to do so like `i2c_master_write()` and `i2c_master_recv()`. Now, we are going to use regamp, and a single access to read a register value will be as easy as this (note the `&`):

```c
ret = regmap_read(regmap, REGISTER_ADDRESS, &register_value);
```

Obviously, `register_value` will store the read value if nothing goes wrong. As you can see, that call is protocol-agnostic, and you could use it everywhere e.g. you could have the same device with two different communication protocols, and only the initialization would differ. That's in fact a common practice for such devices.
How would we write some value to a register? Easy:

```c
ret = regmap_write(regmap, REGISTER_ADDRESS, regiser_value);
```

Those two functions would be enough to get things done by means of **single** accesses, but if we want to access multiple registers at once, there is a better approach than a succession of single operations: **bulk** operations.

Bulk operations work well when we need to access a contiguous range of registers, which is indeed a very common task in device drivers. With regmap, bulk read/write operations look very similar to what we just saw for single accesses, and we will only need to pass arrays to account for the multiple registers:

```c
ret = regmap_bulk_read(regmap, REGISTER_ADDRESS, array, sizeof(array));
ret = regmap_bulk_write(regmap, REGISTER_ADDRESS, array, sizeof(array));
```

If you want to access registers that are not contiguous, there is a solution for your use case as well thanks to `regmap_multi_reg_read()` and `regmap_multi_reg_write()`. The read operation requires two arrays (one for the register addresses and one for the data to be read), and the total amount of registers being accessed. Let's see an example with 8-bit registers:
```c
#define DUMMY1234_REG_STATUS1   0x10
#define DUMMY1234_REG_STATUS2   0x28

unsigned int reg_address[2] = {DUMMY1234_REG_STATUS1, DUMMY1234_REG_STATUS2};
u8 rdata[2];

ret = regmap_multi_reg_read(regmap, reg_address, rdata, sizeof(rdata));
```

In order to use `regmap_multi_reg_write()`, we will need a new structure called `reg_sequence`:

```c
/**
 * struct reg_sequence - An individual write from a sequence of writes.
 *
 * @reg: Register address.
 * @def: Register value.
 * @delay_us: Delay to be applied after the register write in microseconds
 *
 * Register/value pairs for sequences of writes with an optional delay in
 * microseconds to be applied after each write.
 */
struct reg_sequence {
	unsigned int reg;
	unsigned int def;
	unsigned int delay_us;
};
```

A lot of text, but in the end that's nothing more that a list of register addresses and values, with an optional delay after the write operation (it can be useful for example for devices that need some delay between two write operations). Let's see how to use that with a simple example:

```c
#define DUMMY1234_REG_STATUS1   0x10
#define DUMMY1234_REG_STATUS2   0x28

//You can omit the delay if it is not required
struct reg_sequence reg_seq[2] = {
	{ DUMMY1234_REG_STATUS1, 0x12 },
	{ DUMMY1234_REG_STATUS2, 0x34 },
};

ret = regmap_multi_reg_write(regmap, reg_seq, ARRAY_SIZE(reg_seq));
```

Try not to abuse of multi-reg operations, and use them only where it really makes sense. As you will see in the mainline kernel, single and bulk operations are more common because they address more common driver needs. For example, at the moment of writing only the *hwmon* subsystem uses `regmap_multi_reg_read()`, and not very often. You will seldom have to read from scattered registers at once, but who knows, maybe you will... and then you will know how to do it. `regmap_multi_reg_write()` is way more common because it covers common cases like writing configurations that are spread across scattered registers.

### 4. Bit-level access

What about bit-wise operations? Sure! If you want to write/update bits from a given register, you can use the following functions:
```c
ret = regmap_write_bits(regmap, REGISTER_ADDRESS, bitmask, value);
ret = regmap_update_bits(regmap, REGISTER_ADDRESS, bitmask, value);
```

The difference between those two functions can be inferred by their names: `regmap_write_bits()` directly writes the result of `bitmask & value` into the register, and `regmap_update_bits()` first reads the register, and updates its value only if `bitmask & value` and the current value differ. The first one will save you from reading the register, which is especially useful if you don't care about its current value, whereas the second one could save you from the write operation if the register value did not change. In the end, both functions are wrappers around the same `regmap_update_bits_base()` with different flags to force or not the write operation.

There are some other wrappers around `regmap_update_bits_base()` to cover more use cases. As an example, `regmap_update_bits_check()` works like `regmap_update_bits()`, but it also requires a reference to a boolean value to notify whether the function call modified the register value or not.

### 5. Regfields

Another common use case: you are only interested in some fields of a register, and you don't really care about the rest. The datasheet gives you values for that field from 0 to X, and you want to replicate that. You could use regular read/write operations and then obtain the region of interest with bit masks and some shifting. That is cumbersome and error-prone. There is a cleaner approach, though: regfields.

Using regfields is as easy as you are probably expecting, but first we have to define the regfields we are interested in and allocate them to be able to work with the references that regmap will return. That's easier than it sounds, don't worry.

Let's see a simple example, where we are going to define a 16-bit configuration register that stores the device power state in the bits [4...6]. First, let's add a new member to our `data` structure to store the references to this regfield. We will probably require more regfields in the near future, so let's embed them in a dedicated structure:

```c
#define DUMMY1234_REG_STATUS 0x01

struct dummy1234_rf {
	struct regmap_field *ps;
};

struct dummy1234_data {
	//...
	struct dummy1243_rf rf;
};

```

```c
//regfield definition
static const struct reg_field dummy1234_rf_ps =
	REG_FIELD(DUMMY1234_REG_STATUS, 4, 6);

//call this function from your probe!
static int dummy1234_rf_init(struct dummy1234_data *data)
{
	//references to work with shorter names
	struct regmap *regmap = data->regmap;
	struct device *dev = data->dev;
	struct dummy_rf *rf = &data->rf;

	//the regfield allocation
	rf->ps = devm_regmap_field_alloc(dev, regmap, dummy1234_rf_ps);
	if (IS_ERR(rf->ps))
		return PTR_ERR(rf->ps);

	//repeat the pattern for all your regfields

	return 0;
}
```

That was it. Now we can access the regfield without any tweaks by calling the following functions:

```c
ret = regmap_field_read(data->rf.ps, &value);
ret = regmap_field_write(data->rf.ps, value);
```

Those two are by far the most common regfield operations. There is also a series of functions which combine regfields and bit accesses, this time based on `regmap_field_update_bits_base()` with different parameters. In general, they are not widely used, but `regmap_field_update_bits()` is one that a few drivers use, which works like the `regmap_update_bits()` function we saw before, but limited to the regfield of interest:

```c
//I will keep on using the regfield we defined. In general, the first
//parameter in the next functions will be struct regmap_field *field
ret = regmap_field_update_bits(data->rf.ps, bitmask, value);
```

And for direct write operations, `regmap_field_clear_bits()` and `regmap_field_set_bits()` can be used:

```c
ret = regmap_field_clear_bits(data->rf.ps, bits);
ret = regmap_field_set_bits(data->rf.ps, bits);
```

At the moment of writing, there is a single user of these two functions, which gives you an idea of how often you will need them. And for the sake of completeness, let's see another weirdo:

```c
ret = regmap_field_test_bits(data->rf.ps, bits);
```

This function will check if the specified bits are set in a register field, returning 0 if at least one of the tested bits is not set, and 1 if all tested bits are set. This description has been copied from `regmap.c` for a simple reason: there are no users of the function. That's in principle dead code, but as I don't know if someone is using it for current discussions, I will not send a patch to remove it. Maybe you will need it soon!

It might be worth mentioning that there are also `regmap_fields_` functions. That is not a typo, just an unfortunate way to hint that these functions accept a *port ID* parameter. What for? If your device has multiple ports that can be accessed separately, and they have the same regfield pattern, you could define your regfields once and then use the `id` to select the port to be accessed. Apart from that, these functions work like their `regmap_field_` counterparts.

```c
ret = regmap_fields_read(field, id, &val);
ret = regmap_fields_write(field, id, val);
```

I will finish this section with the `_force_` variants of some of the functions we have discussed. The only difference is that they will *force* the operation into the hardware, ignoring if the register or the cache already have the same value. That's for example useful if writing into the register triggers some action in the hardware. If that's not your case, there is no need to *force* anything.

```c
ret = regmap_field_force_update_bits(field, mask, val);
ret = regmap_fields_force_write(field, id, val);
//...
```

### 6. Register ranges

With regmap you can define ranges of register addresses and their access rights like read-only, write-only, r/w, etc. This feature by itself might not look that powerful, although it will help if some operation attempts to access a register that does not allow that kind of access. Nevertheless, it will give us access to some cool stuff later. Alright, let's define some register ranges! First, I will define some registers that will have different access permissions:

```c
#define DUMMY1234_REG_CONF   0x00 //read/write
#define DUMMY1234_REG_STATUS 0x01 //read-only
#define DUMMY1234_REG_TEMP   0x02 //read-only
#define DUMMY1234_REG_INT1   0x06 //read-only
#define DUMMY1234_REG_INT2   0x07 //write-only
#define DUMMY1234_REG_ID     0x09 //read-only
#define DUMMY1234_REG_UID    0x0A //read/write
```

In order to define the ranges based on the given register addresses, we will have to initialize two types of structures: `regmap_range` to describe the ranges, and `regmap_access_table` to indicate the access permissions. Both structures are pretty simple, and the official documentation describes their fields as follows:

```c
/**
 * struct regmap_range - A register range, used for access related checks
 *                       (readable/writeable/volatile/precious checks)
 *
 * @range_min: address of first register
 * @range_max: address of last register
 */
struct regmap_range {
	unsigned int range_min;
	unsigned int range_max;
};

/**
 * struct regmap_access_table - A table of register ranges for access checks
 *
 * @yes_ranges : pointer to an array of regmap ranges used as "yes ranges"
 * @n_yes_ranges: size of the above array
 * @no_ranges: pointer to an array of regmap ranges used as "no ranges"
 * @n_no_ranges: size of the above array
 *
 * A table of ranges including some yes ranges and some no ranges.
 * If a register belongs to a no_range, the corresponding check function
 * will return false. If a register belongs to a yes range, the corresponding
 * check function will return true. "no_ranges" are searched first.
 */
struct regmap_access_table {
	const struct regmap_range *yes_ranges;
	unsigned int n_yes_ranges;
	const struct regmap_range *no_ranges;
	unsigned int n_no_ranges;
};
```

The only thing that might be a bit confusing is that combination of "yes" and "no" ranges. In a nutshell, you are expected to provide access tables for your access operations like read and write, and you can use the "yes" and "no" attributes to provide a straight or an inverted logic. For example, you could have a single write-only register, and in that case you could instantiate a write-only access table with that register as "no", and then assign the table to the `rd_table` element within the `regmap_config` structure. That will mean "this is the only register that can't be read". If you still don't get it, continue reading, and I will show you an example a bit later.

Let's continue with our example. First, we will define the ranges. For that, we will use the `regmap_reg_range` macro for every read range. If you take a look at the register definitions, you will realize that there are holes i.e. not all addresses are defined, and those holes will act as range delimiters:

```c
static const struct regmap_range dummy1234_read_ranges[] = {
	regmap_reg_range(DUMMY1234_REG_CONF, DUMMY1234_REG_TEMP),
	regmap_reg_range(DUMMY1234_REG_INT1, DUMMY1234_REG_INT2),
	regmap_reg_range(DUMMY1234_REG_ID, DUMMY1234_REG_UID),
};

static const struct regmap_range dummy1234_write_ranges[] = {
	regmap_reg_range(DUMMY1234_REG_CONF, DUMMY1234_REG_CONF),
	regmap_reg_range(DUMMY1234_REG_INT2, DUMMY1234_REG_INT2),
	regmap_reg_range(DUMMY1234_REG_UID, DUMMY1234_REG_UID),
};
```

Now we can use our ranges to define the access tables and assign them to `regmap_config`:

```c
static const struct regmap_access_table dummy1234_read_table = {
	.yes_ranges = dummy1234_read_ranges,
	.n_yes_ranges = ARRAY_SIZE(dummy1234_read_ranges),
};

static const struct regmap_access_table dummy1234_write_table = {
	.yes_ranges = dummy1234_write_ranges,
	.n_yes_ranges = ARRAY_SIZE(dummy1234_write_ranges),
};

static const struct regmap_config dummy1234_regmap = {
	//...
	.rd_table = &dummy1234_read_table,
	.wr_table = &dummy1234_write_table,
	//...
};
```

Now let's imagine that we have the following set of registers:

```c
#define DUMMY1234_REG_CONF   0x00 //read/write
#define DUMMY1234_REG_STATUS 0x01 //read/write
#define DUMMY1234_REG_TEMP   0x02 //read/write
#define DUMMY1234_REG_INT1   0x03 //read/write
#define DUMMY1234_REG_INT2   0x04 //write-only
#define DUMMY1234_REG_ID     0x05 //read/write
```

In this case we could opt for the "no" approach:

```c
static const struct regmap_range dummy1234_write_only_ranges[] = {
	regmap_reg_range(DUMMY1234_REG_INT2, DUMMY1234_REG_INT2),
}`;

static const struct regmap_range dummy1234_write_ranges[] = {
	regmap_reg_range(DUMMY1234_REG_CONF, DUMMY1234_REG_ID),
};

static const struct regmap_access_table dummy1234_read_table = {
	.no_ranges = dummy1234_write_only_ranges,
	.n_no_ranges = ARRAY_SIZE(dummy1234_write_only_ranges),
};

static const struct regmap_access_table dummy1234_write_table = {
	.yes_ranges = dummy1234_write_ranges,
	.n_yes_ranges = ARRAY_SIZE(dummy1234_write_ranges),
};
```

The assignments to `regmap_config` don't change because we are still assigning a read and a write table, just with a modified logic:

```c
static const struct regmap_config dummy1234_regmap = {
	//...
	//DUMMY1234_REG_INT2 is the only "no" read register
	.rd_table = &dummy1234_read_table,
	.wd_table = &dummy1234_write_table,
	//...
};
```

If you still think that the read range is incomplete because no `yes_range` was defined, I am glad that you don't believe everything I say. The `regmap_check_range_table()` function in `drivers/base/regmap/regmap.c` is in charge of that behavior:

```c
bool regmap_check_range_table(struct regmap *map, unsigned int reg,
			      const struct regmap_access_table *table)
{
	/* Check "no ranges" first */
	if (regmap_reg_in_ranges(reg, table->no_ranges, table->n_no_ranges))
		return false;

	/* In case zero "yes ranges" are supplied, any reg is OK */ <-----READ THIS!
	if (!table->n_yes_ranges)
		return true;

	return regmap_reg_in_ranges(reg, table->yes_ranges,
				    table->n_yes_ranges);
}
```
If you don't define any "yes" range, then all accesses for the given operation will be allowed. But if you define a "yes" range, that will not be true anymore, and you will have to provide all registers that allow that access operation.

Keep in mind that the "yes" ranges are way more common than the "no" ones, partially because a lot of hardware does not need to exclude ranges, but also because it is a bit more confusing, and it is easy to get it wrong. I guess that many people provide the "yes" ranges and move on. Anyway, you should be able to handle both scenarios. Who knows if your first driver will be better off with "no" ranges!

The `regmap_range` structure is not only used for read/write operations, and we are going to see another use case in a moment.

Before finishing this section, I would like to mention that using the structures we just saw is not the only way to define ranges. It is also possible to provide *callbacks* to check the access rights of the registers:

```c
	bool (*writeable_reg)(struct device *dev, unsigned int reg);
	bool (*readable_reg)(struct device *dev, unsigned int reg);
```

For this approach, you will have to declare the callbacks, assign them to `writeable_reg` or `readable_reg` (also *volatile* and *precious* are possible, we will talk about these access rights later) in `regmap_config`, and program the logic to decide if a register can be accessed that way (often with a `switch()` or something similar). This way is less elegant than the approach we discussed, and I am not a big fan of it. I believe it existed before the range and access structs were introduced, which are currently preferred for most of the cases. But as you will find several drivers that use this approach, it is good that you know what it is about.

### 7. Caching

We are now able to access registers like a pro, but do we always have to *access* the registers? Many of them will keep their value until the driver updates it, so why would we trigger a time-consuming operation like accessing a slow bus when the value has not been modified? Let's introduce caching to keep on accessing registers like a pro, but only when we have to.
In order to support caching, we will need more regmap ranges to specify what registers are updated by the device itself. We will be able to cache registers that are only modified by the driver, because their values should be the last ones the driver wrote. On the other hand, we should not cache registers that the device can update (**volatile**) because that would invalidate the cache, and we would not notice. Let's take a look at this new set of registers:

```c
#define DUMMY1234_REG_CONF   0x00 //read/write, not updated by the device
#define DUMMY1234_REG_STATUS 0x01 //read-only, updated by the device
#define DUMMY1234_REG_TEMP   0x02 //read-only, updated by the device
#define DUMMY1234_REG_INT    0x03 //read-write, updated by the device
#define DUMMY1234_REG_ID     0x04 //read-only, constant value
```
The descriptions simulate the information you would get from the datasheet about those registers. Thanks to that information, we can define some new ranges for the cacheable, volatile, and precious registers. Can you tell to what category belongs every register I defined? If you still can't, let's use some logic:

- `DUMMY1234_REG_CONF`: if we can read and write its value, and the device does not update it, the driver has full control. Therefore, this register should be cacheable.
- `DUMMY1234_REG_STATUS`, `DUMMY1234_REG_TEMP`: those registers are updated by the device, and therefore we should not cache them. They are volatile, and we will need a fresh value for every read operation.
- `DUMMY1234_REG_INT`:
- `DUMMY1234_REG_ID`: constant value, we definitely want to cache this and avoid additional accesses to the hardware over a potentially slow bus.

We are now ready to describe the new range and access table:

```c
static const struct regmap_range dummy1234_volatile_ranges[] = {
	regmap_reg_range(DUMMY1234_REG_STATUS, DUMMY1234_REG_INT),
};

static const struct regmap_access_table dummy1234_volatile_table = {
	.yes_ranges = dummy1234_volatile_ranges,
	.n_yes_ranges = ARRAY_SIZE(dummy1234_volatile_ranges),
};
```

We are almost ready to update the `regmap_config` initialization to support caching. Almost? Yes, we still have to define the **cache type**. Regmap supports different cache types, enumerated in `regmap.h` as follows:

```c
/*
 * The supported cache types, the default is no cache.  Any new caches
 * should usually use the maple tree cache unless they specifically
 * require that there are never any allocations at runtime and can't
 * provide defaults in which case they should use the flat cache.  The
 * rbtree cache *may* have some performance advantage for very low end
 * systems that make heavy use of cache syncs but is mainly legacy.
 */
enum regcache_type {
	REGCACHE_NONE,
	REGCACHE_RBTREE,
	REGCACHE_FLAT,
	REGCACHE_MAPLE,
};
```

`REGCACHE_NONE` is the default cache type i.e. by default, caching is disabled (you don't need to add `.cache_type = REGCACHE_NONE`). Usually, you will just follow the description above and use `REGCACH_MAPLE` unless you have a good reason to use another type.

All things considered, our `regmap_config` should look like this:

```c
static const struct regmap_config dummy1234_regmap_config = {
	//...
	.volatile_table = &dummy1234_volatile_table,
	.precious_table = &dummy1234_precious_table,
	.cache_type = REGCACHE_MAPLE,
	//...
};
```

We are ready to move on, but we could also go the extra mile and discuss cache synchronization. This is not a topic that you will see in most of the drivers that use regmap, yet you might stumble upon it from time to time.

#### 7.1. Cache synchronization/bypassing

There are a number of reasons why the regmap cache might require to be synchronized like after returning from a low-power mode or a device reset if those conditions flush the register values. For example, I have seen some `hwmon` devices that need to be re-initialized after a reset, which invalidates the cache. You can find an interesting real case in `drivers/hwmon/ina2xxx.c`.

These considerations usually depend on the hardware and the nature of the registers, and it is not something that most drivers will have to consider. But if yours does, you will probably make use of the following functions:

- `regcache_sync(regmap)`: the function to synchronize the cache. Obviously, it will ignore volatile registers as they are not cached.
- `regcache_mark_dirty(regmap)`: indicate that the device registers have been reset to their default values. This function is usually combined with `regcache_sync()` to let it know that it will have to update the non-default values from the cache to the device.
- `regmap_read_bypassed(regmap, REGISTER_ADDRESS, &register_value)`: to read directly from the device, bypassing the cache. Although it is a very uncommon operation, sometimes it's useful to access a specific register that might indicate what to do after a reset or a similar event that might have invalidated the cache. If you can avoid using this function and implement a more common approach, please do so. If there is no way around it, consider adding a comment to justify its need in your driver. Otherwise, its usage will be seen as rather suspicious.
- `regcache_cache_only(regmap, bool enable)`: if enabled, no hardware access will be carried out by regmap, restricting itself to the cache. This could be useful for register accesses while the hardware is not accessible. The cache will then have to be synchronized when the hardware is accessible again, also setting `regcache_cache_only()` back to false.

### 8 Precious registers

There is yet a third scenario that must be observed when describing the device registers: **precious** registers whose value is automatically updated after reading it, and need special care to avoid *unplanned* reads. What are unplanned reads? Those that were not requested by the driver. Still, when could that happen? Are we not programming a driver to be in charge of the device? Yes, more or less... but as we will see later, it is possible to bypass the driver and dump (i.e. read) the register values.

A typical example of such precious registers are event/interrupt notifiers that are cleared upon read (to be ready for the next event/interrupt). If someone else reads that register, it will be cleared and the notification will be lost. That's definitely not what we want as driver developers! Fortunately, we can fix it with the same approach we have been following: `regmap_range` + `regmap_access_table` + `regmap_config` update. In the following example, `DUMMY1234_REG_EVENTS` is used by the driver to notify events (e.g. interrupts, overflows), and reading its value clears it:

```c
#define DUMMY1234_REG_EVENTS    0x0B //read-only, clear on read

static const struct regmap_range dummy1234_precious_ranges[] = {
	regmap_reg_range(DUMMY1234_REG_EVENTS, DUMMY1234_REG_EVENTS),
};

static const struct regmap_access_table dummy1234_precious_table = {
	.yes_ranges = dummy1234_precious_ranges,
	.n_yes_ranges = ARRAY_SIZE(dummy1234_precious_ranges),
};

static const struct regmap_config dummy1234_regmap_config = {
	//...
	.precious_table = &dummy1234_precious_table,
	//...
};
```

`DUMMY1234_REG_EVENTS` will only be read by the driver, allowing us to control the event management. If you are still missing events after declaring a register `precious`, you will probably have to solve some other kind of issue: hardware, timing, etc. Sorry, mate!

### 9. Debugging

Once a driver starts controlling a device, you are usually no longer able to access its registers. That makes sense because accessing the registers at that point is very hacky, and you could break many things by playing around with their values. But knowing what values they store is sometimes useful, especially while programming the driver and later on for debugging purposes. Regmap offers a safe solution for those needs: `debugfs` attributes.

In order to use this feature, we will have to get access to `debugfs`. Easy task, though: in many systems `/sys/kernel/debug/regmap` is already visible and available (if at least one device supports regmap). If not, try to mount `debugfs` like this: `mount -t debugfs none /sys/kernel/debug`. Then you will be able to see the devices with regmap, hopefully yours among them. For example, my SBC is connected to two I2C devices, one I wrote a driver for a while ago, and the dummy1234 we have been working on:

```sh
ls /sys/kernel/debug/regmap/
1-0010-veml6075  1-0029-dummy1234
```

What do the names mean? In this case, for I2C devices, the first name is the bus address/number (in this case, I2C-1 for both devices), the second is the device address (0x10 and 0x29), and the rest is the name used to initialize the `.name` member of `regmap_config`, if a name was defined (empty otherwise).

What's in there? Let's see:
```sh
ls /sys/kernel/debug/regmap/1-0029-dummy1234
access  name  range  register
```

These attributes are all read-only and need root access to be read. Let's see them one by one:
- <u>access:</u>: it displays a rather cryptic table with per-register access rights in the following form:

```
    reg-addr: read write volatile precious
```
I have populated some register ranges with different access rights, so you can see it in action:

```sh
cat access
00: y y n n
01: y y n n
02: y y n n
03: y y n n
04: y y n n
05: y y n n
06: y y n n
07: y y n n
08: y n n n
09: y n n n
0a: y n n n
0b: y n n n
0c: y n n n
0d: y n n n
0e: y n n n
0f: y n n n
10: y n y n
11: y n y n
12: y n y n
13: y n y n
14: y n n n
15: y n n n
16: y n n n
17: y n n y
18: y n n n
```

In this example, all registers [0x00-0x18] can be read, only [0x00-0x07] can be written, [0x10-0x13] are volatile, and only 0x17 is precious.

- <u>name:</u>: trivial one, the driver name.

```sh
cat name
dummy1234
```

- <u>range:</u>: the range of addresses you can access:

```sh
cat range
0-16
18-18
```
Why are there two ranges, and why is the address 0x17 gone? This debugging feature only allows you to read register values, but NO precious registers, as only the driver should be able to access them. As we saw before, 0x17 is precious, and it's good that it cannot be accessed via `debugfs`.

- <u>registers:</u>: the register values at a given point:

```sh
cat registers
00: 01
01: 80
02: 00
03: 00
04: ff
05: ff
06: 00
07: 00
08: 00
09: 00
0a: 00
0b: 00
0c: 00
0d: 00
0e: 00
0f: 00
10: 00
11: 00
12: 00
13: 00
14: 01
15: 00
16: 00
18: ff
```
As you can see, 0x17 is missing, which is actually what we want. But the rest of the readable registers are available, and knowing their values is great for debugging and way cleaner than hacking `printk()` functions all over the place.

### 10. Other features

I bet you have already noticed that regmap covers many different use cases, and for that it has to offer many different features. Even though we have already seen what you will be using ~99% of the time, there are some other features that are worth mentioning, as they could help you in some specific situations.

#### 10.1 Register defaults

Regmap allows drivers to provide default register values <u>for the cache</u> (not to write default values to the hardware!) during its initialization, and they are not meant to be used without it. This feature is extensively used in some subsystems (e.g. sound codecs) due to the nature of their devices, but it is not something that most of the drivers implement.

There are two different approaches to provide default values, and unfortunately, they are poorly documented. But here I am to help you out! You can either provide a set of pairs [address-value] by means of the `reg_defaults` and `num_reg_defaults` attributes, or values for a contiguous range via `reg_defaults_raw` and `num_reg_defaults_raw`, all of them within `regmap_config`.

Let's see some upstream code using the different approaches, first with `reg_defaults`, which makes use of the `reg_default` struct to provide the [address-value] pairs:

```c
// drivers/leds/flash/leds-lm3601x.c

static const struct reg_default lm3601x_regmap_defs[] = {
	{ LM3601X_ENABLE_REG, 0x20 },
	{ LM3601X_CFG_REG, 0x15 },
	{ LM3601X_LED_FLASH_REG, 0x00 },
	{ LM3601X_LED_TORCH_REG, 0x00 },
};
//...
static const struct regmap_config lm3601x_regmap = {
	//...
	.reg_defaults = lm3601x_regmap_defs,
	.num_reg_defaults = ARRAY_SIZE(lm3601x_regmap_defs),
	//...
};
```

And now with `reg_defaults_raw`:

```c
// sound/soc/codecs/jz4770.c

static const u8 jz4770_codec_reg_defaults[] = {
	0x00, 0xC3, 0xC3, 0x90, 0x98, 0xFF, 0x90, 0xB1,
	0x11, 0x10, 0x00, 0x03, 0x00, 0x00, 0x40, 0x00,
	0xFF, 0x00, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x34,
	0x07, 0x44, 0x1F, 0x00
};

static const struct regmap_config jz4770_codec_regmap_config = {
	//...
	.reg_defaults_raw = jz4770_codec_reg_defaults,
	.num_reg_defaults_raw = ARRAY_SIZE(jz4770_codec_reg_defaults),
	//...
};
```

Note that the first approach is way, way more common than the second. What you will actually find *slightly* more often is `num_reg_defaults_raw` **without** `reg_defaults_raw`. That is the way to tell regmap that default values can't be provided by the driver, and `num_reg_defaults_raw` registers will be directly read from the hardware to initialize the cache. Usually, the driver will configure the device in a certain way in the probe function, and setting defaults won't make much sense, but that will always depend on the device needs and what is more optimal in every case.

#### 10.2. Patching

This feature allows updating multiple registers efficiently in a single operation, which is useful when initializing hardware with a predefined set of values. When would you need that? The function documentation gives you an example: *Typically this is used to apply corrections to the device defaults on startup, such as the updates some vendors provide to undocumented registers.*
If you ever find yourself requiring such feature, you will need a `reg_sequence` again like we did for multi-register accesses (which is what this one is doing too, but always directly to the hardware), and its size:

```c
ret = regmap_register_patch(regmap, reg_sequence, num_regs);
```

Although you should know by now how to use this function, let's see an upstream use case:

```c
// sound/soc/codecs/cs35l32.c

/* Current and threshold powerup sequence Pg37 in datasheet */
static const struct reg_sequence cs35l32_monitor_patch[] = {
	{ 0x00, 0x99 },
	{ 0x48, 0x17 },
	{ 0x49, 0x56 },
	{ 0x43, 0x01 },
	{ 0x3B, 0x62 },
	{ 0x3C, 0x80 },
	{ 0x00, 0x00 },
};
//...
static int cs35l32_i2c_probe(struct i2c_client *i2c_client)
{
	//...
	ret = regmap_register_patch(cs35l32->regmap, cs35l32_monitor_patch,
				    ARRAY_SIZE(cs35l32_monitor_patch));
	//...
}
```

#### 10.3. Polling

Polling in regmap refers to the process of repeatedly reading a register until a specific condition is met. This is useful for situations where a hardware event must be waited on, such as checking if a device is ready, waiting for a status bit to change, or synchronizing with hardware.

Regmap provides `regmap_read_poll_timeout()` to simplify polling logic, automatically handling retries and timeouts. This function is particularly useful when waiting for a device to complete an operation without requiring an interrupt. It avoids busy loops and ensures that the wait time is limited. The macro definition looks like this:

```c
/**
 * regmap_read_poll_timeout - Poll until a condition is met or a timeout occurs
 *
 * @map: Regmap to read from
 * @addr: Address to poll
 * @val: Unsigned integer variable to read the value into
 * @cond: Break condition (usually involving @val)
 * @sleep_us: Maximum time to sleep between reads in us (0 tight-loops). Please
 *            read usleep_range() function description for details and
 *            limitations.
 * @timeout_us: Timeout in us, 0 means never timeout
 *
 * This is modelled after the readx_poll_timeout macros in linux/iopoll.h.
 *
 * Returns: 0 on success and -ETIMEDOUT upon a timeout or the regmap_read
 * error return value in case of a error read. In the two former cases,
 * the last read value at @addr is stored in @val. Must not be called
 * from atomic context if sleep_us or timeout_us are used.
 */
#define regmap_read_poll_timeout(map, addr, val, cond, sleep_us, timeout_us) \
({ \
	int __ret, __tmp; \
	__tmp = read_poll_timeout(regmap_read, __ret, __ret || (cond), \
			sleep_us, timeout_us, false, (map), (addr), &(val)); \
	__ret ?: __tmp; \
})

// would be used like this:
ret = regmap_read_poll_timeout(regmap, reg_address, read_val, condition, sleep_us, timeout_us);
```

This macro (and similar ones in `include/linux/regmap.h` to read a regfield, for atomic operations, etc.) is useful to wait for a condition i.e. usually an expected value in the register before taking some action. Again, some real upstream code to illustrate it:

```c
// drivers/clk/clk-si5341.c

/* wait for device to report input clock present and PLL lock */
err = regmap_read_poll_timeout(data->regmap, SI5341_STATUS, status,
	!(status & (SI5341_STATUS_LOSREF | SI5341_STATUS_LOL)),
       10000, 250000);
```

#### 10.4. Raw access

While regmap provides a structured API for reading and writing registers, sometimes direct access to raw register values is needed. This is where `regmap_raw_read()` and `regmap_raw_write()` come in, allowing bulk transfer of data instead of individual register accesses. This is useful when dealing with block transfers, such as firmware loading or reading sensor data in bulk.

Raw access is particularly helpful when a device exposes a memory-mapped interface or when performance is critical. It reduces the overhead of multiple function calls and allows for efficient data transfers. Once again, you should have good reasons to require such optimizations, and even though there are multiple drivers that use it (oh surprise, a few of them under `sound/soc/codecs`, the king of uncommon regmap features 😝), it is not as common as the previous access functions we discussed.

In order to use them, you will have to provide a `void` pointer for the payload and its length:

```c
/**
 * regmap_raw_write() - Write raw values to one or more registers
 *
 * @map: Register map to write to
 * @reg: Initial register to write to
 * @val: Block of data to be written, laid out for direct transmission to the
 *       device
 * @val_len: Length of data pointed to by val.
 *
 * This function is intended to be used for things like firmware
 * download where a large block of data needs to be transferred to the
 * device.  No formatting will be done on the data provided.
 *
 * A value of zero will be returned on success, a negative errno will
 * be returned in error cases.
 */
int regmap_raw_write(struct regmap *map, unsigned int reg,
		     const void *val, size_t val_len);
/**
 * regmap_raw_read() - Read raw data from the device
 *
 * @map: Register map to read from
 * @reg: First register to be read from
 * @val: Pointer to store read value
 * @val_len: Size of data to read
 *
 * A value of zero will be returned on success, a negative errno will
 * be returned in error cases.
 */
int regmap_raw_read(struct regmap *map, unsigned int reg, void *val,
		    size_t val_len);
```

If you are going to use these functions, take a look at existing drivers: you will notice that they often require uncommon handling of the cache with explicit block synchronization and flushing. I have never had to use anything like that so far, and even though it must be interesting, it is most probably an easy way to mess up and introduce bugs 😆

#### 10.5. Locking strategies

By default, regmap handles register access locking by an internal mutex to prevent race conditions when multiple threads or contexts access the same registers. That's a great default behavior, and usually what we want to have. However, in some cases, a different locking approach can be required, like using a spinlock instead of a mutex or using a custom lock/unlock callback.

For example: if you need **justified** fast I/O accesses, the `fast_io` attribute of `regmap_config` is the boolean you will have to set (it is false by default, of course) to use spinlocks instead of mutexes. It is as easy as assigning `.fast_io = true;` within `regmap_cofig`, but it has all the implications of using spinlocks instead of mutexes, so use it carefully and when it is absolutely required.

If you have to overwrite the internal `regmap_lock()` and `regmap_unlock()` functions, you will have to provide callbacks with the following signature:

```c
typedef void (*regmap_lock)(void *);
typedef void (*regmap_unlock)(void *);
```

Again, these functions will be assigned to `.lock` and `.unlock` in `regmap_config`. And again, you should have good reasons to require your own locking mechanism, like requiring some specific calls like `raw_spin_lock_irqsave()` and `raw_spin_unlock_irqrestore()` (see `drivers/gpio/gpio-104-dio-48e.c` for a real example).

---

Let's call it a day. If you have read the whole article until the end, you probably know more about regmap that you will ever need, and you are definitely ready to use basic and advanced features of this amazing API. Nevertheless, if you would like me to discuss yet another feature (believe it or not, regmap has more to offer), please let me know, and I will update the article with that obscure feature you desperately need

I hope you will use regmap from day 1 on in your device drivers!
