local_dir    := lib/CANopen
local_lib    := $(local_dir)/CANopen.a
local_src    := $(addprefix $(local_dir)/src/,                            \
CANopen.c           CO_Emergency.c      CO_OD.c       CO_timer.c          \
CO_HBconsumer.c     CO_PDO.c            CO_SDO.c      CO_SYNC.c           \
CO_NMT_Heartbeat.c  CO_SDOmaster.c      crc16-ccitt.c  stm32/CO_driver.c  \
)
local_obj    := $(patsubst %.c,%.o,$(local_src))
libraries    += $(local_lib)
sources      += $(local_src)

$(local_lib): $(local_obj)
	$(AR) $(ARFLAGS) $@ $^
