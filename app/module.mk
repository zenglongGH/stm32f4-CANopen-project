local_dir    := app
local_prg    := bin/out
local_asm    := $(local_dir)/startup_stm32f4xx.s # startup code 
local_src    := $(shell find $(local_dir)/src -name *.c | tr '\n' ' ')
local_obj    := $(patsubst %.c,%.o,$(local_src))
programs     += $(addprefix $(local_prg),.elf .hex .bin)
sources      += $(local_src)

$(local_prg).elf: $(local_obj) $(local_asm) $(libraries)
	$(CC) $(CFLAGS) $(LDFLAGS) $(IPATHS) $(LPATHS) $^ -lc -lm -o $@

$(local_prg).hex: $(local_prg).elf
	$(OBJCOPY) -O binary $^ $@

$(local_prg).bin: $(local_prg).elf
	$(OBJCOPY) -O ihex $^ $@
