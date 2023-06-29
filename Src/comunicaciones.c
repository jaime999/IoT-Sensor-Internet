/*
 * comunicaciones.c
 *
 *  Created on: Apr 27, 2023
 *      Author: pperez
 */

#include <stdio.h>
#include <string.h>
#include "comunicaciones.h"
#include "main.h"
#include <tareas.h>

int _write(int file, char *ptr, int len)
{
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
	    //ITM_SendChar( *ptr++ );
	   HAL_UART_Transmit(&huart2, (uint8_t*)ptr++,1,1000);
	}

	return len;
}


void ESP8266_RESET_Old(void){
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


void ESP8266_Boot(void)

{
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
 	for (ct=0;ct<2000000;ct++);
 	HAL_UART_DMAStop(&huart1);
 	HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_SET);
	printf("1: %s",buff_recv);


	// version
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1, ( unsigned char *)"AT+GMR\r\n",strlen("AT+GMR\r\n"),10000);
	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_SET);
	for (ct=0;ct<2000000;ct++);
	HAL_UART_DMAStop(&huart1);
	HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_SET);

	printf("2: %s",buff_recv);


	// Pon en modo station=1,  station+access_point=3
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1, ( unsigned char *) "AT+CWMODE=1\r\n",strlen("AT+CWMODE=1\r\n"),100000);
	for (ct=0;ct<2000000;ct++);
	HAL_UART_DMAStop(&huart1);
	printf("3: %s",buff_recv);

	// Programa la contraseña del access-point
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1,( unsigned char *) "AT+CWJAP=\"" SSID "\",\"" SSID_PASS "\"\r\n",strlen("AT+CWJAP=\"" SSID "\",\"" SSID_PASS "\"\r\n"),10000);
	for (ct=0;ct<10000000;ct++);
	HAL_UART_DMAStop(&huart1);
	printf("4: %s",buff_recv);

	// verifica si hay IP
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1, ( unsigned char *)"AT+CIFSR\r\n",strlen("AT+CIFSR\r\n"),10000);
	for (ct=0;ct<2000000;ct++);
	HAL_UART_DMAStop(&huart1);
	printf("5: %s",buff_recv);

	// abrir conexión con
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1, ( unsigned char *)			"AT+CIPSTART=\"TCP\",\"worldtimeapi.org\",80\r\n",strlen("AT+CIPSTART=\"TCP\",\"worldtimeapi.org\",80\r\n"),10000);
	for (ct=0;ct<2000000;ct++);
	HAL_UART_DMAStop(&huart1);
	printf("6: %s",buff_recv);

	// enviar una peticion HTTP

	char GETHTTP[]="GET /api/timezone/Europe/Madrid HTTP/1.1\r\n\r\n";
	int lc=strlen(GETHTTP);

	sprintf(buff_send,"AT+CIPSEND=%d\r\n",lc);

	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);

	HAL_UART_Transmit(&huart1,buff_send,strlen(buff_send),10000);
	for (ct=0;ct<2000000;ct++);
	HAL_UART_DMAStop(&huart1);
	printf("7: %s",buff_recv);

	// ahora HTTP

	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1,GETHTTP,strlen(GETHTTP),10000);
	for (ct=0;ct<100000000;ct++);
	HAL_UART_DMAStop(&huart1);
	printf("8: %s",buff_recv);








}


