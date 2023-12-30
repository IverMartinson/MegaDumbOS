NASM=nasm
DD=dd
BUILD_DIR=build

all: $(BUILD_DIR)/os.img

$(BUILD_DIR)/boot.bin: src/boot.asm
	$(NASM) -f bin $< -o $@

$(BUILD_DIR)/kernel.bin: src/kernel.asm
	$(NASM) -f bin $< -o $@

$(BUILD_DIR)/os.img: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/kernel.bin
	$(DD) if=/dev/zero of=$@ bs=512 count=2880
	$(DD) if=$(BUILD_DIR)/boot.bin of=$@ conv=notrunc
	$(DD) if=$(BUILD_DIR)/kernel.bin of=$@ seek=1 conv=notrunc
