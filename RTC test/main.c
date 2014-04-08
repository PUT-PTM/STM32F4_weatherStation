#include "stm32f4xx_gpio.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_rcc.h"

#define I2C_RTC_ADDR 0xD0		/*I2c RTC sensor addr */
#define I2C_TIMEOUT_MAX 0xF00
#define MEM_DEVICE_WRITE_ADDR 0x48


//brak testow

void I2C1_Init(void)
{
GPIO_InitTypeDef GPIO_InitStructure;
I2C_InitTypeDef I2C_InitStructure;

RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

/* Konfiguracja Pinów I2C: SCL i SDA */
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
GPIO_Init(GPIOB, &GPIO_InitStructure);

/* Konfiguracja I2C */
I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
I2C_InitStructure.I2C_ClockSpeed = 100000;

/* W³¹czenie I2C */
I2C_Cmd(I2C1, ENABLE);
/* Potwierdzamy konfiguracjê przed w³¹czeniem I2C */
I2C_Init(I2C1, &I2C_InitStructure);
}

uint8_t Read(uint8_t adresa)
{
	uint8_t Data;
	/* While the bus is busy */
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

	/* Send START condition */
	I2C_GenerateSTART(I2C1, ENABLE);

	/* Test on EV5 and clear it */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	/* Send EEPROM address for read */
	I2C_Send7bitAddress(I2C1,  I2C_RTC_ADDR, I2C_Direction_Transmitter);

	/* Test on EV6 and clear it */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	/* Clear EV6 by setting again the PE bit */
	I2C_Cmd(I2C1, ENABLE);

	/* Send the EEPROM's internal address to read from: MSB of the address first */
	I2C_SendData(I2C1, adresa);

	/* Test on EV8 and clear it */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	/* Send START condition a second time */
	I2C_GenerateSTART(I2C1, ENABLE);

	/* Test on EV5 and clear it */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	/* Send EEPROM address for read */
	I2C_Send7bitAddress(I2C1,  I2C_RTC_ADDR, I2C_Direction_Receiver);

	/* Test on EV6 and clear it */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	/* Disable Acknowledgement */
	I2C_AcknowledgeConfig(I2C1, DISABLE);

	/* Send STOP Condition */
	I2C_GenerateSTOP(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)){}

	/* Read a byte from the EEPROM */
	Data = I2C_ReceiveData(I2C1);

	/* Enable Acknowledgement to be ready for another reception */
	I2C_AcknowledgeConfig(I2C1, ENABLE);

	return(Data);
}


uint8_t Write(uint16_t Addr, uint8_t Data)
{

	uint32_t timeout = I2C_TIMEOUT_MAX;
	uint8_t upper_addr,lower_addr;

	lower_addr = (uint8_t)((0x00FF)&Addr);


	Addr = Addr;
	upper_addr = (uint8_t)((0x00FF)&Addr);

       /* Generate the Start Condition */
       I2C_GenerateSTART(I2C1, ENABLE);

       /* Test on I2C1 EV5, Start trnsmitted successfully and clear it */
       timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
       while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
       {
            /* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
       }

       /* Send Memory device slave Address for write */
       I2C_Send7bitAddress(I2C1, MEM_DEVICE_WRITE_ADDR, I2C_Direction_Transmitter);

       /* Test on I2C1 EV6 and clear it */
       timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
       while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
       {
           /* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
       }

		/* Send I2C1 location address LSB */
		I2C_SendData(I2C1, upper_addr);

		/* Test on I2C1 EV8 and clear it */
		timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			/* If the timeout delay is exeeded, exit with error code */
			if ((timeout--) == 0) return 0xFF;
		}

	/* Send I2C1 location address LSB */
        I2C_SendData(I2C1, lower_addr);

      /* Test on I2C1 EV8 and clear it */
      timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
      while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
      {
            /* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
      }

       /* Send Data */
       I2C_SendData(I2C1, Data);

        /* Test on I2C1 EV8 and clear it */
        timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
             /* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
        }

        /* Send I2C1 STOP Condition */
        I2C_GenerateSTOP(I2C1, ENABLE);

        /* If operation is OK, return 0 */
        return 0;
}
/* ----------------------------------------------------------------------- */

//RGB I2c devaddr
#define I2C_SPEED 100000
#define I2C_RTC_ADDR 0xD0		/*I2c RTC sensor addr */
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


 //  char buf[24];
   //Display hour
   /*
           buf,sizeof(buf),"%02x:%02x:%02x",
           rtc[RTC_HOUR_CENTURY_REG] & RTC_HOUR_MASK,
           rtc[RTC_MIN_REG]& RTC_MIN_MASK,
           rtc[RTC_SEC_REG] & RTC_SEC_MASK
   */

   //Display date
   /*
	buf,sizeof(buf),"%02x-%02x-%02x",
	rtc[RTC_DATE_REG] & RTC_DATE_MASK,
	rtc[RTC_MONTH_REG] & RTC_MONTH_MASK,
	rtc[RTC_YEAR_REG]*/




/* ----------------------------------------------------------------------- */
int main(void)
{
	I2C1_Init();
    //Register configuration
    static const uint8_t pgm_regs[] =
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
    uint8_t buf[RTC_REGS_SIZE];
    int code = i2cm_transfer_7bit(I2C_RTC_ADDR,pgm_regs,sizeof(pgm_regs),NULL,0);   // do zrobienia odczzytywanie i zapisywanie wielu bajtow
    if(code<0)
    {
        print_i2cerr(code);
        for(;;);    //Fail never end loop
    }
    for( ;;)
    {
        //Send configuration registgers
	code = i2cm_transfer_7bit( I2C_RTC_ADDR,&sw_addr,sizeof(sw_addr),buf, sizeof(buf) );
        if(code==0)
        {
            print_date(buf);
        }
        else
        {
            print_i2cerr(code);
            for(;;);    //Fail never end loop
        }
        wait_ms(MEASURE_DELAY_TIME);
    }
    return 0;
}
