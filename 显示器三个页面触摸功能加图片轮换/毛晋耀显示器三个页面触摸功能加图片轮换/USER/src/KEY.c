#include "KEY.h"
#include "Delay.h" 
#include "TIMER.h"
void KEY_Init(void)
{
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_Key = {0};
    GPIO_Key.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_Key.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Key.GPIO_PuPd = GPIO_PuPd_DOWN; 
    GPIO_Key.GPIO_Pin  = KEY1_PIN;
    GPIO_Init(KEY1_PORT, &GPIO_Key);
    GPIO_Key.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Key.GPIO_Pin  = KEY2_PIN | KEY3_PIN;
    GPIO_Init(KEY2_PORT, &GPIO_Key);
}

uint8_t KEY_Scan(void)
{
    static uint8_t key_up = 1; 
    uint8_t k1 = (GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN) == 1);
    uint8_t k2 = (GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN) == 0);
    uint8_t k3 = (GPIO_ReadInputDataBit(KEY3_PORT, KEY3_PIN) == 0);

    
    if (key_up && (k1 || k2 || k3))
    {
        delay_ms(20); 
        k1 = (GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN) == 1);
        k2 = (GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN) == 0);
        k3 = (GPIO_ReadInputDataBit(KEY3_PORT, KEY3_PIN) == 0);

        if (k1 || k2 || k3)
        {
            key_up = 0; 

            if (k1) return 1;
            if (k2) return 2;
            if (k3) return 3;
        }
    }
    
    else if (!k1 && !k2 && !k3)
    {
        key_up = 1;
    }

    return 0; 
}
static uint8_t count1 = 0;
static uint8_t count2 = 0;
static uint8_t count3 = 0;
uint8_t KEY1_count(void)
{
    uint8_t key = KEY_Scan();

    if (key == 1)
    {
        count1++;
        if (count1 > 4)
            count1 = 1;
    }
    return count1;
}

uint8_t KEY2_count(void)
{
    uint8_t key = KEY_Scan();
    if (key == 2)
    {
        count2++;
        if (count2 > 3)
            count2 = 1;
    }
    return count2;
}

uint8_t KEY3_count(void)
{
    uint8_t key = KEY_Scan();

    if (key == 3)
    {
        count3++;
        if (count3 > 3)
            count3 = 1;
    }
    return count3;
}
