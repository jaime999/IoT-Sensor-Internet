#include "main.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include "tareas.h"


void waitLoop() {
	for(int i = 0; i < 1000000; i++) {}
}

void resetLeds(Pin pines[]) {
	for(int i=0; i<8;i++) {
		  HAL_GPIO_WritePin(pines[i].GPIOx, pines[i].GPIO_Pin, GPIO_PIN_RESET);
	}
}

void startLeds(Pin pines[]) {
	resetLeds(pines);
	for(int i=0; i<8;i++) {
		  HAL_GPIO_WritePin(pines[i].GPIOx, pines[i].GPIO_Pin, GPIO_PIN_SET);
		  waitLoop();
	}
}

void setBuzzer() {
	printf("Esta %s\r\n", "pitandooooo");
	HAL_GPIO_WritePin(GPIOA, Buzzer_Pin, GPIO_PIN_SET);
	waitLoop();
	//HAL_GPIO_WritePin(GPIOA, Buzzer_Pin, GPIO_PIN_RESET);
}

void getButtonState(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, char* buttonName) {
	GPIO_PinState buttonState=HAL_GPIO_ReadPin(GPIOx,GPIO_Pin);
	if (buttonState == GPIO_PIN_RESET)
	    printf("Estado %s: GPIO_PIN_RESET\r\n", buttonName);
	else
	    printf("Estado %s: GPIO_PIN_SET\r\n", buttonName);
}

void testMain(Pin pines[]) {
	printf("Entrando en el modo %s\r\n", "Test");

	startLeds(pines);
	setBuzzer();

	getAnalogDataPot();
	getAnalogDataLdr();
	getAnalogDataNtc();
	printf(POT_VALUE, pot);
	printf(LDR_VALUE, ldr);
	printf(NTC_VALUE, temp);

	getButtonState(PULSADOR1_GPIO_Port, PULSADOR1_Pin, "Boton 1");
	getButtonState(PULSADOR2_GPIO_Port, PULSADOR2_Pin, "Boton 2");

	getVersion();
}


