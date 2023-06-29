#include "main.h"
#include <stdio.h>
#include "tareas.h"

char* getBuzzer() {
	if(	HAL_GPIO_ReadPin(GPIOA, Buzzer_Pin) == GPIO_PIN_SET) {
		return "T";
	}

	return "F";
}

void getWriteInfo(char* nodeId, char* mode) {
    unsigned int ct;

    char* alarma = getBuzzer();
    getAnalogDataLdr();
    getAnalogDataNtc();

	if(strcmp(mode, CLON) == 0) {
	    sprintf(bufferBody, BODY_JSON_ALARM, DEVICE_TO_CLONE, MY_DEVICE, alarmCont);
	    alarmCont++;
	}

	else {
		sprintf(bufferBody, BODY_JSON_WRITE, nodeId, alarma, ldr, LDR_MAX, LDR_MIN, ALARM_THROW_LDR, temp, TEMP_MAX, TEMP_MIN, ALARM_THROW_TEMP);
	}
    int contentLength = strlen(bufferBody);

    sprintf(request, COMPLETE_STRING_WRITE, contentLength, bufferBody);
    memcpy(httpRequestWrite.format, request, sizeof(request));
}


