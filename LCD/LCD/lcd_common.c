#include "lcd_common.h"

unsigned int lcd_position = 0;

void TIM3_init()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 16000;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_Prescaler = 134400;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_ClearITPendingBit,(TIM3, TIM_IT_Update);

	TIM_Cmd(TIM3, ENABLE);
}

void display_position(unsigned int position)
{
	lcd_cls();
	switch (position)
	{
		case 0:
			lcd_str_center(0, "Godzina:");
			lcd_str_center(1, "12:34:23");
		break;

		case 1:
			lcd_str_center(0, "Data:");
			lcd_str_center(1, "20.05.14");
		break;

		case 2:
			lcd_str_center(0, "Temperatura:");
			lcd_str_center(1, "25.0 *C");
		break;

		case 3:
			lcd_str_center(0, "Wilgotnosc:");
			lcd_str_center(1, "89%");
		break;

		case 4:
			lcd_str_center(0, "Cisnienie:");
			lcd_str_center(1, "1012 hPa");
		break;

		case 5:
			lcd_str_center(0, "Stacja Meteo:");
			lcd_str_center(1, "0.8.0");
		break;

		default:
		break;
	}
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		display_position(lcd_position);

		(lcd_position == 5) ? (lcd_position = 0) : (lcd_position++);

		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}
