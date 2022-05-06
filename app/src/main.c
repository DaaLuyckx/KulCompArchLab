/**
 * @file main.c
 * @version 0.1
 */

#include <stdint.h>
#include <stm32l4xx.h>
#include <math.h>

#include "../lib/stm32l452xx.h"
int tick = 0;
int splitted_number[4] = {0};
int toggle =0;
float number;
float Raw_POT = 0;
float Raw_NTC = 0;

//Hier maak ik mijn noten aan om later een nummer te schrijven: Vb: A= (48*10^6)/880(frequentie la) = 54545
int A = 54545;
int B = 48583;
int C = 45889;
int D = 40886;
int E = 36419;
int F = 34384;
int G = 30612;
int A2 = 27273;

void delay(unsigned int n){
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
    split_number(number);
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
	number = (splitted_number[0]*1000+splitted_number[1]*100+splitted_number[2]*10+splitted_number[3]);
}

int main(void){
	//klok
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;

	// Klok selecteren, hier gebruiken we sysclk
	RCC->CCIPR &= ~RCC_CCIPR_ADCSEL_Msk;
	RCC->CCIPR |= RCC_CCIPR_ADCSEL_0 | RCC_CCIPR_ADCSEL_1;

	//NTC
	GPIOA->MODER &= ~GPIO_MODER_MODE0_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE0_1;

	//BUZZER
	GPIOA->MODER &= ~GPIO_MODER_MODE1_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE1_0 | GPIO_MODER_MODE1_1;

	// Deep powerdown modus uitzetten
	ADC1->CR &= ~ADC_CR_DEEPPWD;

	// ADC voltage regulator aanzetten
	ADC1->CR |= ADC_CR_ADVREGEN;

	//Wachten tot de regulator is gestabiliseerd (max. 20 us)
	delay(20000);

	// Kalibratie starten
	ADC1->CR |= ADC_CR_ADCAL;
	while(ADC1->CR & ADC_CR_ADCAL);

	// ADC aanzetten
	ADC1->CR |= ADC_CR_ADEN;

	// Kanaal (5,6) instellen
	ADC1->SMPR1 |= ADC_SMPR1_SMP5_0 | ADC_SMPR1_SMP5_1 | ADC_SMPR1_SMP5_2; //(111, traagste samplefrequentie.. beste)
	ADC1->SMPR1 |= ADC_SMPR1_SMP6_0 | ADC_SMPR1_SMP6_1 | ADC_SMPR1_SMP6_2; //(111, traagste samplefrequentie.. beste)
	ADC1->SQR1 &= ~(ADC_SQR1_L_0 | ADC_SQR1_L_1 | ADC_SQR1_L_2 | ADC_SQR1_L_3); //(lengte' aantal in te lezen kanalen (1= 0000)

	// Configureren GPIO
	GPIOB->MODER &= ~GPIO_MODER_MODE8_Msk; //Alle bits van pin8 (blok B) laag maken.
	GPIOB->MODER |=  GPIO_MODER_MODE8_1; //Bit 1 van pin 8 hoog maken (alternate function modus)
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT8;	//Laag zetten voor push-pull
	GPIOB->AFR[1] = (GPIOB->AFR[1] & (~GPIO_AFRH_AFSEL8_Msk)) | (0xE << GPIO_AFRH_AFSEL8_Pos); //Alternate function register gebruiken we om de juiste functie te selecteren

	//Instellen timer hoofdteller
	TIM16->PSC = 0; //Prescaler contains the value to be loaded in the active prescaler register at each update event

	//Instellen timer, capture & compare
	TIM16->CCMR1 &= ~TIM_CCMR1_CC1S; //CC1S: caputure/compare 1 selectie, hier 00 wilt zeggen -> channel configured as output.
	TIM16->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1FE;
	//OC1M: deze bits definieren het gedrag van het output ref. signal. Bit 1 en 2 zijn hier hoog, -> ingesteld als toggle.
	//OC1FE: hoog wilt zeggen dat output compare 1 fast enable waar is, deze bit wordt gebruikt om het effect te versnellen van een event.
	TIM16->CCER |= TIM_CCER_CC1E; //CCER: capture/compare enable register: CC1E hoog -> capture/Compare 1 output enabled.
	TIM16->CCER &= ~TIM_CCER_CC1P;//CC1P laag -> Capture/Compare 1 output polarity active high.
	TIM16->CR1 |= TIM_CR1_CEN; //CR1:control register 1: CEN hoog -> counter enabled.

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

    while(1){
    	//BUZZER
    	ADC1->SQR1 &= ~ADC_SQR1_SQ1_0; //bit 0 op 0 zetten (zie verschil NTC)
    	ADC1->SQR1 |= (ADC_SQR1_SQ1_1 | ADC_SQR1_SQ1_2); //00110 (6 binair)
    	// Start de ADC en wacht tot de sequentie klaar is
    	ADC1->CR |= ADC_CR_ADSTART;
    	while(!(ADC1->ISR & ADC_ISR_EOC));  //EOC gebruiken! da zorgt ervoor da alles is afgrond, EOS zorgt er alleen voor dat de sampling is afgrond, dus kunnen ze in elkaar zitten.
    	Raw_POT = ADC1->DR;

    	//NTC
    	ADC1->SQR1 &= ~ADC_SQR1_SQ1_1; //bit 1 op 0 zetten (zie verschil buzzer)
    	ADC1->SQR1 |= (ADC_SQR1_SQ1_0 | ADC_SQR1_SQ1_2); //00101 (5 binair)
    	// Start de ADC en wacht tot de sequentie klaar is
    	ADC1->CR |= ADC_CR_ADSTART;
    	while(!(ADC1->ISR & ADC_ISR_EOC));
    	// Lees de waardes in
    	Raw_NTC = ADC1->DR;
    	// Zet ruwe weerstandswaarde om naar temperatuur
    	float V = (Raw_NTC*3.0f)/4096.0f;
    	float R = (10000.0f*V)/(3.0f-V);
    	number = (1.0f/((logf(R/10000.0f)/3936.0f)+(1.0f/298.15f)))-273.15f;
    	number *= 10; //zorgt voor de gevraagde afronding van 0.1°C

    	if( Raw_POT > Raw_NTC){ //Er zit een inversie in omdat de spanning naar beneden gaat bij een vergroting.
    		//Logica om liedje af te spelen, hiervoor zou ook een lus kunnen worden gebruikt met de noten en vertragingen in een lijst, maar dit vereiste onnodig extra werk voor een kort stukje.
    		TIM16->BDTR |= TIM_BDTR_MOE;

    		TIM16->ARR = C;
    		TIM16->CCR1 = C/2;
    		delay(5000000);

    		TIM16->BDTR &= ~TIM_BDTR_MOE;
    		delay(1000000);
    		TIM16->BDTR |= TIM_BDTR_MOE;

    		TIM16->ARR = C;
    		TIM16->CCR1 = C/2;
    		delay(5000000);

    		TIM16->BDTR &= ~TIM_BDTR_MOE;
    		delay(1000000);
    		TIM16->BDTR |= TIM_BDTR_MOE;

    		TIM16->ARR = G;
    		TIM16->CCR1 = G/2;
    		delay(5000000);

    		TIM16->BDTR &= ~TIM_BDTR_MOE;
    		delay(1000000);
    		TIM16->BDTR |= TIM_BDTR_MOE;

    		TIM16->ARR = G;
    		TIM16->CCR1 = G/2;
    		delay(5000000);

    		TIM16->BDTR &= ~TIM_BDTR_MOE;
    		delay(1000000);
    		TIM16->BDTR |= TIM_BDTR_MOE;

    		TIM16->ARR = A2;
    		TIM16->CCR1 = A2/2;
    		delay(5000000);

    		TIM16->BDTR &= ~TIM_BDTR_MOE;
    		delay(1000000);
    		TIM16->BDTR |= TIM_BDTR_MOE;

    		TIM16->ARR = A2;
    		TIM16->CCR1 = A2/2;
    		delay(5000000);

    		TIM16->BDTR &= ~TIM_BDTR_MOE;
    		delay(1000000);
    		TIM16->BDTR |= TIM_BDTR_MOE;

    		TIM16->ARR =G;
    		TIM16->CCR1 = G/2;
    		delay(10000000);

    		TIM16->ARR = F;
    		TIM16->CCR1 = F/2;
    		delay(5000000);

    		TIM16->BDTR &= ~TIM_BDTR_MOE;
    		delay(1000000);
    		TIM16->BDTR |= TIM_BDTR_MOE;

    		TIM16->ARR = F;
    		TIM16->CCR1 = F/2;
    		delay(5000000);

    		TIM16->BDTR &= ~TIM_BDTR_MOE;
    		delay(1000000);
    		TIM16->BDTR |= TIM_BDTR_MOE;

    		TIM16->ARR = E;
    		TIM16->CCR1 = E/2;
    		delay(5000000);

    		TIM16->BDTR &= ~TIM_BDTR_MOE;
    		delay(1000000);
    		TIM16->BDTR |= TIM_BDTR_MOE;

    		TIM16->ARR = E;
    		TIM16->CCR1 = E/2;
    		delay(5000000);

    		TIM16->BDTR &= ~TIM_BDTR_MOE;
    		delay(1000000);
    		TIM16->BDTR |= TIM_BDTR_MOE;

    		TIM16->ARR = D;
    		TIM16->CCR1 = D/2;
    		delay(5000000);

    		TIM16->BDTR &= ~TIM_BDTR_MOE;
    		delay(1000000);
    		TIM16->BDTR |= TIM_BDTR_MOE;

    		TIM16->ARR = D;
    		TIM16->CCR1 = D/2;
    		delay(5000000);

    		TIM16->BDTR &= ~TIM_BDTR_MOE;
    		delay(1000000);
    		TIM16->BDTR |= TIM_BDTR_MOE;

    		TIM16->ARR = C;
    		TIM16->CCR1 = C/2;
    		delay(1000000);

    		TIM16->BDTR &= ~TIM_BDTR_MOE;
    		delay(1000000);

    	}
    	else{
    		TIM16->BDTR &= ~TIM_BDTR_MOE;


    	}
    }

}

