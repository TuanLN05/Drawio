PROJECT := b5_w2

CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE    := arm-none-eabi-size

BUILD_DIR := Build
BIN_DIR   := Bin
LINKER    := Linker/STM32F103C8.ld

C_SOURCES := src/main.c \
             Inc/pwm/pwm.c \
             Inc/uart/uart.c \
             src/system_stm32f1xx.c
STARTUP   := src/startup_stm32f103xb.s
OBJECTS   := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES)) \
             $(BUILD_DIR)/startup_stm32f103xb.o
DEPS      := $(patsubst %.c,$(BUILD_DIR)/%.d,$(C_SOURCES))

CPUFLAGS := -mcpu=cortex-m3 -mthumb
INCLUDES := -IInc -IInc/pwm -IInc/uart
CFLAGS   := $(CPUFLAGS) -DSTM32F103xB $(INCLUDES) \
            -std=gnu11 -O2 -g3 -Wall -Wextra -ffreestanding \
            -ffunction-sections -fdata-sections -MMD -MP
LDFLAGS  := $(CPUFLAGS) -T$(LINKER) -nostdlib \
            -Wl,--gc-sections -Wl,-Map=$(BIN_DIR)/$(PROJECT).map

ELF := $(BIN_DIR)/$(PROJECT).elf
BIN := $(BIN_DIR)/$(PROJECT).bin
HEX := $(BIN_DIR)/$(PROJECT).hex

.PHONY: all flash flash-stlink clean

all: $(ELF) $(BIN) $(HEX)
	$(SIZE) $(ELF)

$(ELF): $(OBJECTS) $(LINKER) | $(BIN_DIR)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/startup_stm32f103xb.o: $(STARTUP) | $(BUILD_DIR)
	$(CC) $(CPUFLAGS) -c $< -o $@

$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

flash: $(ELF)
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
		-c "program $(ELF) verify reset exit"

flash-stlink: $(BIN)
	st-flash --reset write $(BIN) 0x08000000

clean:
	rm -f $(OBJECTS) $(DEPS)
	rm -f $(ELF) $(BIN) $(HEX) $(BIN_DIR)/$(PROJECT).map

-include $(DEPS)
