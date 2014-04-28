#include "main.h"
#include "i2c.h"

/*---------------------------- Symbol Define -------------------------------*/

#define GPIO_TIMEOUT      (0x100000)  // timeout for blinking LEDs
#define I2C_EEPROM_ADDR   (0xD0)     // EEPROM I2C-Bus device address
#define I2C_EEPROM_PAGE   (32)       // EEPROM memory page size
/*---------------------------- Variable Define -----------------------------*/

/*---------------------------- Function Prototypes -------------------------*/

void store_to_eeprom(char* myString, uint16_t addr_ptr);
void read_from_eeprom(char* myString, uint16_t addr_ptr);
void LED_Init(void);






char * mystr = "String-D @ EEPROM-Addr[0x0080]";
static char myStri[I2C_EEPROM_PAGE] = {"Default String"};
/*---------------------------- Function Code -------------------------------*/




void LED_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOD Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  /* Configure PD12, PD13, PD14 and PD15 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void store_to_eeprom(char* myString, uint16_t addr_ptr)
{
  while (I2C_CheckDevice(I2C_EEPROM_ADDR) != OK);  // wait until EEPROM is ready

  I2C_WriteTransfer(I2C_EEPROM_ADDR, myString, strlen(myString)+1, addr_ptr, sizeof(addr_ptr));
  GPIO_SetBits(GPIOD,GPIO_Pin_14|GPIO_Pin_15);
}
void write_rtc(uint8_t* regs)
{
  while (I2C_CheckDevice(I2C_EEPROM_ADDR) != OK);  // wait until EEPROM is ready

  I2C_WriteTransfer(I2C_EEPROM_ADDR, regs, sizeof(regs), 0, sizeof(0x0040));
  GPIO_SetBits(GPIOD,GPIO_Pin_14|GPIO_Pin_15);
}
void read_from_eeprom(char* myString, uint16_t addr_ptr)
{
  while (I2C_CheckDevice(I2C_EEPROM_ADDR) != OK);  // wait until EEPROM is ready
  I2C_ReadTransfer(I2C_EEPROM_ADDR, myString, I2C_EEPROM_PAGE, addr_ptr, sizeof(addr_ptr));
}
void read_rtc(uint8_t* rbuf)
{
  while (I2C_CheckDevice(I2C_EEPROM_ADDR) != OK);  // wait until EEPROM is ready
  I2C_ReadTransfer(I2C_EEPROM_ADDR, rbuf, sizeof(rbuf), 0, sizeof(0x0040));
}
/*------------------------------------------------------------------------*/
#define MEASURE_DELAY_TIME 200

enum rtc_regs
{
    RTC_SEC_REG,
    RTC_MIN_REG,
    RTC_HOUR_CENTURY_REG,
    RTC_DAY_REG,
    RTC_DATE_REG,
    RTC_MONTH_REG,
    RTC_YEAR_REG,
    RTC_CONTROL_REG,
    RTC_REGS_SIZE	//Last item
};
/* ----------------------------------------------------------------------- */
#define RTC_HOUR_MASK 0x3f
#define RTC_MIN_MASK 0x7f
#define RTC_SEC_MASK 0x7F
#define RTC_DATE_MASK 0x3f
#define RTC_MONTH_MASK 0x1f

//Print rtc date
static void print_date(const uint8_t *rtc)
{
   char buf[24];
   //Display hour
   tiny_snprintf(
           buf,sizeof(buf),"%02x:%02x:%02x",
           rtc[RTC_HOUR_CENTURY_REG] & RTC_HOUR_MASK,
           rtc[RTC_MIN_REG]& RTC_MIN_MASK,
           rtc[RTC_SEC_REG] & RTC_SEC_MASK
   );
   nlcd_put_string(buf,0,1);
   //Display date
   tiny_snprintf(
	buf,sizeof(buf),"%02x-%02x-%02x",
	rtc[RTC_DATE_REG] & RTC_DATE_MASK,
	rtc[RTC_MONTH_REG] & RTC_MONTH_MASK,
	rtc[RTC_YEAR_REG]
   );
   nlcd_put_string(buf,0,2);
}

/* ----------------------------------------------------------------------- */
uint8_t buf[RTC_REGS_SIZE];
/*---------------------------- Main Code -------------------------------*/

int main(void)
{
  LED_Init();
  GPIO_SetBits(GPIOD,GPIO_Pin_14);
  I2C_BusInit();
//  printf("\n-----------------------------------\n");
  //printf("STM32F4-Discovery Board is booting!\n");
  //printf("UART and I2C-Bus Demo for STM32F4  \n");
  //printf("-----------------------------------\n");
  while(I2C_CheckDevice(I2C_EEPROM_ADDR)!= OK){


  };
  GPIO_ResetBits(GPIOD, GPIO_Pin_13|GPIO_Pin_12|GPIO_Pin_14|GPIO_Pin_15);


  store_to_eeprom("String-A ",0x0020);
  GPIO_ResetBits(GPIOD, GPIO_Pin_13|GPIO_Pin_12|GPIO_Pin_14|GPIO_Pin_15);
  read_from_eeprom(myStri, 0x0020);
      store_to_eeprom("String-B ",0x0040);
      GPIO_ResetBits(GPIOD, GPIO_Pin_13|GPIO_Pin_12|GPIO_Pin_14|GPIO_Pin_15);
      store_to_eeprom("String-C ",0x0060);
      GPIO_ResetBits(GPIOD, GPIO_Pin_13|GPIO_Pin_12|GPIO_Pin_14|GPIO_Pin_15);
      store_to_eeprom("vsdvsdvsd",0x0080);


      read_from_eeprom(myStri, 0x0020);
      GPIO_SetBits(GPIOD, GPIO_Pin_13|GPIO_Pin_12);
            read_from_eeprom(myStri, 0x0040);
           read_from_eeprom(myStri, 0x0060);
           read_from_eeprom(myStri, 0x0080);
           GPIO_ResetBits(GPIOD, GPIO_Pin_13|GPIO_Pin_12|GPIO_Pin_14|GPIO_Pin_15);

  //--------------------------------------------------- RTC
/*  static const uint8_t pgm_regs[] =
      {
  		0x00,       //Start Addr
          	0x00,       //Sec
          	0x30,		//Min
  		0x20,		//Hour
  		0x04,		//Day num
  		0x05,		//day
  		0x06,		//Month
  		0x07,		//Year
  		0x00		//Config
      };
      static const uint8_t sw_addr = 0;



      write_rtc(pgm_regs);

         for( ;;)
         {
             //Send configuration registgers
     	read_rtc(buf);

             //    print_date(buf);

            // wait_ms(MEASURE_DELAY_TIME);
     	//while(10000--);
         }*/
}

