CROSS_COMPILE := riscv32-unknown-linux-gnu-
COMMON_FLAGS  := -fno-pic -march=rv32ia_zicsr -mabi=ilp32
CFLAGS        += $(COMMON_FLAGS) -static
ASFLAGS       += $(COMMON_FLAGS) -O2
LDFLAGS       += -melf32lriscv
