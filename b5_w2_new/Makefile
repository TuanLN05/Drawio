PROJECT := b5_w2_new

CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE    := arm-none-eabi-size
OPENOCD ?= openocd

BUILD_DIR := Build
BIN_DIR   := Bin
LINKER    := Linker/STM32F103C8.ld

C_SOURCES := src/main.c \
             Inc/pwm/pwm.c \
             Inc/uart/uart.c \
             src/system_stm32f1xx.c
STARTUP   := src/startup_stm32f103xb.s

OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES)) \
           $(BUILD_DIR)/startup_stm32f103xb.o
DEPS    := $(patsubst %.c,$(BUILD_DIR)/%.d,$(C_SOURCES))

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

OPENOCD_INTERFACE ?= interface/cmsis-dap.cfg
OPENOCD_TARGET    ?= target/stm32f1x.cfg
ADAPTER_SPEED     ?= 1000
DAPLINK_DRIVE     ?=
DAPLINK_MOUNT     ?=

ifeq ($(OS),Windows_NT)
# STM32CubeCLT provides sh.exe, but the Windows recipes below use cmd syntax.
SHELL := cmd.exe
.SHELLFLAGS := /C
make-dir = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
remove-dir = if exist "$(subst /,\,$(1))" rmdir /S /Q "$(subst /,\,$(1))"
else
make-dir = mkdir -p "$(1)"
remove-dir = rm -rf "$(1)"
endif

.PHONY: all flash flash-copy clean

all: $(ELF) $(BIN) $(HEX)
	$(SIZE) $(ELF)

$(ELF): $(OBJECTS) $(LINKER) | $(BIN_DIR)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/%.o: %.c
	@$(call make-dir,$(patsubst %/,%,$(dir $@)))
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/startup_stm32f103xb.o: $(STARTUP) | $(BUILD_DIR)
	$(CC) $(CPUFLAGS) -c $< -o $@

$(BUILD_DIR) $(BIN_DIR):
	@$(call make-dir,$@)

# Flash over the CMSIS-DAP debug interface exposed by DAPLink.
flash: $(ELF)
	$(OPENOCD) -f $(OPENOCD_INTERFACE) -c "transport select swd" -f $(OPENOCD_TARGET) -c "adapter speed $(ADAPTER_SPEED)" -c "program $(ELF) verify reset exit"

# Alternative: copy the raw binary to the DAPLink mass-storage drive.
ifeq ($(OS),Windows_NT)
flash-copy: $(BIN)
	@if "$(DAPLINK_DRIVE)"=="" (echo Usage: make flash-copy DAPLINK_DRIVE=E: & exit /b 1)
	@copy /Y "$(subst /,\,$(BIN))" "$(DAPLINK_DRIVE)\$(notdir $(BIN))"
else
flash-copy: $(BIN)
	@test -n "$(DAPLINK_MOUNT)" || (echo "Usage: make flash-copy DAPLINK_MOUNT=/media/DAPLINK"; exit 1)
	cp "$(BIN)" "$(DAPLINK_MOUNT)/$(notdir $(BIN))"
endif

clean:
	@$(call remove-dir,$(BUILD_DIR))
	@$(call remove-dir,$(BIN_DIR))

-include $(DEPS)
