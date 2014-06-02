#include "LCD/lcd_common.h"
#include "LCD/lcd_hd44780.h"
#include "I2C/i2c_common.h"

uint8_t received_data[2];

int main(void)
{
	static const uint8_t pgm_regs[] =
	{
			0x01,       //Sec
			0x17,		//Min
	  		0x12,		//Hour
	  		0x2,		//Day num
	  		0x13,		//day
	  		0x05,		//Month
	  		0x14,		//Year
	  		0x3f
	};

	init_I2C1();

	WriteDate(pgm_regs);

	lcd_init();
	TIM3_init();



	//	Write(0x06,0x14);
//	Write_RTC(0x00, 0x00);
	while(1)
	{
		int x=100000;
			while(x--);
		ReadRTCDate();

		date(buf);

	 // write one byte to the slave
		int y=100000;
		while(y--);
		RTC_data.temp = Read(0x00);


	}
}
