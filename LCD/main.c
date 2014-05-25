#include "LCD/lcd_hd44780.h"

int main(void)
{
	lcd_init();

	        while (1)
	          {

	                  lcd_locate(1,0);
	                  lcd_str("Hello World!");

	          }
}
