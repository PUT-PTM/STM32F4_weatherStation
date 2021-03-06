#include "stm32f4xx_gpio.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_rcc.h"
#include "i2c_common.h"



//Print rtc date
void date(const uint8_t *rtc)
{
	RTC_data.godziny =  (rtc[RTC_HOUR_CENTURY_REG] & RTC_HOUR_MASK);
	RTC_data.minuty =  rtc[RTC_MIN_REG]& RTC_MIN_MASK;
	RTC_data.sekundy =   rtc[RTC_SEC_REG] & RTC_SEC_MASK;

	RTC_data.numerdnia = rtc[RTC_DAY_REG]&RTC_DAY_MASK;
	RTC_data.dzien = rtc[RTC_DATE_REG] & RTC_DATE_MASK;
	RTC_data.miesiac = rtc[RTC_MONTH_REG] & RTC_MONTH_MASK;
	RTC_data.rok = rtc[RTC_YEAR_REG];
	RTC_data.rejestr = rtc[RTC_CONTROL_REG];
}

// KONFIGURACJA I2C - działająca
void init_I2C1(void)
{

	GPIO_InitTypeDef GPIO_InitStruct; // this is for the GPIO pins used as I2C1SDA and I2C1SCL
	GPIO_InitTypeDef GPIO_Output;     // For some debugging LEDs
	I2C_InitTypeDef I2C_InitStruct; // this is for the I2C1 initilization

	/* enable APB1 peripheral clock for I2C1*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	/* enable the peripheral clock for the pins used by	PB6 for I2C SCL and PB9 for I2C1_SDL*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* This sequence sets up the I2C1SDA and I2C1SCL pins so they work correctly with the I2C1 peripheral*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // Pin 6(I2C1_SCL) Pin 7(I2C1_SDA)
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; // the pins are configured as alternate function so the USART peripheral has access to them
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;// this defines the IO speed and has nothing to do with the baudrate!
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;// this defines the output type as open drain
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;// this activates the pullup resistors on the IO pins
	GPIO_Init(GPIOB, &GPIO_InitStruct);// now all the values are passed to the GPIO_Init()

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	 /* Configure PD12, PD13, PD14 and PD15 in output pushpull mode */
    GPIO_Output.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
    GPIO_Output.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Output.GPIO_OType = GPIO_OType_PP;
    GPIO_Output.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Output.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_Output);

	/* The I2C1_SCL and I2C1_SDA pins are now connected to their AF so that the I2C1 can take over control of the pins */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);

	 /* Configure I2C1 */
    I2C_DeInit(I2C1);

    /* Enable the I2C peripheral */
    I2C_Cmd(I2C1, ENABLE);

    /* Set the I2C structure parameters */
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStruct.I2C_OwnAddress1 = 0xEE;
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStruct.I2C_ClockSpeed = 400000;

    /* Initialize the I2C peripheral w/ selected parameters */
    I2C_Init(I2C1,&I2C_InitStruct);
}


uint8_t Write(uint16_t Addr, uint16_t Data)   // wysyłanie
{
	uint32_t timeout = I2C_TIMEOUT_MAX;

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
    I2C_SendData(I2C1, Addr);

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

uint8_t Read(uint16_t Addr) // odbieranie sygnału - do uzupełnienia
{
	  uint32_t timeout = I2C_TIMEOUT_MAX;
	  uint8_t Data = 0;

	  /* Generate the Start Condition */
	  I2C_GenerateSTART(I2C1, ENABLE);

	  /* Test on I2C1 EV5 and clear it */
	  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
	  {
	    /* If the timeout delay is exceeded, exit with error code */
			if ((timeout--) == 0) return 0xFF;
	  }

	  I2C_Send7bitAddress(I2C1, MEM_DEVICE_WRITE_ADDR, I2C_Direction_Transmitter);

	  /* Test on I2C1 EV6 and clear it */
	  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	  {
	    /* If the timeout delay is exeeded, exit with error code */
			if ((timeout--) == 0) return 0xFF;
	  }

	  /* Send I2C1 location address LSB */
	  I2C_SendData(I2C1,Addr);

			/* Test on I2C1 EV8 and clear it */
	  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	  {
		  /* If the timeout delay is exeeded, exit with error code */
		  if ((timeout--) == 0) return 0xFF;
	  }

	  I2C_GenerateSTART(I2C1, ENABLE);

	  /* Test on I2C1 EV6 and clear it */
	  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
	  {
		  /* If the timeout delay is exeeded, exit with error code */
		  if ((timeout--) == 0) return 0xFF;
	  }

	  I2C_Send7bitAddress(I2C1, MEM_DEVICE_READ_ADDR, I2C_Direction_Receiver);

	  /* Test on I2C1 EV6 and clear it */
	  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	  {
		  /* If the timeout delay is exeeded, exit with error code */
		  if ((timeout--) == 0) return 0xFF;
	  }

	  /* Prepare an NACK for the next data received */
	  I2C_AcknowledgeConfig(I2C1, DISABLE);

	  /* Test on I2C1 EV7 and clear it */
	  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
	  {
		  /* If the timeout delay is exeeded, exit with error code */
	      if ((timeout--) == 0) return 0xFF;
	  }
	  I2C_GenerateSTOP(I2C1, ENABLE);  //
	  Data = I2C_ReceiveData(I2C1);

	  return Data;
}

/*-------------------------------------------------------------------------------------------*/
/*---------------------------------         RTC        --------------------------------------*/
/*-------------------------------------------------------------------------------------------*/

uint8_t Write_RTC(uint16_t Addr, uint16_t Data)   // wysyłanie
{
	uint32_t timeout = I2C_TIMEOUT_MAX;

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
    I2C_Send7bitAddress(I2C1, MEM_RTC_ADDR, I2C_Direction_Transmitter);
    /* Test on I2C1 EV6 and clear it */
    timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
    	/* If the timeout delay is exeeded, exit with error code */
    	if ((timeout--) == 0) return 0xFF;
	}

    /* Send I2C1 location address LSB */
    I2C_SendData(I2C1, Addr);

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

uint8_t Read_RTC(uint16_t Addr)
{
	  uint32_t timeout = I2C_TIMEOUT_MAX;
	  uint8_t Data = 0;

	  /* Generate the Start Condition */
	  I2C_GenerateSTART(I2C1, ENABLE);

	  /* Test on I2C1 EV5 and clear it */
	  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
	  {
	    /* If the timeout delay is exceeded, exit with error code */
			if ((timeout--) == 0) return 0xFF;
	  }

	  I2C_Send7bitAddress(I2C1, MEM_RTC_ADDR, I2C_Direction_Transmitter);

	  /* Test on I2C1 EV6 and clear it */
	  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	  {
	    /* If the timeout delay is exeeded, exit with error code */
			if ((timeout--) == 0) return 0xFF;
	  }

	  /* Send I2C1 location address LSB */
	  I2C_SendData(I2C1,Addr);

			/* Test on I2C1 EV8 and clear it */
	  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	  {
		  /* If the timeout delay is exeeded, exit with error code */
		  if ((timeout--) == 0) return 0xFF;
	  }

	  I2C_GenerateSTART(I2C1, ENABLE);

	  /* Test on I2C1 EV6 and clear it */
	  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
	  {
		  /* If the timeout delay is exeeded, exit with error code */
		  if ((timeout--) == 0) return 0xFF;
	  }

	  I2C_Send7bitAddress(I2C1, MEM_RTC_ADDR, I2C_Direction_Receiver);

	  /* Test on I2C1 EV6 and clear it */
	  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	  {
		  /* If the timeout delay is exeeded, exit with error code */
		  if ((timeout--) == 0) return 0xFF;
	  }

	  /* Prepare an NACK for the next data received */
	  I2C_AcknowledgeConfig(I2C1, DISABLE);

	  /* Test on I2C1 EV7 and clear it */
	  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
	  {
		  /* If the timeout delay is exeeded, exit with error code */
	      if ((timeout--) == 0) return 0xFF;
	  }
	  I2C_GenerateSTOP(I2C1, ENABLE);  //
	  Data = I2C_ReceiveData(I2C1);

	  return Data;
}

void ReadRTCDate()
{
	buf[RTC_SEC_REG]=Read_RTC(0x00);
	 buf[RTC_HOUR_CENTURY_REG]=Read_RTC(0x02);
	  buf[RTC_MIN_REG]=Read_RTC(0x01);
	   buf[RTC_DAY_REG]=Read_RTC(0x03);
		buf[RTC_DATE_REG]= Read_RTC(0x04);
		buf[RTC_MONTH_REG]=Read_RTC(0x05);
		buf[RTC_YEAR_REG]=Read_RTC(0x06);
		buf[RTC_CONTROL_REG]=Read_RTC(0x07);

}

void WriteDate(uint8_t* buf)
{

	uint8_t i=0x00;
	for(i;i<(uint8_t)sizeof(buf)+1;i++)
	{
		Write_RTC(i,buf[i]);

	}
}
