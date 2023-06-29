#include "main.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>


void checkBuzzer(char* alarm) {
	if(alarm == "T") {
		HAL_GPIO_WritePin(GPIOA, Buzzer_Pin, GPIO_PIN_SET);
	}

	else {
		HAL_GPIO_WritePin(GPIOA, Buzzer_Pin, GPIO_PIN_RESET);
	}
}

void clonMain() {
	char* response = showInfo("SensorSEU_PBB94");
	checkBuzzer(response);
}


