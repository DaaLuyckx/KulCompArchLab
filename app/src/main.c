/**
 * @file main.c
 * @version 0.1
 *
 */

#include <stdint.h>
#include <stm32l4xx.h>

void delay(unsigned int n){
	volatile unsigned int delay = n;
	while (delay--);
}

int main(void) {

	// Klok voor GPIOB en SYSCFG aanzetten
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	//Led1
	GPIOB->MODER &= ~GPIO_MODER_MODE9_Msk; //Alle bits van pin9 (blok B) op 0 zetten
	GPIOB->MODER |= GPIO_MODER_MODE9_0;    //Bit 0 van pin 9 hoog maken (=output)
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT9;     //Laag zetten voor push pull

	//Led2
	GPIOC->MODER &= ~GPIO_MODER_MODE13_Msk; //Alle bits van pin 13 (blok C) op 0 zetten
	GPIOC->MODER |= GPIO_MODER_MODE13_0;    //Bit 0 van pin 13 hoog maken (=output)
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT13;     //Laag zetten voor push pull

	// GPIOB Pin 14 configureren als input
	GPIOB->MODER &= ~GPIO_MODER_MODE14_Msk;

	// Pull up aanzetten op Pin 14 van GPIOB
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD14_Msk;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD14_0;

	// Pin 14 van GPIOB routeren naar de EXTI
	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI14_Msk;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI14_PB;

	// Falling edge interrupt aanzetten
	EXTI->FTSR1 |= EXTI_FTSR1_FT14;
	EXTI->IMR1 |= EXTI_IMR1_IM14;

	// Interrupt aanzetten met een prioriteit van 128
	NVIC_SetPriority(EXTI15_10_IRQn, 128);
	NVIC_EnableIRQ(EXTI15_10_IRQn);

  while (1) {
  }
}

void EXTI15_10_IRQHandler(void){
	GPIOC->ODR |= GPIO_ODR_OD13;
    GPIOB->ODR |= GPIO_ODR_OD9;
    delay(2500000);
    GPIOC->ODR &= ~GPIO_ODR_OD13;
    GPIOB->ODR &= ~GPIO_ODR_OD9;
    delay(2500000);
    if(EXTI->PR1 & EXTI_PR1_PIF14){
        GPIOC->ODR |= GPIO_ODR_OD13;

        EXTI->PR1 = EXTI_PR1_PIF14;
    }
}

