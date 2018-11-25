aarch64-elf-as -mcpu=cortex-a53 -g startup.s -o startup.o
aarch64-elf-gcc -mcpu=cortex-a53 -g -Wall -nostdlib -ffreestanding -c test.c -o test.o
aarch64-elf-ld -T test.ld test.o startup.o -o test.elf
aarch64-elf-objcopy -O binary test.elf test.bin
