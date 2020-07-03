obj-m = mod_hello.o mod_hrtimer.o mod_readaddr.o mod_readaddr_litex.o mod_chardev.o mod_tdc.o
KERNEL_SRC_HOST = /lib/modules/$(shell uname -r)/build


# TODO anpassen
KERNEL_SRC_TARGET = /home/trellisdev/tools/litex/buildroot/output/build/linux-0192f8256a3966ac16ab5a2f3d88a2a4323078ca

# TODO anpassen
RISCV_TOOLCHAIN = riscv64-unknown-elf-

target:
	@echo "Building for target"
	@make \
		CROSS_COMPILE=$(RISCV_TOOLCHAIN) \
		ARCH=riscv \
		-C $(KERNEL_SRC_TARGET) \
		M=$(PWD) \
		V=1 \
		modules

	@rm  -f  *.o  *.mod.c .*.cmd modules.order Module.symvers .cache.mk *.mod

host:
	@echo "Building for host"
	@make  -C $(KERNEL_SRC_HOST)  M=$(PWD)  modules
	@rm  -f  *.o  *.mod.c .*.cmd modules.order Module.symvers .cache.mk

clean:
	make -C $(KERNEL_SRC_TARGET) M=$(PWD) clean

install:
	scp mod_hello.ko root@esl.local:/root
	scp mod_hrtimer.ko root@esl.local:/root

# indents in rules above have to be TAB characters! Check&correct after copy&paste out of PDF!
