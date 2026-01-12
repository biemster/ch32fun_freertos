all : flash

TARGET:=freertos
TARGET_MCU:=CH592
TARGET_MCU_PACKAGE:=CH592F

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
	./FreeRTOS/portable/MemMang/heap_4.c \
	./FreeRTOS/portable/GCC/RISC-V/portASM.S

include ~/temp/ch32fun/ch32fun/ch32fun.mk

flash : cv_flash
clean : cv_clean
