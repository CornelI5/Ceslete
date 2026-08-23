$(BUILD)/tools/malbc: tools/malbc.c | $(BUILD)/tools
	$(CC) -O2 -o $@ $<

$(BUILD)/eru/render.o: $(SRC)/eru/render.mbc $(BUILD)/tools/malbc | $(BUILD)/eru
	$(BUILD)/tools/malbc $< $(BUILD)/eru/render.bin
	$(LD) -r -b binary -o $@ $(BUILD)/eru/render.bin

$(BUILD)/tools:
	mkdir -p $(BUILD)/tools
