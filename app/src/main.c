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

    //Klok activeren
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    //Led1
    GPIOB->MODER &= ~GPIO_MODER_MODE9_Msk; //Alle bits van pin9 (blok B) op 0 zetten
    GPIOB->MODER |= GPIO_MODER_MODE9_0;    //Bit 0 van pin 9 hoog maken (=output)
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT9;     //Laag zetten voor push pull

    //Led2
    GPIOC->MODER &= ~GPIO_MODER_MODE13_Msk; //Alle bits van pin 13 (blok C) op 0 zetten
    GPIOC->MODER |= GPIO_MODER_MODE13_0;    //Bit 0 van pin 13 hoog maken (=output)
    GPIOC->OTYPER &= ~GPIO_OTYPER_OT13;     //Laag zetten voor push pull

    //KnopA
    GPIOB->MODER &= ~GPIO_MODER_MODE13_Msk; //Alle bits van pin 13 (blok B) laag zetten (=input)

    //KnopB
    GPIOB->MODER &= ~GPIO_MODER_MODE14_Msk; //Alle bits van pin 13 (blok B) laag zetten (=input)

    //pull up weerstand hoog zetten
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD13_Msk;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD13_0;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD14_Msk;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD14_0;


    while (1) {
        if (!(GPIOB->IDR  & GPIO_IDR_ID13)) {
        	GPIOB->BSRR |= GPIO_BSRR_BS15;
            GPIOC->ODR |= GPIO_ODR_OD13;
            GPIOB->ODR |= GPIO_ODR_OD9;
            delay(500000);
            GPIOC->ODR &= ~GPIO_ODR_OD13;
            GPIOB->ODR &= ~GPIO_ODR_OD9;
            delay(500000);
        }

        else if (!(GPIOB->IDR & GPIO_IDR_ID14)){
            GPIOC->ODR |= GPIO_ODR_OD13;
            GPIOB->ODR |= GPIO_ODR_OD9;
            delay(2500000);
            GPIOC->ODR &= ~GPIO_ODR_OD13;
            GPIOB->ODR &= ~GPIO_ODR_OD9;
            delay(2500000);
            }

        else {
            GPIOC->ODR &= ~GPIO_ODR_OD13;
            GPIOB->ODR &= ~GPIO_ODR_OD9;
        }
    }


}
