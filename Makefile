all : flash

TARGET:=freertos
TARGET_MCU:=CH592
TARGET_MCU_PACKAGE:=CH592F
LINKER_SCRIPT:=freertos.ld

EXTRA_CFLAGS += \
	-I"./FreeRTOS" \
	-I"./FreeRTOS/include" \
	-I"./FreeRTOS/portable/GCC/RISC-V"

ADDITIONAL_C_FILES += \
	./FreeRTOS/croutine.c \
	./FreeRTOS/event_groups.c \
	./FreeRTOS/list.c \
	./FreeRTOS/queue.c \
	./FreeRTOS/stream_buffer.c \
	./FreeRTOS/tasks.c \
	./FreeRTOS/timers.c \
	./FreeRTOS/portable/GCC/RISC-V/port.c \
	./FreeRTOS/portable/MemMang/heap_4.c

include ~/temp/CH570/ch32fun/ch32fun/ch32fun.mk

FILES_TO_COMPILE += \
	./startup_CH592.S \
	./FreeRTOS/portable/GCC/RISC-V/portASM.S

%.o: ./%.S
	${PREFIX}-gcc \
	$(CFLAGS) \
	-x assembler-with-cpp \
	-MMD \
	-MP \
	-MF"$(@:%.o=%.d)" \
	-MT"$(@)" \
	-c \
	-o "$@" "$<"

flash : cv_flash
clean : cv_clean
