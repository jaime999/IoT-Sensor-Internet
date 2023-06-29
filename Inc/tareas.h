#ifndef __TAREAS_H__
#define __TAREAS_H__
#include "FreeRTOS.h"
#include "main.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#define HIGH_PRIORITY (( configMAX_PRIORITIES - 1 )>>1)+1
#define NORMAL_PRIORITY (( configMAX_PRIORITIES - 1 )>>1)
#define LOW_PRIORITY (( configMAX_PRIORITIES - 1 )>>1)-1
#define UART_ESP8266  &huart1
#define MY_DEVICE "SensorSEU_JMS"
#define DEVICE_TO_CLONE "SensorSEU_PPB22"
#define BODY_JSON_WRITE "{ \"contextElements\": [{\"type\": \"Sensor\", \"isPattern\": \"false\",\"id\": \"%s\", \"attributes\": [{\"name\": \"Alarma\", \"type\": \"boolean\", \"value\": \"%s\"}, {\"name\": \"IntensidadLuz\", \"type\": \"floatArray\", \"value\": \"%f,%f,%f,%f\"}, {\"name\": \"Temperatura\", \"type\": \"boolean\", \"value\": \"%f,%f,%f,%f\"}]}], \"updateAction\": \"APPEND\"}"
#define BODY_JSON_ALARM       "{ \"contextElements\": [{\"type\": \"Sensor\", \"isPattern\": \"false\",\"id\": \"%s\",\"attributes\": [ {\"name\": \"Alarma_src\",\"type\": \"string\",\"value\": \"%s_%d\"}]}],\"updateAction\": \"APPEND\"}"
#define COMPLETE_STRING_WRITE "POST /v1/updateContext HTTP/1.1\r\nContent-Type: application/json\r\nAccept: application/json\r\nContent-Length: %d\r\n\r\n%s"
#define COMPLETE_STRING_READ "POST /v1/queryContext HTTP/1.1\r\nContent-Type: application/json\r\nAccept: application/json\r\nContent-Length: %d\r\n\r\n%s"
#define BODY_JSON_READ "{ \"entities\": [{\"type\": \"Sensor\", \"isPattern\": \"false\",\"id\": \"%s\"}]}"
#define CONECTADO "CONECTADO"
#define TEST "TEST"
#define CLON "CLON"
#define KEY_NOT_FOUND_READ "READ: No se encontró la clave %s\r\n"
#define JSON_NOT_FOUND_READ "READ: No se encontró ningún JSON.\r\n"
#define JSON_FOUND_TIME "TIME: El valor obtenido de UTC Datetime es: %s\r\n"
#define KEY_NOT_FOUND_TIME "TIME: No se encontró la clave UTC Datetime\r\n"
#define JSON_NOT_FOUND_TIME "TIME: No se encontró ningún JSON.\r\n"
#define MESSAGE_SENDED_WRITE "WRITE: Sensores actualizados\r\n"
#define POT_VALUE "El valor del potenciometro es %f\r\n"
#define LDR_VALUE "El valor del LDR es %f por ciento\r\n"
#define NTC_VALUE "El valor de la temperatura es %f grados\r\n"
#define ALARM_THROW_LDR 90.000000
#define LDR_MAX 100.000000
#define LDR_MIN 0.000000
#define ALARM_THROW_TEMP 29.000000
#define TEMP_MAX 32.000000
#define TEMP_MIN 25.000000

void Task_MODE( void *pvParameters );
void Task_TEST( void *pvParameters );
void Task_HW( void *pvParameters );
void Task_WIFI( void *pvParameters );
void Task_COMM( void *pvParameters );
void Task_TIME( void *pvParameters );
void Task_WRITE( void *pvParameters );
void Task_READ( void *pvParameters );

void ESP8266_RESET(void);
void ESP8266_Boot(void);
void ESP8266_TEST(void);

typedef struct {
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
} Pin;

typedef struct {
	char * dst;
	char * 	port;
	uint8_t *  format;
	char * response;
} HTTP_Request;

typedef uint8_t BUFF_ITEM_t ;

extern Pin pines[8];
extern uint8_t bufferBody[2048];
extern uint8_t request[2048];
extern HTTP_Request httpRequestRead;
extern HTTP_Request httpRequestWrite;
extern HTTP_Request httpRequestTime;
extern double pot;
extern double ldr;
extern double temp;
extern char* alarm;
extern int alarmCont;
extern char* actualAlarmSrc;
extern char delimeter[];

#endif
