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

src_dir := src/arch/$(arch)
build_dir := build/arch/$(arch)

c_files := $(src_dir)/kernel_main.c $(src_dir)/vga.c $(src_dir)/memfuncs.c
o_files := $(build_dir)/vga.o $(build_dir)/kernel_main.o $(build_dir)/memfuncs.o $(build_dir)/inline_asm.o $(build_dir)/ps2.o
h_files := $(src_dir)/vga.h $(src_dir)/memfuncs.h $(src_dir)/inline_asm.h $(src_dir)/ps2.h

.PHONY: all clean run iso

all: $(kernel)

clean:
	@rm -r build

run: $(iso)
	@qemu-system-x86_64 -cdrom $(iso)

iso: $(iso)

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	@cp $(kernel) build/isofiles/boot/kernel.bin
	@cp $(grub_cfg) build/isofiles/boot/grub
	@grub-mkrescue -o $(iso) build/isofiles 2> /dev/null
	@rm -r build/isofiles

$(kernel): $(assembly_object_files) $(linker_script) $(o_files) $(h_files)
	ld -n -T $(linker_script) -o $(kernel) $(assembly_object_files) $(o_files) 

# compile assembly files 
$(build_dir)/long_mode_init.o: $(src_dir)/long_mode_init.asm
	mkdir -p $(shell dirname $@)
	nasm -felf64 $< -o $@

$(build_dir)/boot.o: $(src_dir)/boot.asm
	mkdir -p $(shell dirname $@)
	nasm -felf64 $< -o $@

$(build_dir)/multiboot_header.o: $(src_dir)/multiboot_header.asm
	mkdir -p $(shell dirname $@)
	nasm -felf64 $< -o $@

$(build_dir)/inline_asm.o: $(src_dir)/inline_asm.c
	mkdir -p $(shell dirname $@)
	$(CC) -g -c $< -o $@ -ffreestanding

$(build_dir)/ps2.o: $(src_dir)/ps2.c
	mkdir -p $(shell dirname $@)
	$(CC) -g -c $< -o $@ -ffreestanding

$(build_dir)/kernel_main.o: $(src_dir)/kernel_main.c
	mkdir -p $(shell dirname $@)
	$(CC) -g -c $< -o $@ -ffreestanding

$(build_dir)/vga.o: $(src_dir)/vga.c $(src_dir)/vga.h
	mkdir -p $(shell dirname $@)
	$(CC) -g -c $< -o $@

$(build_dir)/memfuncs.o: $(src_dir)/memfuncs.c $(src_dir)/memfuncs.h
	mkdir -p $(shell dirname $@)
	$(CC) -g -c $< -o $@
	


