/**
 * @file main.c
 * @version 0.1
 */

#include <stdint.h>
#include <stm32l4xx.h>
#include <math.h>

int tick = 0;
int splitted_number[4] = {0};
int toggle =0;
double hoek = 0;

void delay(unsigned int n){
	volatile unsigned int delay = n;
	while (delay != 0){
		if (tick){
			delay--;
			tick = 0;
		}
	}
}

int __io_putchar(int ch){
    while(!(USART1->ISR & USART_ISR_TXE));
    USART1->TDR = ch;
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

void split_number(int n){
	// Zet het getal om in de segmenten per module
	int thousand = (n/1000)%10;
	int hundred = (n/100)%10;
	int ten = (n/10)%10;
	int unit = n%10;
	splitted_number[0] = thousand;
	splitted_number[1] = hundred;
	splitted_number[2] = ten;
	splitted_number[3] = unit;
}

void SysTick_Handler(void){
	//Stuurt display aan.
    tick++;
    split_number(hoek);
    switch(toggle){
    case 0:
    	//00
    	clear_segments();
    	GPIOA-> ODR &= ~GPIO_ODR_OD8;
    	GPIOA-> ODR &= ~GPIO_ODR_OD15;
    	GPIOA-> ODR &= ~GPIO_ODR_OD6;
    	segments(splitted_number[0]);
    	toggle++;
    	break;
    case 1:
    	//01
    	clear_segments();
    	GPIOA-> ODR |= GPIO_ODR_OD8;
    	GPIOA-> ODR &= ~GPIO_ODR_OD15;
    	GPIOA-> ODR &= ~GPIO_ODR_OD6;
    	segments(splitted_number[1]);
    	toggle++;
    	break;
    case 2:
    	//10
    	clear_segments();
    	GPIOA-> ODR &= ~GPIO_ODR_OD8;
    	GPIOA-> ODR |= GPIO_ODR_OD15;
    	GPIOA-> ODR |= GPIO_ODR_OD6; //puntje
    	segments(splitted_number[2]);
    	toggle++;
    	break;
    case 3:
    	//11
    	clear_segments();
    	GPIOA-> ODR |= GPIO_ODR_OD8;
    	GPIOA-> ODR |= GPIO_ODR_OD15;
    	GPIOA-> ODR &= ~GPIO_ODR_OD6;
    	segments(splitted_number[3]);
    	toggle = 0;
    }
	hoek = (splitted_number[0]*1000+splitted_number[1]*100+splitted_number[2]*10+splitted_number[3]);
}

void Write_I2S_Accel(int data, int reg){
	I2C1->CR2 &= ~(1<<10); //Enable write mode
	I2C1->CR2 |= I2C_CR2_NACK_Msk;
    I2C1->CR2 |=  (1 << 13)|(2 << 16)|(0x53 << 1); //Grote pakket, connected device,
    while((I2C1->ISR & (1<<4)) == 0 && (I2C1->ISR & (1<<1)) == 0);
    //NACKF = 0, TXIS = 0
    if((I2C1->ISR & (1<<4)) != 0){ //NACKF = 1
        return;
    }

    I2C1->TXDR = reg; //Register doorsturen

    while(I2C1->ISR & (1<<4) == 0 && I2C1->ISR & (1<<1) == 0){};
    //NACKF = 0, TXIS = 0
    if((I2C1->ISR & (1<<4)) != 0){ //NACKF = 1
        return;
    }

    I2C1->TXDR = data;
	while((I2C1->ISR & I2C_ISR_STOPF) == 0);
}

int Read_I2S_Accel(int reg){
	while((I2C1->ISR & I2C_ISR_BUSY));
	I2C1->CR2 &= ~(1<<10); //Enable write mode
	I2C1->CR2 &= ~I2C_CR2_AUTOEND_Msk;
	I2C1->CR2 &= ~I2C_CR2_NBYTES_Msk;
	I2C1->CR2 |= I2C_CR2_NACK_Msk;
	I2C1->CR2 |=  (1 << 13)|(1 << 16)|(0x53 << 1); //Grote pakket, connected device,
	while(((I2C1->ISR & (1<<4)) == 0) && ((I2C1->ISR & (1<<1)) == 0)){};
	//NACKF = 0, TXIS = 0
	if((I2C1->ISR & (1<<4)) != 0){ //NACKF = 1
		return;
	}

	I2C1->TXDR = reg; //Register doorsturen
	while((I2C1->ISR & (1<<6)) == 0);


	I2C1->CR2 |= I2C_CR2_AUTOEND_Msk;
	I2C1->CR2 |= (1<<10); //Enable read mode
	//Read
	I2C1->CR2 |=  (1 << 16)|(0x53 << 1);
	I2C1->CR2 |= (1<<13);
	while(!(I2C1->ISR & I2C_ISR_RXNE));

	return I2C1->RXDR;
}

int main(void) {
	//Klok
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	//GPIO
    GPIOA->MODER &= ~GPIO_MODER_MODE9_Msk;
    GPIOA->MODER |=  GPIO_MODER_MODE9_1;
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT9;
    GPIOA->AFR[1] = (GPIOA->AFR[1] & (~GPIO_AFRH_AFSEL9_Msk)) | (0x7 << GPIO_AFRH_AFSEL9_Pos);

    GPIOA->MODER &= ~GPIO_MODER_MODE10_Msk;
    GPIOA->AFR[1] = (GPIOA->AFR[1] & (~GPIO_AFRH_AFSEL10_Msk)) | (0x7 << GPIO_AFRH_AFSEL10_Pos);

    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;
    USART1->BRR = 417;
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

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
   	//puntje
   	GPIOA->MODER &= ~GPIO_MODER_MODE6_Msk;
   	GPIOA->MODER |= GPIO_MODER_MODE6_0;
   	GPIOA->OTYPER &= ~GPIO_OTYPER_OT6;

   	//multiplexer
   	GPIOA->MODER &= ~GPIO_MODER_MODE8_Msk;
   	GPIOA->MODER |= GPIO_MODER_MODE8_0;
   	GPIOA->OTYPER &= ~GPIO_OTYPER_OT8;

   	GPIOA->MODER &= ~GPIO_MODER_MODE15_Msk;
   	GPIOA->MODER |= GPIO_MODER_MODE15_0;
   	GPIOA->OTYPER &= ~GPIO_OTYPER_OT15;

	//Systick
	SysTick_Config(48000);
	NVIC_SetPriority(SysTick_IRQn, 128);
	NVIC_EnableIRQ(SysTick_IRQn);

	//I2C
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

	GPIOB->MODER &= ~GPIO_MODER_MODE6_Msk;
	GPIOB->MODER |=  GPIO_MODER_MODE6_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT6;
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL6_Msk)) | (0x4 << GPIO_AFRL_AFSEL6_Pos);

	GPIOB->MODER &= ~GPIO_MODER_MODE7_Msk;
	GPIOB->MODER |=  GPIO_MODER_MODE7_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT7;
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL7_Msk)) | (0x4 << GPIO_AFRL_AFSEL7_Pos);

	I2C1->TIMINGR = 0x20303E5D;
	I2C1->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);
	I2C1->CR1 |= I2C_CR1_PE;

	//Array
	volatile int16_t array[3];
	Write_I2S_Accel(1<<3,0x2D);
	array[0] =  Read_I2S_Accel(0x2D);

    while (1) {
    	for (int i = 0; i<3; i++){
			array[i] = Read_I2S_Accel(0x32+i*2)<<(8+Read_I2S_Accel(0x32+i*2+1));
		}

		hoek = (acos(array[2]/(sqrt(array[0]*array[0]+array[1]*array[1]+array[2]*array[2]))))*(180/3.14);
		hoek *= 10; //Displayt de hoek als xxx.x graden. 1 cijfer na de komma dus

    	delay(300); //Refresht elke 300 miliseconden
    }
}
