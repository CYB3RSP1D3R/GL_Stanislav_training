# GPIO

In this project there is 2 kernel modules that use GPIO input and output.
First one [gpio_irq](./gpio_irq) uses interrupts that trigger when **User/Boot** button changes its state. Second one
[gpio_poll](./gpio_poll) polls **User/Boot** button. Both modules use 2 light diods to indicate the state of module.

## GPIO_IRQ

When the button change its state, one interrupt iterate `counter` and second displays counter value.

Using `simulate_busy` parameter with insmod user can disable the simulation of long-time work in the function that
displays `counter` value (sleep for several seconds).

### Output of the running module

```
[  133.090593] gpio_irq: module initialized
/ # [  136.346878] gpio_irq: simulating long-time work (sleeping)
[  138.416462] gpio_irq: Long-time work is done
[  138.420939] gpio_irq: counter value: 2
[  138.424863] gpio_irq: simulating long-time work (sleeping)
[  140.496471] gpio_irq: Long-time work is done
[  140.500949] gpio_irq: counter value: 2
[  146.342872] gpio_irq: simulating long-time work (sleeping)
[  148.416452] gpio_irq: Long-time work is done
[  148.420927] gpio_irq: counter value: 4
[  148.424850] gpio_irq: simulating long-time work (sleeping)
[  150.496467] gpio_irq: Long-time work is done
[  150.500942] gpio_irq: counter value: 4
[  154.498027] gpio_irq: simulating long-time work (sleeping)
[  156.576459] gpio_irq: Long-time work is done
[  156.580936] gpio_irq: counter value: 5
[  158.539547] gpio_irq: simulating long-time work (sleeping)
[  160.576466] gpio_irq: Long-time work is done
[  160.580941] gpio_irq: counter value: 6
[  202.225721] gpio_irq: simulating long-time work (sleeping)
[  204.256463] gpio_irq: Long-time work is done
[  204.260938] gpio_irq: counter value: 16
[  204.264961] gpio_irq: simulating long-time work (sleeping)
[  206.336460] gpio_irq: Long-time work is done
[  206.340935] gpio_irq: counter value: 22
[  206.344950] gpio_irq: simulating long-time work (sleeping)
[  208.416464] gpio_irq: Long-time work is done
[  208.420938] gpio_irq: counter value: 22
```

## GPIO_POLL

The button is polled every 20 ms. The button value is transfered to the light diode. If the module detects the change of
state, then it turns on high-precision counter and blinks for 2 ms on the other light diode.
