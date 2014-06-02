#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"

void TIM3_init(void);
void display_position(unsigned int position);
void TIM3_IRQHandler(void);
