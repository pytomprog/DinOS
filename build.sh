#!/bin/bash
scriptDir=$(dirname "$0")

nasm $scriptDir/src/mbr.asm -f bin -o $scriptDir/build/mbr.bin
nasm $scriptDir/src/kernelEntry.asm -f elf -o $scriptDir/build/kernelEntry.o
i686-linux-gnu-gcc -ffreestanding -nostdlib -fno-pic -fno-pie -c $scriptDir/src/main.c -o $scriptDir/build/main.o
i686-linux-gnu-ld -Ttext 0x1000 --oformat binary $scriptDir/build/kernelEntry.o build/main.o -o $scriptDir/build/kernel.bin
truncate -s 8192 $scriptDir/build/kernel.bin
cat $scriptDir/build/mbr.bin $scriptDir/build/kernel.bin > $scriptDir/build/os.bin

