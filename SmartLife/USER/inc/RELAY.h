#ifndef __RELAY_H
#define __RELAY_H

#include "stm32f4xx.h"

void Relay_Init(void);
void Relay_On(void);
void Relay_Off(void);
void Relay_Toggle(void);

#endif