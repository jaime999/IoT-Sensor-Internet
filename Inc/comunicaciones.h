#ifndef __COMUNICACIONES_H__
#define __COMUNICACIONES_H__

#include "main.h"

#define SSID			  "routerSEU"
#define SSID_PASS		  "00000000"


//global variables
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;

extern char buff_recv[2048];
extern char buff_send[2048];

void ESP8266_RESET(void);
void ESP8266_Boot(void);
int _write(int file, char *ptr, int len);

#endif
