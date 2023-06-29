#include "cJSON.h"
#include "main.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include "tareas.h"

//Parámetros globales del modo CONECTADO
char pinValues[9];
char percentageIntensidad[20];
char temperatura[20];
char alarma[1];


void readLEDS(Pin pines[]) {
	for(int i=0; i<8;i++) {
		if (HAL_GPIO_ReadPin(pines[i].GPIOx, pines[i].GPIO_Pin) == GPIO_PIN_SET) {
			pinValues[i] = '1';  // Si el pin está en SET, agrega '1' a la cadena
		} else {
			pinValues[i] = '0';  // Si el pin no está en SET, agrega '0' a la cadena
		}
	}
	pinValues[8] = '\0';
}

void readIntensidadLuz() {
	double percentage = getAnalogDataLdr();
	//snprintf(percentageIntensidad, sizeof(percentageIntensidad), "%.2f", percentage);
	sprintf(percentageIntensidad, "%f", percentageIntensidad);
}

void readAlarma() {
	if(HAL_GPIO_ReadPin(GPIOA, Buzzer_Pin) == GPIO_PIN_SET) {
		sprintf(alarma, "%s", "T");
	}

	else {
		sprintf(alarma, "%s", "T");
	}
}

void readTemperatura() {
	double temperatureNumber = getAnalogDataNtc();
	sprintf(temperatura, "%f", temperatureNumber);
}

char addAttributes(char name, char type, char value) {
	char attribute[100];
	sprintf(attribute, "{\"name\": \"%s\", \"type\": \"%s\", \"value\": \"%s\"}", name, type, value);
	return attribute;
}

void publishMessage(Pin pines[]) {
	printf("%s\r\n", "Publicando Mensaje...");
	readLEDS(pines);
	readIntensidadLuz();
	readAlarma();
	readTemperatura();

	char attributes[500];
	attributes[0] = '\0';
	strcat(attributes, "{\"name\": \"LEDS\", \"type\": \"binary\", \"value\": \"");
	strcat(attributes, pinValues);
	strcat(attributes, "\"}");

	strcat(attributes, ",{\"name\": \"IntensidadLuz\", \"type\": \"floatArray\", \"value\": \"");
	strcat(attributes, percentageIntensidad);
	strcat(attributes, "\"}");

	strcat(attributes, ",{\"name\": \"Alarma\", \"type\": \"boolean\", \"value\": \"");
	strcat(attributes, alarma);
	strcat(attributes, "\"}");

	strcat(attributes, ",{\"name\": \"Temperatura\", \"type\": \"floatArray\", \"value\": \"");
	strcat(attributes, temperatura);
	strcat(attributes, "\"}");

	cJSON *jsonObject = cJSON_CreateObject();
	char bodyJSON[500];
	sprintf(bodyJSON, "{ \"contextElements\": [{\"type\": \"Sensor\", \"isPattern\": \"false\",\"id\": \"SensorSEU_PPB94\", \"attributes\": [%s]}],\"updateAction\": \"APPEND\"}", attributes);
	int contentLength = strlen(bodyJSON);

	char completeString[1000];
	sprintf(completeString, "POST /v1/updateContext HTTP/1.1\r\nContent-Type: application/json\r\nAccept: application/json\r\nContent-Length: %d\r\n\r\n%s", contentLength, bodyJSON);

    // Agrega pares clave-valor al objeto JSON
    cJSON_AddStringToObject(jsonObject, "dst", "pperez-seu-or.disca.upv.es");
    cJSON_AddStringToObject(jsonObject, "port", "1026");
    cJSON_AddStringToObject(jsonObject, "format", completeString);
	char* response = sendMessage(jsonObject);

	printf("%s\r\n", "Sensores actualizados...");
}

char generateWriteJSON() {
	printf("%s\r\n", "Publicando Mensaje...");
	readLEDS(pines);
	readIntensidadLuz();
	readAlarma();
	readTemperatura();

	char attributes[500];
	attributes[0] = '\0';
	strcat(attributes, "{\"name\": \"LEDS\", \"type\": \"binary\", \"value\": \"");
	strcat(attributes, pinValues);
	strcat(attributes, "\"}");

	strcat(attributes, ",{\"name\": \"IntensidadLuz\", \"type\": \"floatArray\", \"value\": \"");
	strcat(attributes, percentageIntensidad);
	strcat(attributes, "\"}");

	strcat(attributes, ",{\"name\": \"Alarma\", \"type\": \"boolean\", \"value\": \"");
	strcat(attributes, alarma);
	strcat(attributes, "\"}");

	strcat(attributes, ",{\"name\": \"Temperatura\", \"type\": \"floatArray\", \"value\": \"");
	strcat(attributes, temperatura);
	strcat(attributes, "\"}");

	cJSON *jsonObject = cJSON_CreateObject();
	char bodyJSON[500];
	sprintf(bodyJSON, "{ \"contextElements\": [{\"type\": \"Sensor\", \"isPattern\": \"false\",\"id\": \"SensorSEU_PPB94\", \"attributes\": [%s]}],\"updateAction\": \"APPEND\"}", attributes);
	int contentLength = strlen(bodyJSON);

	char completeString[1000];
	sprintf(completeString, "POST /v1/updateContext HTTP/1.1\r\nContent-Type: application/json\r\nAccept: application/json\r\nContent-Length: %d\r\n\r\n%s", contentLength, bodyJSON);
	return completeString;
}

void conectadoMain(Pin pines[]) {
	printf("Entrando en el modo %s\r\n", "Conectado");
	//showInfo("SEU_PPB94");

    publishMessage(pines);
}

