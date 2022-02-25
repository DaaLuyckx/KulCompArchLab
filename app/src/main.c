/**
 * @file main.c
 * @version 0.1
 *
 */

#include <stdint.h>
#include <stm32l4xx.h>

void delay(unsigned int n) {
	volatile unsigned int delay = n;
	while (delay--)
		;
}

int main(void) {


	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;


	GPIOB->MODER &= ~GPIO_MODER_MODE9_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE9_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT9;

	while (1) {

	  GPIOB->BSRR = GPIO_BSRR_BS9;
	  delay(250);
	  GPIOB->BSRR = GPIO_BSRR_BR9;
	  delay(250);

  }
}
