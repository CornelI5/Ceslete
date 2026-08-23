$(BUILD)/tools/esasm: tools/esasm.c | $(BUILD)/tools
	$(CC) -O2 -o $@ $<

$(BUILD)/eru/render.o: $(SRC)/eru/render.es.txt $(BUILD)/tools/esasm | $(BUILD)/eru
	$(BUILD)/tools/esasm $< $(BUILD)/eru/render.bin
	$(LD) -r -b binary -o $@ $(BUILD)/eru/render.bin

$(BUILD)/tools:
	mkdir -p $(BUILD)/tools
