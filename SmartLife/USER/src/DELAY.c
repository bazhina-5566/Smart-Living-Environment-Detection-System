#include "Delay.h"
void delay_us(uint32_t us)
{
    uint32_t i;
    while(us--)
    {
        
        i = 165; 
        while(i--)
        {
            __NOP(); 
        }
    }
}


void delay_ms(uint32_t ms)
{
    while(ms--)
    {
        delay_us(1000); 
    }
}
