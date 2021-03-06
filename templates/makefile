#########################################################
# project files
#########################################################
TARGET =targetname
PRJPATH =.
OBJPATH =.
CSRCPATH =.
INCSPATH =.
CSRCS =
GCSYMBOLS =
#########################################################
# TOOLCHAIN
#########################################################
GCC_EXEC_PREFIX =arm-none-eabi
GCC = $(GCC_EXEC_PREFIX)-gcc
GPP = $(GCC_EXEC_PREFIX)-g++
AS = $(GCC_EXEC_PREFIX)-as
LD = $(GCC_EXEC_PREFIX)-ld
SIZE = $(GCC_EXEC_PREFIX)-size
OBJCOPY = $(GCC_EXEC_PREFIX)-objcopy
OBJDUMP = $(GCC_EXEC_PREFIX)-objdump
OPENOCD = openocd
REMOVE = rm -f
#########################################################
#Startup files and libraries
#########################################################
STARTUP   =startup
GCFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Og -g3 -Wall -fmessage-length=0 -ffunction-sections
#GCFLAGS =-O3 -mcpu=cortex-m3 -mthumb -Wall -g
LDFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Wl,--gc-sections
LIBS =-nostdlib #-lgcc -lc -lnosys
CSRCS   +=system_stm32f1xx.c
ASRCS   +=startup_stm32f103xb.s
LDSCRIPT ="startup/STM32F103C8Tx_FLASH.ld"
##########################################################
OBJECTS =$(addprefix $(OBJPATH)/, $(CSRCS:.c=.o)) $(addprefix $(OBJPATH)/,$(ASRCS:.s=.o))
VPATH += $(CSRCPATH) $(BSPPATH) $(LIBSPATH)

all: $(TARGET).bin stats

	
$(TARGET).axf:  $(OBJECTS)
	@echo "---- Linking ---->" $@
	$(GCC) $(LDFLAGS) -T$(LDSCRIPT) $(addprefix -L, $(LIBSPATH)) -o "$(TARGET).axf" $(OBJECTS) $(LIBS)
#arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -T"../STM32F103C8Tx_FLASH.ld" -Wl,-Map=output.map -Wl,--gc-sections -o "TDSO.elf" @"objects.list"   -lm

$(TARGET).hex: $(TARGET).axf
	@$(OBJCOPY) -O ihex -j .text -j .data $(TARGET).axf $(TARGET).hex

$(TARGET).bin: $(TARGET).axf
	@$(OBJCOPY) -O binary -j .text -j .data $(TARGET).axf $(TARGET).bin

stats: $(TARGET).axf
	@echo "---- Sections Summary ---"
	@$(SIZE) -A -x $<

aslist: $(TARGET).axf
	@$(OBJDUMP) -D $(TARGET).axf > $(TARGET).lst
	
dis: $(TARGET).axf
	@$(OBJDUMP) -S $(TARGET).axf > $(TARGET).lst

clean:
	$(REMOVE) $(OBJECTS) $(XOBJS) $(TARGET) $(TARGET).exe $(TARGET).hex $(TARGET).axf *.bin
	$(MAKE) -C $(XLIBPATH) clean
	

rebuild: clean all

debug:	$(TARGET).axf
	$(DBG) $(TARGET).axf
	
doc:
	doxygen difs.txt

$(TARGET).jlink:
	@echo "Creating Jlink configuration file"
	echo "erase\nloadbin $(TARGET).bin , 0x8000000\nr\nq" > $(TARGET).jlink
	
flash: $(TARGET).jlink #tdso.bin #$(TARGET).bin
	$(JLINK) -device $(DEVICE) -if SWD -speed auto -CommanderScript $(TARGET).jlink

$(TARGET).cfg:
	@echo "Creating opencod configuration file"
	echo "interface jlink\ntransport select swd\nsource [find target/stm32f1x.cfg]\nadapter_khz 4000" > $(TARGET).cfg

program: $(TARGET).axf $(TARGET).cfg
	$(OPENOCD) -f $(TARGET).cfg -c "program $(TARGET).axf verify reset exit"


$(OBJPATH)/%.o : %.c
	@echo "---- Compile" $< "---->" $@
	$(GCC) $(GCFLAGS) $(addprefix -I, $(INCSPATH)) $(addprefix -D, $(GCSYMBOLS))  -c $< -o $@
	
$(OBJPATH)/%.o : %.s
	@echo "---- Assemble" $< "---->" $@
	@$(AS) $(ASFLAGS) $< -o $@

.PHONY: doc
	
