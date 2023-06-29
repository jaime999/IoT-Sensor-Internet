#include "main.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include "cJSON.h"
#include "tareas.h"

//Variables globales par las funciones auxiliares
extern ADC_HandleTypeDef hadc1;
extern ADC_ChannelConfTypeDef sConfig;
double const BETA = 3900;
double const R25 = 10000;
double const T25 = 298;
double const KELVIN = 273;
//char completeString[200];
//char* bodyJSONRead;

uint32_t getAnalogData() {
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    // Disparo la conversion
    HAL_ADC_Start(&hadc1);
    // Espero la finalización
    HAL_ADC_PollForConversion(&hadc1, 10000);
    // Leo el valor de la conversión 4095 y 0
    uint32_t valueAD=HAL_ADC_GetValue(&hadc1);
    return valueAD;
}

double getVoltage(uint32_t valueAD) {
	pot = valueAD * 3.3 / 4095.0;
}

double getPercentage(uint32_t valueAD) {
	ldr = valueAD * 100 / 4095.0;
}

double getTemperature(uint32_t valueAD) {
	double vDivisor = ((double)valueAD/4095) * 3.3;
	double rNTC = (3.3 * (10000/(3.3 - vDivisor))) - 10000;
	double ec2 = BETA/(log(rNTC/R25) + (BETA/T25));
	temp = ec2 - KELVIN;
}

double getAnalogDataPot() {
	// configuración del canal de entrada A/D
	      sConfig.Channel = ADC_CHANNEL_4;
	      uint32_t valueAD = getAnalogData();
	      return getVoltage(valueAD);
}

double getAnalogDataLdr() {
	// configuración del canal de entrada A/D
	      sConfig.Channel = ADC_CHANNEL_0;
	      uint32_t valueAD = getAnalogData();
	      return getPercentage(valueAD);
}

double getAnalogDataNtc() {
	// configuración del canal de entrada A/D
	      sConfig.Channel = ADC_CHANNEL_1;
	      uint32_t valueAD = getAnalogData();
	      return getTemperature(valueAD);
}

void parseMessage(char* json_start, char* mode) {
    if (json_start != NULL) {
    	cJSON* json = cJSON_Parse(json_start);
    	cJSON *contextResponsesArray = cJSON_GetObjectItem(json, "contextResponses");
    	cJSON *firstContextResponse = cJSON_GetArrayItem(contextResponsesArray, 0);
    	cJSON *contextElement = cJSON_GetObjectItem(firstContextResponse, "contextElement");
    	cJSON* id = cJSON_GetObjectItem(contextElement, "id");
    	if (id != NULL && cJSON_IsString(id)) {
    	    //char* idStr = id->valuestring;
    	    printf("Sensor: %s\r\n", id->valuestring);
    	} else {
    	    printf(KEY_NOT_FOUND_READ, "id");
    	}

    	cJSON *attributesArray = cJSON_GetObjectItem(contextElement, "attributes");
    	//int attributesCount = cJSON_GetArraySize(attributesArray);

    	for (int i = 0; i < cJSON_GetArraySize(attributesArray); i++) {
    	    cJSON *attribute = cJSON_GetArrayItem(attributesArray, i);

    	    // Obtén los valores de los campos "name", "type" y "value"
    	    cJSON *name = cJSON_GetObjectItem(attribute, "name");
    	    cJSON *type = cJSON_GetObjectItem(attribute, "type");
    	    cJSON *value = cJSON_GetObjectItem(attribute, "value");

    	    // Imprime los valores de los campos
    	    printf("%s:", name->valuestring);
    	    printf("%s\r\n", value->valuestring);
    	}

    	if(strcmp(mode, CLON) == 0) {
    		getAlarm(attributesArray);
    	}

    	if(strcmp(mode, CONECTADO) == 0) {
    		getAlarmSrc(attributesArray);
    	}
    	cJSON_Delete(json);
    } else {
        printf(JSON_NOT_FOUND_READ);
    }
}

void parseTime(char* json_start) {
			    if (json_start != NULL) {
			    	cJSON* json = cJSON_Parse(json_start);
			    	cJSON* utcDatetime = cJSON_GetObjectItem(json, "utc_datetime");
			    	if (utcDatetime != NULL && cJSON_IsString(utcDatetime)) {
			    	    printf(JSON_FOUND_TIME, utcDatetime->valuestring);
			    	} else {
			    	    printf(KEY_NOT_FOUND_TIME);
			    	}
			    	cJSON_Delete(json);
			    } else {
			        printf(JSON_NOT_FOUND_TIME);
			    }
}

void getAlarm(cJSON* attributesArray) {
    cJSON* item = NULL;
    cJSON_ArrayForEach(item, attributesArray) {
      cJSON* name = cJSON_GetObjectItem(item, "name");
      if (name != NULL && cJSON_IsString(name) && strcmp(name->valuestring, "Alarma") == 0) {
        cJSON* value = cJSON_GetObjectItem(item, "value");
        if (value != NULL && cJSON_IsString(value) ) {
            char* valueAlarm = cJSON_GetStringValue(value);
        	if(strcmp(valueAlarm, "T") == 0) {
        		HAL_GPIO_WritePin(GPIOA, Buzzer_Pin, GPIO_PIN_SET);
        	}
        	else {
        		HAL_GPIO_WritePin(GPIOA, Buzzer_Pin, GPIO_PIN_RESET);
        	}

    		return 0;
        }
      }
    }
}

void getAlarmSrc(cJSON* attributesArray) {
    cJSON* item = NULL;
    cJSON_ArrayForEach(item, attributesArray) {
    cJSON* name = cJSON_GetObjectItem(item, "name");
      if (name != NULL && cJSON_IsString(name) && strcmp(name->valuestring, "Alarma_src") == 0) {
        cJSON* value = cJSON_GetObjectItem(item, "value");
        if (value != NULL && cJSON_IsString(value) ) {
            char* alarmSrc = cJSON_GetStringValue(value);
        	if(strcmp(alarmSrc, actualAlarmSrc) != 0) {
        		HAL_GPIO_WritePin(GPIOA, Buzzer_Pin, GPIO_PIN_RESET);
        		actualAlarmSrc = alarmSrc;
        	}

    		return 0;
        }
      }
    }
}

void showInfo(char* nodeId) {
	unsigned int ct;

    sprintf(bufferBody, BODY_JSON_READ, nodeId);
    int contentLength = strlen(bufferBody);

    sprintf(request, COMPLETE_STRING_READ, contentLength, bufferBody);
    memcpy(httpRequestRead.format, request, sizeof(request));
}

void cleanResponse(char * data,int maxlen)
{
	int t,h,l,o,i;
	char * res;
	int indexbk;
	char bytes[4];

	char * j,*from, *pc;

	res=data;


	 if (strlen(data)<300)
		 res=1;
	 else
		 res=0;
	 i=0;
	 while (j=strstr(data,"+IPD")){

		 from=strstr(j,":");
		 from++;

		 	 for (pc=from;pc<(data+2048);pc++)
			 	*(j++)=*(from++);
	 };
	 	 do
			                  {
			                  }while(data[i++]!='{');
			                  i--;
	for (t=0;t<(2048-i);t++)
		data[t]=data[t+i];

	i=0;

    for (t=0;t<2048;t++)
    	if (data[t]=='}')
    		i=t;
    data[i+1]=0;




}

void resetBuzzer() {
	//printf("Reiniciando dispositivo...\r\n");
	HAL_GPIO_WritePin(GPIOA, Buzzer_Pin, GPIO_PIN_RESET);
	for(int i = 0; i < 50000000; i++) {}
}

void checkIfBuzzerSet(double LEDSensor, int LEDBuzzer) {
	if(ldr > 70 || temp > 30)
		  HAL_GPIO_WritePin(GPIOA, Buzzer_Pin, GPIO_PIN_SET);
}

