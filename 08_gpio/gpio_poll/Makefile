
ifneq ($(KERNELRELEASE),)

ARCH=arm
PATH+=/opt/gcc-arm-8.3-2019.03-x86_64-arm-eabi/bin
CROSS_COMPILE='ccache arm-eabi-'

# kbuild part of makefile
obj-m := gpio_poll.o
else
# normal makefile
KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
endif

check_patch:
	$(KDIR)/scripts/checkpatch.pl -f --no-tree gpio_poll.c
