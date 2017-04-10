arch ?= x86_64
kernel := build/kernel-$(arch).bin
iso := build/os-$(arch).iso

linker_script := src/arch/$(arch)/linker.ld
grub_cfg := src/arch/$(arch)/grub.cfg
assembly_source_files := $(wildcard src/arch/$(arch)/*.asm)
assembly_object_files := $(patsubst src/arch/$(arch)/%.asm, \
	build/arch/$(arch)/%.o, $(assembly_source_files))
c_files := src/arch/$(arch)/kernel_main.c src/arch/$(arch)/vga.c
o_files := src/arch/$(arch)/vga.o src/arch/$(arch)/kernel_main.o
h_files := src/arch/$(arch)/vga.h

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

$(kernel): $(assembly_object_files) $(linker_script) $(o_files)
	@ld -n -T $(linker_script) -o $(kernel) $(assembly_object_files) $(o_files)

# compile assembly files
build/arch/$(arch)/%.o: src/arch/$(arch)/%.asm $(c_files) $(h_files)
	@gcc -g -c $(c_files) $(h_files)
	@cp kernel_main.o src/arch/$(arch)
	@mkdir -p $(shell dirname $@)
	@nasm -felf64 $< -o $@
