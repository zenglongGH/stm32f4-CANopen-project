# Initialize variables for nonrecursive make

programs     :=
sources      :=
libraries    :=
objects       = $(patsubst %.c,%.o,$(sources))
dependencies  = $(patsubst %.c,%.d,$(sources))

# Define header file and library directories

include_dirs := app/inc 
include_dirs += app/inc/periphs 
include_dirs += lib/CMSIS/Include 
include_dirs += lib/CMSIS/Device/ST/STM32F4xx/Include
include_dirs += lib/STM32F4xx_StdPeriph_Driver/inc

library_dirs := lib

vpath %.h $(include_dirs)

# Define toolchains and build options  

AR           := ar
SED          := sed
CP           := cp
MV           := mv
RM           := rm -f
CC           := arm-none-eabi-gcc
LD           := arm-none-eabi-gcc
OBJCOPY      := arm-none-eabi-objcopy

CFLAGS       := -O0 #-Werror
CFLAGS       += -mlittle-endian -mthumb -mcpu=cortex-m4 -fsingle-precision-constant
CFLAGS       += -mthumb-interwork -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS       += -DUSE_STDPERIPH_DRIVER -DSTM32F4XX
CFLAGS       += -DMANGUSTA_DISCOVERY -DHSE_VALUE=8000000
LDFLAGS      := -fno-exceptions -ffunction-sections -fdata-sections
LDFLAGS      += -nostartfiles -Wl,--gc-sections -Tstm32_flash.ld
ARFLAGS      := rcv
IPATHS        = $(addprefix -I,$(include_dirs))
LPATHS        = $(addprefix -L,$(library_dirs))

# Include make modules

all:
include lib/STM32F4xx_StdPeriph_Driver/module.mk
include app/module.mk

ifneq "$(MAKECMDGOALS)" "clean"
-include $(dependencies)
endif

# Define additional rules

all: $(programs) 

#l_ankle l_knee l_hip r_ankle r_knee r_hip teststand: clean
#	cd app/inc && \
#	rm -f config.h && \
#	ln -s ../../target/$@/config.h . && \
#	cd ../..
#	cd app/src/mechanics && \
#	rm -f mechanics.c && \
#	ln -s ../../../target/$@/mechanics.c . && \
#	cd ../../..

libraries: $(libraries)

clean:
	$(RM) $(programs) app/src/*.o app/src/*.d app/src/*~ \
	./*~ app/*~

realclean: clean
	$(RM) $(libraries) $(objects) $(dependencies)

%.o: %.c
	$(CC) -c $(CFLAGS) $(IPATHS) $< -o $@

%.d: %.c
	$(CC) $(CFLAGS) $(IPATHS) -M $< | \
	$(SED) 's,\($(notdir $*)\.o\) *:,$(dir $@)\1 $@: ,' > $@.tmp
	$(MV) $@.tmp $@

.PHONY: all libraries clean realclean 
