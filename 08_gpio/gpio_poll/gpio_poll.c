// SPDX-License-Identifier: MIT and GPL
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/ktime.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/hrtimer.h>


MODULE_DESCRIPTION("gpio_poll");
MODULE_AUTHOR("CYB3RSP1D3R");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL");


// (port, bit) to address convertion
#define GPIO_ADDR(port, bit) (32 * (port) + (bit))

// GPIO defines
#define LLED_PIN GPIO_ADDR(1, 12)
#define SLED_PIN GPIO_ADDR(1, 13)
#define BUTTON_PIN GPIO_ADDR(2, 8)

#define BUTTON_PERIOD_MS (20)
#define BLINK_PERIOD_US (1000000)

enum pin_state {low = false, high = true};


struct timer_list button_timer;
struct hrtimer blink_timer;
static enum pin_state button_state;


int gpio_led_init(uint16_t pin, const char *label)
{
	if (!gpio_is_valid(pin))
		return -EINVAL;

	return gpio_request_one(pin, GPIOF_OUT_INIT_LOW, label);
}


int gpio_button_init(uint16_t pin, const char *label)
{
	int rc;

	if (!gpio_is_valid(pin))
		return -EINVAL;

	rc = gpio_request_one(pin, GPIOF_IN, label);

	if (rc < 0)
		return rc;

	return 0;
}

enum hrtimer_restart blink_fnc(struct hrtimer *data)
{
	gpio_set_value(SLED_PIN, low);
	return HRTIMER_NORESTART;
}


void button_fnc(struct timer_list *data)
{
	ktime_t period;
	enum pin_state current_state = gpio_get_value(BUTTON_PIN);

	if (button_state == current_state) {
		gpio_set_value(LLED_PIN, button_state);
	} else {
		period = ktime_set(0, BLINK_PERIOD_US);
		gpio_set_value(SLED_PIN, high);
		hrtimer_start(&blink_timer, period, HRTIMER_MODE_REL);
		button_state = current_state;
	}

	mod_timer(&button_timer, jiffies + msecs_to_jiffies(BUTTON_PERIOD_MS));
}


// Init and exit functions
static int __init init_mod(void)
{
	int state = 0;

	state = gpio_led_init(LLED_PIN, "LLED");
	if (state < 0) {
		pr_err("gpio_poll: gpio LLED at GPIO%d initialization failed\n", LLED_PIN);
		return state;
	}

	state = gpio_led_init(SLED_PIN, "SLED");
	if (state < 0) {
		pr_err("gpio_poll: gpio SLED at GPIO%d initialization failed\n", SLED_PIN);
		return state;
	}

	state = gpio_button_init(BUTTON_PIN, "BUTTON");
	if (state < 0) {
		pr_err("gpio_poll: gpio button at GPIO%d initialization failed\n", BUTTON_PIN);
		return state;
	}

	timer_setup(&button_timer, button_fnc, 0);
	mod_timer(&button_timer, jiffies + msecs_to_jiffies(BUTTON_PERIOD_MS));
	hrtimer_init(&blink_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	blink_timer.function = &blink_fnc;
	button_state = gpio_get_value(BUTTON_PIN);

	pr_info("gpio_poll: module initialized\n");

	return state;
}


static void __exit cleanup_mod(void)
{
	gpio_set_value(LLED_PIN, low);

	hrtimer_cancel(&blink_timer);
	del_timer(&button_timer);

	gpio_free(LLED_PIN);
	gpio_free(SLED_PIN);
	gpio_free(BUTTON_PIN);

	pr_info("gpio_poll: module has been deleted from the kernel\n");
}

module_init(init_mod);
module_exit(cleanup_mod);
