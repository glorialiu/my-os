arch ?= x86_64
kernel := build/kernel-$(arch).bin
iso := build/os-$(arch).iso


target:= x86_64-elf
path := /home/glorialiu/opt/cross/bin

linker_script := src/arch/$(arch)/linker.ld
grub_cfg := src/arch/$(arch)/grub.cfg
assembly_source_files := $(wildcard src/arch/$(arch)/*.asm)
assembly_object_files := $(patsubst src/arch/$(arch)/%.asm, \
	build/arch/$(arch)/%.o, $(assembly_source_files))

CC := $(path)/$(target)-gcc
FLAGS := -g -ggdb -mno-red-zone -ffreestanding  
ASM_FLAGS := -f elf64 -g
src_dir := src/arch/$(arch)
build_dir := build/arch/$(arch)

c_files := $(src_dir)/kernel_main.c $(src_dir)/vga.c $(src_dir)/memfuncs.c
o_files := $(build_dir)/vga.o $(build_dir)/kernel_main.o $(build_dir)/memfuncs.o $(build_dir)/inline_asm.o $(build_dir)/ps2.o $(build_dir)/interrupt.o $(build_dir)/serial.o $(build_dir)/page_alloc.o $(build_dir)/page_table.o $(build_dir)/kmalloc.o $(build_dir)/process.o $(build_dir)/snakes.o $(build_dir)/block.o $(build_dir)/mbr.o $(build_dir)/vfs.o
h_files := $(src_dir)/vga.h $(src_dir)/memfuncs.h $(src_dir)/inline_asm.h $(src_dir)/ps2.h $(src_dir)/interrupt.h $(src_dir)/serial.h $(src_dir)/page_alloc.h $(src_dir)/page_table.h $(src_dir)/kmalloc.h $(src_dir)/process.h $(src_dir)/snakes.h $(src_dir)/block.h $(src_dir)/mbr.h $(src_dir)/vfs.h

.PHONY: all clean run iso

all: img

clean:
	@rm -r build

run: $(iso)
	@qemu-system-x86_64 -s -cdrom $(iso) -serial stdio

iso: $(iso)

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	@cp $(kernel) build/isofiles/boot/kernel.bin
	@cp $(grub_cfg) build/isofiles/boot/grub
	@grub-mkrescue -o $(iso) build/isofiles 2> /dev/null
	@rm -r build/isofiles

img: $(kernel)
	rm -r .img
	mkdir .img
	mkdir .img/boot
	mkdir .img/boot/grub
	cp src/arch/$(arch)/grub.cfg .img/boot/grub
	cp build/kernel-$(arch).bin .img/boot/kernel.bin

	dd if=/dev/zero of=fat.img bs=512 count=32768
	parted fat.img mklabel msdos
	parted fat.img mkpart primary fat32 2048s 30720s
	parted fat.img set 1 boot on

	sudo losetup /dev/loop0 fat.img
	sudo losetup /dev/loop2 fat.img -o 1048576
	sudo mkdosfs -F32 -f 2 /dev/loop2
	sudo mkdir -p /mnt/fatgrub
	sudo mount /dev/loop2 /mnt/fatgrub
	sudo grub-install --root-directory=/mnt/fatgrub --no-floppy --modules="normal part_msdos ext2 multiboot" /dev/loop0
	sudo cp -r .img/* /mnt/fatgrub
	sudo umount /mnt/fatgrub
	sudo losetup -d /dev/loop0
	sudo losetup -d /dev/loop2
	qemu-system-x86_64 -s -drive format=raw,file=fat.img -serial stdio

$(kernel): $(assembly_object_files) $(linker_script) $(o_files) $(h_files)
	ld -n -T $(linker_script) -o $(kernel) $(assembly_object_files) $(o_files) 

# compile assembly files 
$(build_dir)/long_mode_init.o: $(src_dir)/long_mode_init.asm
	mkdir -p $(shell dirname $@)
	nasm $(ASM_FLAGS) $< -o $@ 

$(build_dir)/boot.o: $(src_dir)/boot.asm
	mkdir -p $(shell dirname $@)
	nasm $(ASM_FLAGS) $< -o $@

$(build_dir)/multiboot_header.o: $(src_dir)/multiboot_header.asm
	mkdir -p $(shell dirname $@)
	nasm $(ASM_FLAGS) $< -o $@

$(build_dir)/inline_asm.o: $(src_dir)/inline_asm.c
	mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(FLAGS)

$(build_dir)/ps2.o: $(src_dir)/ps2.c
	mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(FLAGS)

$(build_dir)/kernel_main.o: $(src_dir)/kernel_main.c
	mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(FLAGS)

$(build_dir)/vga.o: $(src_dir)/vga.c $(src_dir)/vga.h
	mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(FLAGS)

$(build_dir)/memfuncs.o: $(src_dir)/memfuncs.c $(src_dir)/memfuncs.h
	mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(FLAGS)
	
$(build_dir)/interrupt.o: $(src_dir)/interrupt.c $(src_dir)/interrupt.h
	mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(FLAGS)

$(build_dir)/serial.o: $(src_dir)/serial.c $(src_dir)/serial.h
	mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(FLAGS)

$(build_dir)/page_alloc.o: $(src_dir)/page_alloc.c $(src_dir)/page_alloc.h
	mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(FLAGS)

$(build_dir)/page_table.o: $(src_dir)/page_table.c $(src_dir)/page_table.h
	mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(FLAGS)

$(build_dir)/kmalloc.o: $(src_dir)/kmalloc.c $(src_dir)/kmalloc.h
	mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(FLAGS)

$(build_dir)/process.o: $(src_dir)/process.c $(src_dir)/process.h
	mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(FLAGS)

$(build_dir)/snakes.o: $(src_dir)/snakes.c $(src_dir)/snakes.h
	mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(FLAGS)

$(build_dir)/block.o: $(src_dir)/block.c $(src_dir)/block.h
	mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(FLAGS)

$(build_dir)/mbr.o: $(src_dir)/mbr.c $(src_dir)/mbr.h
	mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(FLAGS)

$(build_dir)/vfs.o: $(src_dir)/vfs.c $(src_dir)/vfs.h
	mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(FLAGS)
