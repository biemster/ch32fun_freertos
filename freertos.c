#include <stdio.h>
#include "ch32fun.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stdarg.h"

#ifdef CH570_CH572
#define LED_PIN   PA9
#define TASK3_PIN PA4
#else
#define LED_PIN   PA8
#define TASK3_PIN PA12
#endif

#define TASK1_TASK_PRIO     5
#define TASK1_STK_SIZE      256
#define TASK2_TASK_PRIO     5
#define TASK2_STK_SIZE      256
#define TASK3_TASK_PRIO     configMAX_PRIORITIES - 1
#define TASK3_STK_SIZE      256

TaskHandle_t Task1Task_Handler;
TaskHandle_t Task2Task_Handler;
TaskHandle_t Task3Task_Handler;
SemaphoreHandle_t xBinarySem;


__HIGH_CODE
void task1_task(void *pvParameters) {
	while (1) {
		GPIO_InverseBits(LED_PIN);
		vTaskDelay(configTICK_RATE_HZ / 4);
		GPIO_InverseBits(LED_PIN);
		vTaskDelay(configTICK_RATE_HZ / 4);
	}
}

__HIGH_CODE
void task2_task(void *pvParameters) {
	while (1) {
		printf("task2 entry 1\n");
		vTaskDelay(configTICK_RATE_HZ / 2);
		printf("task2 entry 2\n");
		vTaskDelay(configTICK_RATE_HZ / 2);
	}
}

__HIGH_CODE
void task3_task(void *pvParameters) {
	xBinarySem = xSemaphoreCreateBinary();
	if(xBinarySem != NULL) {
		funPinMode(TASK3_PIN, GPIO_CFGLR_IN_PU);
		R16_PA_INT_MODE |= TASK3_PIN; // edge mode, should go to ch32fun.h
		funDigitalWrite(TASK3_PIN, FUN_LOW); // falling edge
		NVIC_EnableIRQ(GPIOA_IRQn);
		R16_PA_INT_IF = (uint16_t)TASK3_PIN;
		R16_PA_INT_EN |= TASK3_PIN;

		while (1) {
			if(xSemaphoreTake(xBinarySem, portMAX_DELAY) == pdTRUE) {
				printf("task3 sem get ok\n");
			}
			else {
				printf("task3 sem get failed\n");
			}
		}
	}
	else {
		printf("task3 sem init failed\n");
	}
}

int main(void) {
	SystemInit();

	funGpioInitAll(); // no-op on ch5xx
	funPinMode( LED_PIN, GPIO_CFGLR_OUT_10Mhz_PP );

	printf("start.\n");

	xTaskCreate((TaskFunction_t)task1_task,
				(const char *)"task1",
				(uint16_t)TASK1_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)TASK1_TASK_PRIO,
				(TaskHandle_t *)&Task1Task_Handler);

	xTaskCreate((TaskFunction_t)task2_task,
				(const char *)"task2",
				(uint16_t)TASK2_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)TASK2_TASK_PRIO,
				(TaskHandle_t *)&Task2Task_Handler);

	xTaskCreate((TaskFunction_t)task3_task,
				(const char *)"task3",
				(uint16_t)TASK3_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)TASK3_TASK_PRIO,
				(TaskHandle_t *)&Task3Task_Handler);

	vTaskStartScheduler();

	while (1) {
		printf("shouldn't run at here!!\n");
	}
}

__HIGH_CODE
__INTERRUPT
void GPIOA_IRQHandler(void) {
	int status = R16_PA_INT_IF;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if(status & TASK3_PIN) {
		xSemaphoreGiveFromISR(xBinarySem, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	R16_PA_INT_IF = status; // acknowledge
}
