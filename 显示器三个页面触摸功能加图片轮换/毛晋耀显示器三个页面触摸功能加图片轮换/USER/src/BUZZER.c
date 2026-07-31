#include "buzzer.h"

void Buzzer_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure; 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;      
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;    
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    
    Buzzer_Off();
}

void Buzzer_On(void) {
    GPIO_SetBits(GPIOD, GPIO_Pin_15);   
}

void Buzzer_Off(void) {
    GPIO_ResetBits(GPIOD, GPIO_Pin_15); 
}

void Buzzer_Toggle(void) {
    GPIO_ToggleBits(GPIOD, GPIO_Pin_15); 
}
