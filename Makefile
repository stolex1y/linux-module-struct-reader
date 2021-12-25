TARGET = get_memblock
MODULES_DIR = /lib/modules/$(shell uname -r)/build
obj-m += $(TARGET).o

all: 
	make -C $(MODULES_DIR) M=$(shell pwd) modules

clean:
	make -C $(MODULES_DIR) M=$(shell pwd) clean

load:
	sudo insmod $(TARGET).ko
	sudo dmesg -c
	
unload:
	sudo rmmod $(TARGET)
	sudo dmesg -c
	
