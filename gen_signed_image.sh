#!/bin/bash
make clean
make -j"$(nproc)" ARCH=arm ast2600_openbmc_spl_defconfig DEVICE_TREE=ast2600-evb
make -j"$(nproc)" ARCH=arm CROSS_COMPILE="ccache arm-linux-gnueabi-" DEVICE_TREE=ast2600-evb 
tools/mkimage -f ../u-boot.its -r -E -k keys -K spl/u-boot-spl.dtb u-boot-signed.bin
cat spl/u-boot-spl-nodtb.bin spl/u-boot-spl.dtb > u-boot-spl-concat.bin
dd if=/dev/zero of=flash bs=64M count=1
dd if=u-boot-spl-concat.bin of=flash conv=notrunc
dd if=u-boot-signed.bin of=flash conv=notrunc seek=64 bs=1024
/home/kotes/qemu/build/qemu-system-arm -M ast2600-evb -nographic -nic user,tftp=flash -drive file=flash,if=mtd,format=raw 
