#include <stdio.h>
#include "ch32fun.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stdarg.h"

#define LED_PIN             PA8
#define TASK1_TASK_PRIO     5
#define TASK1_STK_SIZE      256
#define TASK2_TASK_PRIO     5
#define TASK2_STK_SIZE      256
#define TASK3_TASK_PRIO     configMAX_PRIORITIES - 1
#define TASK3_STK_SIZE      256

TaskHandle_t Task1Task_Handler;
TaskHandle_t Task2Task_Handler;
TaskHandle_t Task3Task_Handler;
SemaphoreHandle_t printMutex;
SemaphoreHandle_t xBinarySem;


__HIGH_CODE
void task1_task(void *pvParameters)
{
	while (1) {
		printf("task1 entry 1\n");
		vTaskDelay(configTICK_RATE_HZ / 4);
		printf("task1 entry 2\n");
		vTaskDelay(configTICK_RATE_HZ / 4);
	}
}

__HIGH_CODE
void task2_task(void *pvParameters)
{
	while (1) {
		printf("task2 entry 1\n");
		vTaskDelay(configTICK_RATE_HZ / 2);
		printf("task2 entry 2\n");
		vTaskDelay(configTICK_RATE_HZ / 2);
	}
}

int main(void) {
	SystemInit();

	funGpioInitAll(); // no-op on ch5xx
	funPinMode( LED_PIN, GPIO_CFGLR_OUT_10Mhz_PP );

	printf("start.\n");

	printMutex = xSemaphoreCreateMutex();
	if(printMutex == NULL)
	{
		printf("printMutex error\n");
		while(1);
	}

	xTaskCreate((TaskFunction_t)task2_task,
				(const char *)"task2",
				(uint16_t)TASK2_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)TASK2_TASK_PRIO,
				(TaskHandle_t *)&Task2Task_Handler);

	xTaskCreate((TaskFunction_t)task1_task,
				(const char *)"task1",
				(uint16_t)TASK1_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)TASK1_TASK_PRIO,
				(TaskHandle_t *)&Task1Task_Handler);

	vTaskStartScheduler();

	while (1) {
		printf("shouldn't run at here!!\n");
	}
}
