TARGET = test1
obj-m := $(TARGET).o
#$(TARGET)-objs := kernel_struct_reader.o memblock_region_reader.o thread_group_cputimer_reader.o
$(TARGET)-objs := test_thread_group_cputimer_reader.o

all: 
	make -C /home/stolexiy/WSL2-Linux-Kernel M=$(shell pwd) modules

clean:
	make -C /home/stolexiy/WSL2-Linux-Kernel M=$(shell pwd) clean

load:
	sudo insmod $(TARGET).ko
	sudo dmesg -c
	
unload:
	sudo rmmod $(TARGET)
	sudo dmesg -c
	