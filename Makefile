TARGET = get_memblock
MODULES_DIR = /home/stolexiy/WSL2-Linux-Kernel
obj-m += $(TARGET).o
#$(TARGET)-objs += get_memblock.o

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
	
