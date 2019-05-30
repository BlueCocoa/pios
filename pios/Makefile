ARMGNU ?= aarch64-linux-gnu

COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only
ASMOPS = -Iinclude 

BUILD_DIR = build
OBJS_DIR = objs
SRC_DIR = src

all : kernel8.img

clean :
	rm -rf $(OBJS_DIR) *.img 

$(OBJS_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@
	
$(OBJS_DIR)/%_cpp.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(ARMGNU)-g++ -std=c++14 $(COPS) -MMD -c $< -o $@

$(OBJS_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
CPP_FILES = $(wildcard $(SRC_DIR)/*.cpp)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(ASM_FILES:$(SRC_DIR)/%.S=$(OBJS_DIR)/%_s.o)
OBJ_FILES += $(CPP_FILES:$(SRC_DIR)/%.cpp=$(OBJS_DIR)/%_cpp.o)
OBJ_FILES += $(C_FILES:$(SRC_DIR)/%.c=$(OBJS_DIR)/%_c.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(OBJS_DIR)/kernel8.elf  $(OBJ_FILES)
	$(ARMGNU)-objcopy $(OBJS_DIR)/kernel8.elf -O binary $(BUILD_DIR)/kernel8.img
