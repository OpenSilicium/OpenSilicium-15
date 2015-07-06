# Run this Makefile as follows:
# (MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
#

KDIR= /lib/modules/$(shell uname -r)/build
PWD= $(shell pwd)

PROG= brb_test

obj-m := brb.o

all: $(PROG)
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

install:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules_install

clean:
	rm -f *~ Module.markers Modules.symvers $(PROG)
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

