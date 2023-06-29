#include "tareas.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "comunicaciones.h"
#include <stdbool.h>
#include "semphr.h"
#include "queue.h"
#include "cJSON.h"
#include "main.h"


SemaphoreHandle_t xSemaphoreConnection;
SemaphoreHandle_t xSemaphoreTest;
SemaphoreHandle_t xSemaphoreOrion;
TaskHandle_t xTaskTestHandle;
TaskHandle_t xTaskReadHandle;
TaskHandle_t xTaskWriteHandle;
static QueueHandle_t xQueueRequestRead = NULL;
static QueueHandle_t xQueueRequestWrite = NULL;
static QueueHandle_t xQueueRequestTime = NULL;
static QueueHandle_t xQueueResponseRead = NULL;
static QueueHandle_t xQueueResponseWrite = NULL;
static QueueHandle_t xQueueResponseTime = NULL;
char *modes[] = {TEST, CONECTADO, CLON};
char* mode = CONECTADO;
uint8_t request[2048];
uint8_t bufferBody[2048];
double pot;
double temp;
double maxTemp;
double minTemp;
double tempAlarm;
double ldr;
double maxLdr;
double minLdr;
double ldrAlarm;
char* alarm = "F";
int alarmCont = 0;
char* actualAlarmSrc;
char delimiter[] = ",";

Pin pines[8] = {
    {GPIOB, D1_Pin},
    {GPIOB, D2_Pin},
    {GPIOA, D3_Pin},
    {GPIOB, D4_Pin},
    {GPIOB, D5_Pin},
    {GPIOA, D6_Pin},
    {GPIOB, D7_Pin},
    {GPIOA, D8_Pin}
};

HTTP_Request httpRequestRead = {
		"pperez-seu-or.disca.upv.es",
		"1026"
};

HTTP_Request httpRequestWrite = {
		"pperez-seu-or.disca.upv.es",
		"1026"
};

HTTP_Request httpRequestTime = {
		"worldtimeapi.org",
		"80",
		"GET /api/timezone/Europe/Madrid HTTP/1.1\r\n\r\n"
};

HTTP_Request pxRxedMessage;


void CONFIGURACION_INICIAL(void){
	BaseType_t res_task;
	xSemaphoreConnection = xSemaphoreCreateMutex();
	xSemaphoreTest = xSemaphoreCreateMutex();
	xSemaphoreOrion = xSemaphoreCreateMutex();
	xQueueRequestRead = xQueueCreate(10, sizeof( HTTP_Request ));
	xQueueResponseRead = xQueueCreate(10, sizeof( unsigned  char* ));
	xQueueRequestWrite = xQueueCreate(10, sizeof( HTTP_Request ));
	xQueueResponseWrite = xQueueCreate(10, sizeof( unsigned  char* ));
	xQueueRequestTime = xQueueCreate(10, sizeof( HTTP_Request ));
	xQueueResponseTime = xQueueCreate(10, sizeof( unsigned  char* ));
	res_task=xTaskCreate(Task_MODE,"MODE",2048,NULL,NORMAL_PRIORITY,NULL);
	 		if( res_task != pdPASS ){
	 				printf("PANIC: Error al crear Tarea Mode\r\n");
	 				fflush(NULL);
	 				while(1);
	 		}

res_task=xTaskCreate(Task_HW,"HW",2048,NULL,	 NORMAL_PRIORITY,NULL);
	 		 		if( res_task != pdPASS ){
	 		 				printf("PANIC: Error al crear Tarea Hardware\r\n");
	 		 				fflush(NULL);
	 		 				while(1);
	 		 		}

 res_task=xTaskCreate(Task_TEST,"TEST",2048,NULL,	 NORMAL_PRIORITY,&xTaskTestHandle);
 		if( res_task != pdPASS ){
 				printf("PANIC: Error al crear Tarea Test\r\n");
 				fflush(NULL);
 				while(1);
 		}

 res_task=xTaskCreate(Task_WIFI,"WIFI",2048,NULL,	 NORMAL_PRIORITY,NULL);
 			if( res_task != pdPASS ){
 					printf("PANIC: Error al crear Tarea Wifi\r\n");
 					fflush(NULL);
 					while(1);
 			}

 res_task=xTaskCreate(Task_COMM,"COMM",2048,NULL,	 NORMAL_PRIORITY ,NULL);
 			 			if( res_task != pdPASS ){
 			 					printf("PANIC: Error al crear Tarea Comunicación\r\n");
 			 					fflush(NULL);
 			 					while(1);
 			 			}

 res_task=xTaskCreate(Task_TIME,"TIME",2048,NULL,	 NORMAL_PRIORITY ,NULL);
 			 			if( res_task != pdPASS ){
 			 			 		printf("PANIC: Error al crear Tarea Time\r\n");
 			 			 		fflush(NULL);
 			 			 		while(1);
 			 			}

res_task=xTaskCreate(Task_WRITE,"WRITE",2048,NULL,	 NORMAL_PRIORITY ,&xTaskWriteHandle);
			 			if( res_task != pdPASS ){
			 			 		printf("PANIC: Error al crear Tarea Write\r\n");
			 			 		fflush(NULL);
			 			 		while(1);
			 			}

res_task=xTaskCreate(Task_READ,"READ",2048,NULL,	 NORMAL_PRIORITY ,&xTaskReadHandle);
			 			if( res_task != pdPASS ){
			 				printf("PANIC: Error al crear Tarea Read\r\n");
			 				fflush(NULL);
			 				while(1);
			 			}
}

void Task_MODE( void *pvParameters) {
	xSemaphoreTake(xSemaphoreConnection, portMAX_DELAY);
	xSemaphoreTake(xSemaphoreTest, portMAX_DELAY);
	if(mode == TEST) {
		vTaskSuspend(xTaskWriteHandle);
		vTaskSuspend(xTaskReadHandle);
		//xSemaphoreGive(xSemaphoreTest);
	}

	if(mode == CONECTADO || mode == CLON) {
		xSemaphoreGive(xSemaphoreConnection);
		vTaskSuspend(xTaskTestHandle);
	}

	if(mode == CLON) {
		vTaskSuspend(xTaskTestHandle);
		vTaskSuspend(xTaskWriteHandle);
	}
	bool button2Set = false;
	bool button1Set = true;
    int currentModeIndex = 0;
	for(;;) {
		if (HAL_GPIO_ReadPin(PULSADOR2_GPIO_Port,PULSADOR2_Pin) == GPIO_PIN_SET)
		    button2Set = true;

		if(HAL_GPIO_ReadPin(PULSADOR2_GPIO_Port,PULSADOR2_Pin) == GPIO_PIN_RESET && button2Set) {
			currentModeIndex = (currentModeIndex + 1) % 3;
			mode = modes[currentModeIndex];
			button2Set = false;

			if(mode == TEST) {
				vTaskResume(xTaskTestHandle);
				vTaskSuspend(xTaskWriteHandle);
				vTaskSuspend(xTaskReadHandle);
			}

			if(mode == CONECTADO || mode == CLON) {
				  // Obtener el identificador de la tarea actual
				  TaskHandle_t currentTaskHandle = xTaskGetCurrentTaskHandle();

				  // Verificar si la tarea actual tiene un semáforo adquirido
				  TaskHandle_t mutexHolder = xSemaphoreGetMutexHolder(xSemaphoreConnection);

				  if (mutexHolder == currentTaskHandle) {
				    xSemaphoreGive(xSemaphoreConnection);
				  }

				vTaskResume(xTaskWriteHandle);
				vTaskResume(xTaskReadHandle);
				vTaskSuspend(xTaskTestHandle);
			}

				if(mode == CLON) {
					vTaskSuspend(xTaskWriteHandle);
			}
		}

		if (HAL_GPIO_ReadPin(PULSADOR1_GPIO_Port,PULSADOR1_Pin) == GPIO_PIN_SET)
		    button1Set = true;

		if(HAL_GPIO_ReadPin(PULSADOR1_GPIO_Port,PULSADOR1_Pin) == GPIO_PIN_RESET && button1Set) {
			if(HAL_GPIO_ReadPin(GPIOA,Buzzer_Pin) == GPIO_PIN_RESET) {
				printf("La cosa está en el modo %s\r\n", mode);
			}

			else {
				HAL_GPIO_WritePin(GPIOA, Buzzer_Pin, GPIO_PIN_RESET);
				if(mode == CLON) {
					vTaskResume(xTaskWriteHandle);
					vTaskSuspend(xTaskReadHandle);
					getWriteInfo(DEVICE_TO_CLONE, mode);
					xQueueSendToBack( xQueueRequestWrite,    ( void * ) &httpRequestWrite,    ( TickType_t ) 10 );
				}
			}

			button1Set = false;
		}
	}
}

void Task_HW( void *pvParameters ){
	bool button1Set = false;
	bool ldrPressed = true;
	double LEDSensor = 0.0;
	GPIO_PinState button1;
	for(;;)
	  {
		checkIfBuzzerSet(LEDSensor, getAnalogDataPot());

		button1=HAL_GPIO_ReadPin(PULSADOR1_GPIO_Port,PULSADOR1_Pin);
		if(button1 == GPIO_PIN_SET) {
			button1Set = true;
		}

		if(button1 == GPIO_PIN_RESET && button1Set) {
			button1Set = false;
			resetBuzzer();
		}

	    osDelay(1);
	  }
}

void Task_TEST( void *pvParameters ){
	for(;;) {
		testMain(pines);
		/*if(xSemaphoreTake(xSemaphoreTest, 20000/portTICK_RATE_MS  ) == pdPASS) {
			xSemaphoreGive(xSemaphoreTest);
		}*/
		osDelay(10);
	}
}

void Task_WIFI( void *pvParameters ){
	xSemaphoreTake(xSemaphoreOrion, portMAX_DELAY);
	for(;;) {
		if (xSemaphoreTake(xSemaphoreConnection, 20000/portTICK_RATE_MS  ) == pdPASS ){
			//printf("%s...\r\n", "Conectando");
			setupConnection();
		}
	    osDelay(10);
	  }
}

void Task_COMM( void *pvParameters ){
	char* response;
	char* messagePublished = "PUBLISHED";
	httpRequestRead.response = (char*)malloc(2048);
	for(;;)
	  {
			if(xQueueReceive( xQueueRequestRead,
			                         &(pxRxedMessage),
			                         ( TickType_t ) 10 ) == pdPASS) {
				response = sendMessage(pxRxedMessage);
			    if (strstr(response, "ERROR") != NULL) {
			    	xSemaphoreGive(xSemaphoreConnection);

			    }

			    else {
			    	strcpy(httpRequestRead.response, buff_recv);
			    	xQueueSendToBack( xQueueResponseRead,    ( void * ) &httpRequestRead.response,    ( TickType_t ) 10 );
			    }
			    //HAL_NVIC_SystemReset();
			}

			if(xQueueReceive( xQueueRequestWrite,
						                        &( pxRxedMessage ),
						                        ( TickType_t ) 10 ) == pdPASS) {
				sendMessage(pxRxedMessage);
			    if (strstr(buff_recv, "ERROR") != NULL) {
			    	xSemaphoreGive(xSemaphoreConnection);
			    }

			    else {
					xQueueSendToBack( xQueueResponseWrite,    ( void * ) &messagePublished,    ( TickType_t ) 10 );
			    }
						    //HAL_NVIC_SystemReset();
			}

			if(xQueueReceive( xQueueRequestTime,
						                        &( pxRxedMessage ),
						                        ( TickType_t ) 10 ) == pdPASS) {
				response = sendMessage(pxRxedMessage);
			    if (strstr(response, "ERROR") != NULL) {
			    	xSemaphoreGive(xSemaphoreConnection);
			    }

			    else {
					xQueueSendToBack( xQueueResponseTime,    ( void * ) &response,    ( TickType_t ) 10 );
			    }
			}
			osDelay(10);
		}
}

void Task_TIME( void *pvParameters ){
	char *responseMessage;
	bool requestSended = false;
	for(;;)
	  {
		if(xSemaphoreTake(xSemaphoreOrion, 20000/portTICK_RATE_MS  ) == pdPASS) {
			if(!requestSended) {
				xQueueSendToBack( xQueueRequestTime,    ( void * ) &httpRequestTime,    ( TickType_t ) 10 );
			}
			requestSended = true;
			xSemaphoreGive(xSemaphoreOrion);
		}

		if(xQueueReceive( xQueueResponseTime,
		                         &( responseMessage ),
		                         ( TickType_t ) 10 ) == pdPASS) {
			if(mode == CONECTADO) {
				parseTime(strchr(responseMessage, '{'));
			}
		    requestSended = false;
		}

	    osDelay(10);
	  }
}

void Task_READ( void *pvParameters ){
	char *responseMessage;
	bool requestSended = false;
	httpRequestRead.format = malloc(2048);
	for(;;)
	  {
		if(xSemaphoreTake(xSemaphoreOrion, 20000/portTICK_RATE_MS  ) == pdPASS) {
			if(!requestSended) {
			    if(mode == CONECTADO) {
			    	showInfo(MY_DEVICE);
			    }

			    if(mode == CLON) {
			    	showInfo(DEVICE_TO_CLONE);
			    }

				xQueueSendToBack( xQueueRequestRead,    ( void * ) &httpRequestRead,    ( TickType_t ) 10 );
				requestSended = true;
			}
			xSemaphoreGive(xSemaphoreOrion);
		}

		if(xQueueReceive( xQueueResponseRead,
								 &( responseMessage ),
								 ( TickType_t ) 10 ) == pdPASS) {
			if(mode == CONECTADO) {
				parseMessage(responseMessage, mode);
			}

			if(mode == CLON) {
				parseMessage(responseMessage, mode);
			}
			requestSended = false;
		}

	    osDelay(10);
	  }
}

void Task_WRITE( void *pvParameters ){
	bool requestSended = false;
	char* responseMessage;
	httpRequestWrite.format = malloc(2048);
	for(;;)
	  {
			if(xSemaphoreTake(xSemaphoreOrion, 20000/portTICK_RATE_MS  ) == pdPASS) {
					if(!requestSended) {
						getWriteInfo(MY_DEVICE, mode);
						xQueueSendToBack( xQueueRequestWrite,    ( void * ) &httpRequestWrite,    ( TickType_t ) 10 );
						requestSended = true;
					}
					xSemaphoreGive(xSemaphoreOrion);
			}

			if(xQueueReceive( xQueueResponseWrite,
									&( responseMessage ),
									( TickType_t ) 10 ) == pdPASS) {
				printf(MESSAGE_SENDED_WRITE);
				requestSended = false;
				if(mode == CLON) {
					//xSemaphoreGive(xSemaphoreOrion);
					vTaskResume(xTaskReadHandle);
					vTaskSuspend(xTaskWriteHandle);
				}
			}
	    osDelay(10);
	  }
}

void setupConnection() {
	getWifiIP();
	verifyIP();
	xSemaphoreGive(xSemaphoreOrion);
}


