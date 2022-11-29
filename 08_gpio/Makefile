
GPIO_IRQ=./gpio_irq
GPIO_POLL=./gpio_poll

KERNELDIR ?= ~/repos/linux-stable/

irq:
	export ARCH=arm\
	&& export CROSS_COMPILE='ccache arm-eabi-'\
	&& export PATH=/opt/gcc-arm-8.3-2019.03-x86_64-arm-eabi/bin:$(PATH)\
	&& cd $(GPIO_IRQ)\
	&& $(MAKE) KDIR=$(KERNELDIR);

poll:
	export ARCH=arm\
	&& export CROSS_COMPILE='ccache arm-eabi-'\
	&& export PATH=/opt/gcc-arm-8.3-2019.03-x86_64-arm-eabi/bin:$(PATH)\
	&& cd $(GPIO_POLL)\
	&& $(MAKE) KDIR=$(KERNELDIR);

all: irq poll

clean_irq:
	cd $(GPIO_IRQ) && $(MAKE) clean;

clean_poll:
	cd $(GPIO_POLL) && $(MAKE) clean;

clean: clean_irq clean_poll
	