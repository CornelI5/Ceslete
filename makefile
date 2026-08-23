CC = gcc
ASM = nasm
LD = ld
CFLAGS = -ffreestanding -m64 -O2 -Wall -Wextra -nostdlib
ASMFLAGS = -f elf64

SRC = src
BUILD = build
TOOLS = tools

OBJS = $(BUILD)/toastloader.o \
       $(BUILD)/dotshirp.o \
       $(BUILD)/eru/vm.o \
       $(BUILD)/eru/render.o

TARGET = $(BUILD)/ceslete.efi

all: dirs tools $(TARGET) assets

dirs:
	@mkdir -p $(BUILD)/eru $(BUILD)/tools assets

tools: $(BUILD)/tools/malbc $(BUILD)/tools/hymn

$(BUILD)/tools/malbc: $(TOOLS)/malbc.c | dirs
	$(CC) -O2 -o $@ $<

$(BUILD)/tools/hymn: $(TOOLS)/hymn.c | dirs
	$(CC) -O2 -lm -o $@ $<

$(BUILD)/toastloader.o: $(SRC)/toastloader.asm | dirs
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILD)/dotshirp.o: $(SRC)/dotshirp.c $(SRC)/ceslete.h | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/eru/vm.o: $(SRC)/eru/vm.c $(SRC)/eru/eru.h | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/eru/render.o: $(SRC)/eru/render.mbc $(BUILD)/tools/malbc | dirs
	$(BUILD)/tools/malbc $< $(BUILD)/eru/render.bin
	$(LD) -r -b binary -o $@ $(BUILD)/eru/render.bin

$(TARGET): $(OBJS) linker.ld | dirs
	$(LD) -T linker.ld -o $@ $(OBJS)

assets: $(BUILD)/tools/hymn $(BUILD)/eru/render.bin
	$(BUILD)/tools/hymn $(BUILD)/eru/render.bin assets/hymn.wav

clean:
	rm -rf $(BUILD) assets/hymn.wav

.PHONY: all dirs tools assets clean
