#ifndef _I2C_H
#define _I2C_H

#include "stm32f4xx.h"

#define I2C1_OPEN
//#define I2C2_OPEN

#define I2C_SCL_SPEED       (100000)    //SCL in kHz
#define I2C_OWN_ADDR        (0x00)      //use 0x00 for master

/**
 * @brief Definition for I2C1
 */  
#if defined	I2C1_OPEN
	#define Open_I2C                        I2C1
	#define Open_I2C_CLK                    RCC_APB1Periph_I2C1
	
	#define Open_I2C_SCL_PIN                GPIO_Pin_6
	#define Open_I2C_SCL_GPIO_PORT          GPIOB
	#define Open_I2C_SCL_GPIO_CLK           RCC_AHB1Periph_GPIOB
	#define Open_I2C_SCL_SOURCE             GPIO_PinSource6
	#define Open_I2C_SCL_AF                 GPIO_AF_I2C1
	
	#define Open_I2C_SDA_PIN                GPIO_Pin_9
	#define Open_I2C_SDA_GPIO_PORT          GPIOB
	#define Open_I2C_SDA_GPIO_CLK           RCC_AHB1Periph_GPIOB
	#define Open_I2C_SDA_SOURCE             GPIO_PinSource9
	#define Open_I2C_SDA_AF                 GPIO_AF_I2C1
	
	#define Open_I2C_IRQn                   I2C1_EV_IRQn
	#define I2Cx_IRQHANDLER                 I2C1_EV_IRQHandler

#elif defined I2C2_OPEN
  #define Open_I2C                        I2C2
  #define Open_I2C_CLK                    RCC_APB1Periph_I2C2

  #define Open_I2C_SCL_PIN                GPIO_Pin_10
  #define Open_I2C_SCL_GPIO_PORT          GPIOB
  #define Open_I2C_SCL_GPIO_CLK           RCC_AHB1Periph_GPIOB
  #define Open_I2C_SCL_SOURCE             GPIO_PinSource10
  #define Open_I2C_SCL_AF                 GPIO_AF_I2C2

  #define Open_I2C_SDA_PIN                GPIO_Pin_11
  #define Open_I2C_SDA_GPIO_PORT          GPIOB
  #define Open_I2C_SDA_GPIO_CLK           RCC_AHB1Periph_GPIOB
  #define Open_I2C_SDA_SOURCE             GPIO_PinSource11
  #define Open_I2C_SDA_AF                 GPIO_AF_I2C2

  #define Open_I2C_IRQn                   I2C2_EV_IRQn
  #define I2Cx_IRQHANDLER                 I2C2_EV_IRQHandler

#else
	#error "Please select the I2C-Device to be used (in i2c.h)"
#endif 		 

int  I2C_ReadTransfer(u08 dev_addr, u08 *buffer, int cnt, u32 ptr, u08 ptrlen);
int  I2C_WriteTransfer(u08 dev_addr, u08 *buffer, int cnt, u32 ptr, u08 ptrlen);
int  I2C_CheckDevice(u08 dev_addr);
void I2C_BusInit(void);

#endif /*_I2C_H*/
