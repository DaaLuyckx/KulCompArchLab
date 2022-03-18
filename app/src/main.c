/**
 * @file main.c
 * @version 0.1
 */

//Dit programma was eigenlijk niet de bedoeling, de display moest eigenlijk in de systick en de buttons moesten niet met interrupts.
//In het ander programma doe ik dit.

#include <stdint.h>
#include <stm32l4xx.h>

int number = 0;
int tick = 0;
int toggle_A = 0;
int toggle_B = 0;

void delay(unsigned int n){  //deze sebiet is testen.
	volatile unsigned int delay = n;
	while (delay--);
}

void segments(int n){
//Hier zeg ik welke segmenten moeten aangaan bij welk cijfer.
	switch (n){
		case 0:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD15 | GPIO_ODR_OD1);
			break;

		case 1:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			break;

		case 2:
			GPIOA->ODR |= (GPIO_ODR_OD7);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD15 | GPIO_ODR_OD2);
			break;

		case 3:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD2);
			break;

		case 4:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD1 | GPIO_ODR_OD2);
			break;

		case 5:
			GPIOA->ODR |= (GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD1 | GPIO_ODR_OD2);
			break;

		case 6:
			GPIOA->ODR |= (GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD15 | GPIO_ODR_OD1 | GPIO_ODR_OD2);
			break;

		case 7:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0);
			break;

		case 8:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD15 | GPIO_ODR_OD1 | GPIO_ODR_OD2);
			break;

		case 9:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD1 | GPIO_ODR_OD2);
			break;
	}
}

void clear_segments(){
//Alle segmenten worden uitgezet.
	GPIOA->ODR &= ~(GPIO_ODR_OD7 | GPIO_ODR_OD5);
	GPIOB->ODR &= ~(GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD15 | GPIO_ODR_OD1 | GPIO_ODR_OD2);
}

void number_to_segments(int n){
// Zet het getal om in de segmenten per module
	int thousand = (n/1000)%10;
	int hundred = (n/100)%10;
	int ten = (n/10)%10;
	int unit = n%10;

//Dit stukje is omdat het een klok is (max waarden van de 'minuten, en 'uren' aanpassen)
	if (ten == 6 && unit == 0){
		ten = 0;
		hundred++;
		number = (thousand*1000+hundred*100+ten*10+unit);
	}
	if (thousand == 2 && hundred == 4){
		thousand = 0;
		hundred = 0;
		number = (thousand*1000+hundred*100+ten*10+unit);
}

//Stuur de waardes naar
	//00
	GPIOA-> ODR &= ~GPIO_ODR_OD8;
	GPIOA-> ODR &= ~GPIO_ODR_OD15;
	segments(thousand);
	clear_segments();

	//01
	GPIOA-> ODR |= GPIO_ODR_OD8;
	GPIOA-> ODR &= ~GPIO_ODR_OD15;
	segments(hundred);
	clear_segments();

	//10
	GPIOA-> ODR &= ~GPIO_ODR_OD8;
	GPIOA-> ODR |= GPIO_ODR_OD15;
	segments(ten);
	clear_segments();

	//11
	GPIOA-> ODR |= GPIO_ODR_OD8;
	GPIOA-> ODR |= GPIO_ODR_OD15;
	segments(unit);
	clear_segments();
}

void SysTick_Handler(void){
    tick++;
}

void EXTI15_10_IRQHandler(void){
    if(EXTI->PR1 & EXTI_PR1_PIF14){
    	toggle_A++;
        EXTI->PR1 = EXTI_PR1_PIF14; //Zet de flag terug laag, anders gaat hij terug in die direct interrupt na dat die gedaan is.
    }

    if(EXTI->PR1 & EXTI_PR1_PIF13){
    	toggle_B++;
        EXTI->PR1 = EXTI_PR1_PIF13;
    }
}

int main(void){
	//klok
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	//Leds
	GPIOB->MODER &= ~GPIO_MODER_MODE9_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE9_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT9;

	GPIOC->MODER &= ~GPIO_MODER_MODE13_Msk;
	GPIOC->MODER |= GPIO_MODER_MODE13_0;
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT13;


	//segmenten
	GPIOB->MODER &= ~GPIO_MODER_MODE0_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE0_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT0;

	GPIOA->MODER &= ~GPIO_MODER_MODE7_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE7_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT7;

	GPIOA->MODER &= ~GPIO_MODER_MODE5_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE5_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT5;

	GPIOB->MODER &= ~GPIO_MODER_MODE12_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE12_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT12;

	GPIOB->MODER &= ~GPIO_MODER_MODE15_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE15_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT15;

	GPIOB->MODER &= ~GPIO_MODER_MODE1_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE1_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT1;

	GPIOB->MODER &= ~GPIO_MODER_MODE2_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE2_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT2;

	//multiplexer
	GPIOA->MODER &= ~GPIO_MODER_MODE8_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE8_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT8;

	GPIOA->MODER &= ~GPIO_MODER_MODE15_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE15_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT15;

	//buttons
    GPIOB->MODER &= ~GPIO_MODER_MODE13_Msk;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD13_Msk;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD13_0;

    GPIOB->MODER &= ~GPIO_MODER_MODE14_Msk;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD14_Msk;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD14_0;

    //systick configureren + interupt aanzetten + prioriteit geven
    SysTick_Config(48000);
    NVIC_SetPriority(SysTick_IRQn, 128);
    NVIC_EnableIRQ(SysTick_IRQn);

    //interrupts buttons
		//GPIOB Pin 14 configureren als input
	GPIOB->MODER &= ~GPIO_MODER_MODE14_Msk;
		//Pull up aanzetten op Pin 14 van GPIOB
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD14_Msk;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD14_0;
		//Pin 14 van GPIOB routeren naar de EXTI
	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI14_Msk;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI14_PB;
		//Falling edge interrupt aanzetten
	EXTI->FTSR1 |= EXTI_FTSR1_FT14;
	EXTI->IMR1 |= EXTI_IMR1_IM14;

		//GPIOB Pin 13 configureren als input
	GPIOB->MODER &= ~GPIO_MODER_MODE13_Msk;
		//Pull up aanzetten op Pin 13 van GPIOB
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD13_Msk;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD13_0;
		//Pin 13 van GPIOB routeren naar de EXTI
	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13_Msk;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PB;
		//Falling edge interrupt aanzetten
	EXTI->FTSR1 |= EXTI_FTSR1_FT13;
	EXTI->IMR1 |= EXTI_IMR1_IM13;
		//Interrupt aanzetten met een prioriteit van 127
	NVIC_SetPriority(EXTI15_10_IRQn, 127);
	NVIC_EnableIRQ(EXTI15_10_IRQn);

    while(1){
    	switch (toggle_A){
    		case 0:
				number_to_segments(number);
				if(tick == 1000){
					number++;
					tick = 0;
				}
				break;

    		case 1:
    			GPIOC->ODR |= GPIO_ODR_OD13;
    			number_to_segments(number);
    			if (toggle_B){
    				number += 100;
    				toggle_B = 0;
    			}
				break;

    		case 2:
    			GPIOC->ODR &= ~GPIO_ODR_OD13;
    			GPIOB->ODR |= GPIO_ODR_OD9;
    			number_to_segments(number);
    			if (toggle_B){
    				number ++;
    				toggle_B = 0;
    			}
    			break;

    		case 3:
    			GPIOB->ODR &= ~GPIO_ODR_OD9;
    			toggle_A = 0;
    			tick = 0;
    			delay(100000);
    			break;
    	}
    }
}
