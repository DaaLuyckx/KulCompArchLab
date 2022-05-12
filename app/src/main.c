/**
 * @file main.c
 * @version 0.1
 */

#include <stdint.h>
#include <stm32l4xx.h>
#include <math.h>
#include <stdio.h>

int tick = 0;
int splitted_number[4] = {0};
int toggle =0;
float number;

/*void delay(unsigned int n){
	volatile unsigned int delay = n;
	while (delay !=0){
		if(tick){
			delay--;
			tick = 0;
		}
	}
}*/

void delay(unsigned int n){
	volatile unsigned int delay = n;
	while (delay--);
}

void SysTick_Handler(void){tick++;}

int readNTC(){
	ADC1->SQR1 |= (ADC_SQR1_SQ1_0 | ADC_SQR1_SQ1_2); //00101 (5 binair)

	// Start de ADC en wacht tot de sequentie klaar is
	ADC1->CR |= ADC_CR_ADSTART;
	while(!(ADC1->ISR & ADC_ISR_EOS));

	return ADC1->DR;
}

int __io_putchar(int ch){
    while(!(USART1->ISR & USART_ISR_TXE));
    USART1->TDR = ch;
}

int main(void){

	//klok
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	//Configuratie ADC
	// Klok selecteren, hier gebruiken we sysclk
	RCC->CCIPR &= ~RCC_CCIPR_ADCSEL_Msk;
	RCC->CCIPR |= RCC_CCIPR_ADCSEL_0 | RCC_CCIPR_ADCSEL_1;
	//NTC
	GPIOA->MODER &= ~GPIO_MODER_MODE0_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE0_1;
	// Deep powerdown modus uitzetten
	ADC1->CR &= ~ADC_CR_DEEPPWD;
	// ADC voltage regulator aanzetten
	ADC1->CR |= ADC_CR_ADVREGEN;
	//Wachten tot de regulator is gestabiliseerd (max. 20 us)
	delay(2000);
	// Kalibratie starten
	ADC1->CR |= ADC_CR_ADCAL;
	while(ADC1->CR & ADC_CR_ADCAL);
	// ADC aanzetten
	ADC1->CR |= ADC_CR_ADEN;
	// Kanaal (6) instellen
	ADC1->SMPR1 |= ADC_SMPR1_SMP5_0 | ADC_SMPR1_SMP5_1 | ADC_SMPR1_SMP5_2; //(111, traagste samplefrequentie.. beste)
	ADC1->SQR1 &= ~(ADC_SQR1_L_0 | ADC_SQR1_L_1 | ADC_SQR1_L_2 | ADC_SQR1_L_3); //(lengte' aantal in te lezen kanalen (1= 000)

	//Configuratie UART
	//GPIO configureren en alternative mode aanzetten
	GPIOA->MODER &= ~GPIO_MODER_MODE9_Msk;
	GPIOA->MODER |=  GPIO_MODER_MODE9_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT9;
	GPIOA->AFR[1] = (GPIOA->AFR[1] & (~GPIO_AFRH_AFSEL9_Msk)) | (0x7 << GPIO_AFRH_AFSEL9_Pos);
	GPIOA->MODER &= ~GPIO_MODER_MODE10_Msk;
	GPIOA->AFR[1] = (GPIOA->AFR[1] & (~GPIO_AFRH_AFSEL10_Msk)) | (0x7 << GPIO_AFRH_AFSEL10_Pos);
	//UART configureren
	USART1->CR1 = 0;
	USART1->CR2 = 0;
	USART1->CR3 = 0;
	USART1->BRR = 417;
	USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

    //Systick configureren + interupt aanzetten + prioriteit geven
    SysTick_Config(48000);
    NVIC_SetPriority(SysTick_IRQn, 128);
    NVIC_EnableIRQ(SysTick_IRQn);

    while(1){
    	// Lees de waarde in en maak conversie.
    	float Raw = readNTC();
    	float V = (Raw*3.0f)/4096.0f;
    	float R = (10000.0f*V)/(3.0f-V);
    	number = (1.0f/((logf(R/10000.0f)/3936.0f)+(1.0f/298.15f)))-273.15f;

    	printf("T=%.1fºC\n",number); // @suppress("Float formatting support")

    	delay(10000000); //1 meting per seconde (ongeveer)
    }

}

