// button_interrupt.c
// Josh Brake
// jbrake@hmc.edu
// 10/31/22

#include "main.h"
// Necessary includes for printf to work
#include <stdio.h>
#include "stm32l432xx.h"

// Function used by printf to send characters to the laptop
int _write(int file, char *ptr, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    ITM_SendChar((*ptr++));
  }
  return len;
}

int led_state;
float counter;

int encoder_A_status;
int encoder_B_status; 

int main(void) {
    led_state = 0;

    // Enable LED as output
    gpioEnable(GPIO_PORT_B);
    pinMode(LED_PIN, GPIO_OUTPUT);

    // Enable button as input
    gpioEnable(GPIO_PORT_A);
    pinMode(ENCODERPINA, GPIO_INPUT);
    pinMode(ENCODERPINB, GPIO_INPUT);

    GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD2, 0b01); // Set PA2 as pull-up
    GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD1, 0b01); // Set PA1 as pull-up

    // Initialize timer
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    initTIM(DELAY_TIM);

    // TODO
    // 1. Enable SYSCFG clock domain in RCC
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    // 2. Configure EXTICR for the input button interrupt
    SYSCFG->EXTICR[1] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI2, 0b000); // Select PA2
    SYSCFG->EXTICR[1] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI1, 0b000); // Select PA1

    // Enable interrupts globally
    __enable_irq();

    // TODO: Configure interrupt for falling edge of GPIO pin for button
    // 1. Configure mask bit (PA2)
    EXTI->IMR1 |= (1 << gpioPinOffset(ENCODERPINA)); // Configure the mask bit
    // 2. Disable rising edge trigger
    EXTI->RTSR1 |= (1 << gpioPinOffset(ENCODERPINA));// Enable rising edge trigger
    // 3. Enable falling edge trigger
    EXTI->FTSR1 |= (1 << gpioPinOffset(ENCODERPINA));// Enable falling edge trigger
    // 4. Turn on EXTI interrupt in NVIC_ISER
    NVIC->ISER[0] |= (1 << EXTI2_IRQn);
    

    // 1. Configure mask bit (PA1)
    EXTI->IMR1 |= (1 << gpioPinOffset(ENCODERPINB)); // Configure the mask bit
    // 2. Disable rising edge trigger
    EXTI->RTSR1 |= (1 << gpioPinOffset(ENCODERPINB));// Enable rising edge trigger
    // 3. Enable falling edge trigger
    EXTI->FTSR1 |= (1 << gpioPinOffset(ENCODERPINB));// Enable falling edge trigger
    // 4. Turn on EXTI interrupt in NVIC_ISER
    NVIC->ISER[0] |= (1 << EXTI1_IRQn);
    
    encoder_A_status = digitalRead(ENCODERPINA);
    encoder_B_status = digitalRead(ENCODERPINB); 

    int led;
    led = 0;

    float rps; 
  
    while(1){   
        delay_millis(TIM2, 100);
        rps = (((counter)/4/408) *10);
        printf("RPS %f\tEncoderA %ds\tEncoderB %d\n", rps, encoder_A_status, encoder_B_status);
        counter = 0; 

        //led ^= 1U;
        //digitalWrite(LED_PIN, led);

    }

}

// TODO: What is the right name for the IRQHandler?
void EXTI2_IRQHandler(void){
    // Check that the button was what triggered our interrupt
    if (EXTI->PR1 & (1 << 2)){
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 << 2);
        
        //encoder_A_status ^= 1U;
        encoder_A_status = digitalRead(ENCODERPINA);
        encoder_B_status = digitalRead(ENCODERPINB); 


        if ((encoder_A_status ^ encoder_B_status) == 1){
            counter += 1; 
        }  else {
            counter -= 1; 
        }

    }
}


void EXTI1_IRQHandler(void){
    // Check that the button was what triggered our interrupt
    if (EXTI->PR1 & (1 << 1)){
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 << 1);

        //encoder_B_status ^= 1U;
        encoder_A_status = digitalRead(ENCODERPINA);
        encoder_B_status = digitalRead(ENCODERPINB); 

        if ((encoder_A_status ^ encoder_B_status) == 0){
            counter += 1; 
        }  else {
            counter -= 1; 
        }

    }
}