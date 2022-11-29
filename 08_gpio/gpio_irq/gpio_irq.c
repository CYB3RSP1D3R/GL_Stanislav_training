// SPDX-License-Identifier: MIT and GPL
// Made by CYB3RSP1D3R

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
#include <linux/delay.h>


MODULE_DESCRIPTION("gpio_irq");
MODULE_AUTHOR("CYB3RSP1D3R");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL");


static bool simulate_busy = true;
module_param(simulate_busy, bool, 0);
MODULE_PARM_DESC(simulate_busy, "Enables simulation of long-time work in the thread_fn");


// (port, bit) to address convertion
#define GPIO_ADDR(port, bit) (32 * (port) + (bit))

// GPIO defines
#define LED_1_PIN GPIO_ADDR(1, 12)
#define LED_2_PIN GPIO_ADDR(1, 13)
#define BUTTON_PIN GPIO_ADDR(2, 8)

// delays
#define BUSY_DELAY_MS (2000)
#define BTN_DEBOUNCE_TIME_US (2000)

enum pin_state {low = false, high = true};


static int button_irq;
static uint32_t counter;
enum pin_state led_1_state;


int gpio_led_init(uint16_t pin, const char *label)
{
	if (!gpio_is_valid(pin))
		return -EINVAL;

	return gpio_request_one(pin, GPIOF_OUT_INIT_LOW, label);
}


int gpio_button_init(uint16_t pin, const char *label, uint32_t debounce)
{
	int rc;

	if (!gpio_is_valid(pin))
		return -EINVAL;

	rc = gpio_request_one(pin, GPIOF_IN, label);

	if (rc < 0)
		return rc;

	return gpio_set_debounce(pin, debounce);
}


static irqreturn_t button_handler(int irq, void *data)
{
	unsigned long flags;

	local_irq_save(flags);
	led_1_state = !led_1_state;
	gpio_set_value(LED_1_PIN, led_1_state);
	counter++;
	local_irq_restore(flags);
	return IRQ_WAKE_THREAD;
}


static irqreturn_t thread_fn(int irq, void *ptr)
{
	if (simulate_busy) {
		gpio_set_value(LED_2_PIN, high);
		pr_info("gpio_irq: simulating long-time work (sleeping)\n");
		msleep(BUSY_DELAY_MS);
		gpio_set_value(LED_2_PIN, low);
		pr_info("gpio_irq: Long-time work is done\n");
	}
	pr_info("gpio_irq: counter value: %d\n", counter);
	return IRQ_HANDLED;
}

// Init and exit functions
static int __init init_mod(void)
{
	int state;

	led_1_state = low;
	state = gpio_led_init(LED_1_PIN, "LED_1");
	if (state < 0) {
		pr_err("gpio_irq: gpio LED_1 at GPIO%d initialization failed\n", LED_1_PIN);
		return state;
	}

	state = gpio_led_init(LED_2_PIN, "LED_2");
	if (state < 0) {
		pr_err("gpio_irq: gpio LED_2 at GPIO%d initialization failed\n", LED_2_PIN);
		return state;
	}

	state = gpio_button_init(BUTTON_PIN, "BUTTON", BTN_DEBOUNCE_TIME_US);
	if (state < 0) {
		pr_err("gpio_irq: gpio button at GPIO%d initialization failed\n", BUTTON_PIN);
		return state;
	}

	button_irq = gpio_to_irq(BUTTON_PIN);
	if (button_irq < 0) {
		pr_err("gpio_irq: from 'gpio_to_irq' for button\n");
		return button_irq;
	}

	state = request_threaded_irq(button_irq, button_handler, thread_fn,
		IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "Button", NULL);

	if (state < 0) {
		pr_err("gpio_irq: failed to request the IRQ\n");
		return state;
	}

	pr_info("gpio_irq: module initialized\n");

	return 0;
}

static void __exit cleanup_mod(void)
{
	gpio_set_value(LED_1_PIN, low);
	gpio_set_value(LED_2_PIN, low);

	gpio_free(LED_1_PIN);
	gpio_free(LED_2_PIN);
	free_irq(button_irq, "Button");
	gpio_free(BUTTON_PIN);

	pr_info("gpio_irq: module has been deleted from the kernel\n");
}

module_init(init_mod);
module_exit(cleanup_mod);
