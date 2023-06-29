#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "comunicaciones.h"
#include "main.h"
#include "tareas.h"


extern UART_HandleTypeDef huart1;
char	buff_sprintf100[100];
uint8_t buff_sprintf100_1[100];

void verifyIP() {
	unsigned int ct;

	// verifica si hay IP
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1, ( unsigned char *)"AT+CIFSR\r\n",strlen("AT+CIFSR\r\n"),10000);
	//for (ct=0;ct<2000000;ct++);
	osDelay(500);
	HAL_UART_DMAStop(&huart1);
	//printf("5: %s",buff_recv);
}

char* sendMessage(HTTP_Request httpRequest) {
	unsigned int ct;

	// abrir conexión con
	HAL_UART_DMAStop(&huart1);
	HAL_UART_Init(&huart1);
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	sprintf(buff_sprintf100,"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",httpRequest.dst, httpRequest.port);
    //snprintf(buff_sprintf100, sizeof(buff_sprintf100), "AT+CIPSTART=\"TCP\",\"%s\",%s\r\n", httpRequest.dst, httpRequest.port);
	HAL_UART_Transmit(&huart1, ( unsigned char *)			buff_sprintf100,strlen(buff_sprintf100),10000);
	Task_TimeOut(2000,buff_recv,"CONNECT\r\n");
	HAL_UART_DMAStop(&huart1);
	//printf("6: %s",buff_recv);

	// enviar una peticion HTTP
	HAL_UART_Init(&huart1);
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	sprintf(buff_sprintf100_1,"AT+CIPSEND=%d\r\n",strlen(httpRequest.format));


	HAL_UART_Transmit(&huart1,buff_sprintf100_1,strlen(buff_sprintf100_1),10000);
    osDelay(10);
	Task_TimeOut(1000,buff_recv,">");
	HAL_UART_DMAStop(&huart1);
	//printf("7: %s",buff_recv);

	// ahora HTTP
	HAL_UART_Init(&huart1);
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1,httpRequest.format,strlen(httpRequest.format),10000);
	Task_TimeOut(2000,buff_recv,"reasonPhrase");
	osDelay(50);
	HAL_UART_DMAStop(&huart1);
	 cleanResponse(( unsigned char *)buff_recv,2048);
	//printf("8: %s",buff_recv);

	 HAL_UART_Init(&huart1);
	 sprintf(buff_send,( unsigned char *)"AT+CIPCLOSE\r\n");
	 //HAL_UART_Transmit(UART_ESP8266,( unsigned char *) buff_sprintf100_1, strlen( buff_sprintf100_1),100000); // cabecera
	 HAL_UART_Transmit_DMA(&huart1, ( unsigned char *) buff_send, strlen( buff_send));
	 osDelay(10);

	return buff_recv;
}

void Task_TimeOut(TickType_t timeout,char *src,char * resp) {
	TickType_t localtimeout=xTaskGetTickCount();
	int ct;

	localtimeout=xTaskGetTickCount();
	while ((strstr(src,resp)==NULL)&&((xTaskGetTickCount()-localtimeout)<(timeout/portTICK_RATE_MS)))

	if ((xTaskGetTickCount()-localtimeout)>=(timeout/portTICK_RATE_MS)){
		 getWifiIP();
       	}
}

void getWifiIP() {
	unsigned int ct;

	getVersion();

	// version
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1, ( unsigned char *)"AT+GMR\r\n",strlen("AT+GMR\r\n"),10000);
	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_SET);
	//osDelay(1000);
	Task_TimeOut(1000,buff_recv,"AT version");
	HAL_UART_DMAStop(&huart1);
	HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_SET);

	//printf("2: %s",buff_recv);


	// Pon en modo station=1,  station+access_point=3
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1, ( unsigned char *) "AT+CWMODE=1\r\n",strlen("AT+CWMODE=1\r\n"),100000);
	//osDelay(4000);
	Task_TimeOut(4000,buff_recv,"OK\r\n");
	HAL_UART_DMAStop(&huart1);
	//printf("3: %s",buff_recv);

	// Programa la contraseña del access-point
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1,( unsigned char *) "AT+CWJAP=\"" SSID "\",\"" SSID_PASS "\"\r\n",strlen("AT+CWJAP=\"" SSID "\",\"" SSID_PASS "\"\r\n"),10000);
	//osDelay(7000);
	Task_TimeOut(7000,buff_recv,"WIFI GOT IP");
	HAL_UART_DMAStop(&huart1);
	//printf("4: %s",buff_recv);
}

void getVersion() {
	unsigned int ct;

 	printf("Reseting...\r\n");

 	ESP8266_RESET();
 	printf("Init...\r\n");

 	HAL_UART_Init(&huart1);

 	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_RESET);
 	HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_RESET);

 	// version
 	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
 	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
 	HAL_UART_Transmit(&huart1, ( unsigned char *)"AT\r\n",strlen("AT\r\n"),10000);
 	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_SET);
 	osDelay(1000);
	//Task_TimeOut(1000,buff_recv,"OK\r\n");
 	HAL_UART_DMAStop(&huart1);
 	HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_SET);
	//printf("1: %s",buff_recv);
}

void ESP8266_RESET(void){
	// RESET
	int ct;
	 HAL_UART_DMAStop(UART_ESP8266);
	 HAL_UART_Init(UART_ESP8266);
	 	        	 for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	 	        	 HAL_UART_Receive_DMA(UART_ESP8266, buff_recv,2048);

		HAL_GPIO_WritePin(ESP8266_RESET_GPIO_Port, ESP8266_RESET_Pin, GPIO_PIN_RESET);
	  	 osDelay(100);
	  	HAL_GPIO_WritePin(ESP8266_RESET_GPIO_Port, ESP8266_RESET_Pin, GPIO_PIN_SET);
		osDelay(3000);
		HAL_UART_DMAStop(UART_ESP8266);
		printf("XXXXX",buff_recv);
		printf("XXXXX","\r\n\n\n\nXXX1");
}

