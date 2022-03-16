/**
 * @file main.c
 * @version 0.1
 *
 */

#include <stdint.h>
#include <stm32l4xx.h>

void EXTI15_10_IRQHandler(void){

    if(EXTI->PR1 & EXTI_PR1_PIF14){
        EXTI->PR1 = EXTI_PR1_PIF14;
    }
}

int main(void) {

	// Klok voor GPIOB en SYSCFG aanzetten
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

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

