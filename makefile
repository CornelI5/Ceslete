CC = gcc
ASM = nasm
LD = ld
CFLAGS = -ffreestanding -m64 -O2 -Wall -Wextra -nostdlib
ASMFLAGS = -f elf64

SRC = src
BUILD = build

OBJS = $(BUILD)/toastloader.o \
       $(BUILD)/dotshirp.o \
       $(BUILD)/eru/vm.o \
       $(BUILD)/eru/render.o

TARGET = $(BUILD)/ceslete.efi

all: $(TARGET)

$(BUILD)/toastloader.o: $(SRC)/toastloader.asm | $(BUILD)
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILD)/dotshirp.o: $(SRC)/dotshirp.c $(SRC)/ceslete.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/eru/vm.o: $(SRC)/eru/vm.c $(SRC)/eru/eru.h | $(BUILD)/eru
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/eru/render.o: $(SRC)/eru/render.es | $(BUILD)/eru
	$(LD) -r -b binary -o $@ $<

$(TARGET): $(OBJS) linker.ld | $(BUILD)
	$(LD) -T linker.ld -o $@ $(OBJS)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/eru:
	mkdir -p $(BUILD)/eru

clean:
	rm -rf $(BUILD)

.PHONY: all clean
