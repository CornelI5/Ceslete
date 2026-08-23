all: dirs tools $(TARGET) assets

# ... existing rules ...

$(BUILD)/mouse.o: $(SRC)/mouse.c $(SRC)/mouse.h | dirs
	$(CC) $(CFLAGS) -c $< -o $@

OBJS += $(BUILD)/mouse.o
