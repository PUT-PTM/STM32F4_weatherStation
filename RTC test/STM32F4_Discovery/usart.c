
#include "main.h"
#include "usart.h"

//-------------------------------------------
// Definition of local constants
//-------------------------------------------

// Buffersizes must be 2^n
#define TBUFSIZE     (256)
#define RBUFSIZE     (256)
#define TMASK        (TBUFSIZE-1)
#define RMASK        (RBUFSIZE-1)
#define RBUFLEN      (uint8_t)(r_in - r_out)
#define TBUFLEN      (uint8_t)(t_in - t_out)
#define UART_TIMEOUT (0x1000)

//-------------------------------------------
// Declaration of local variables
//-------------------------------------------
static volatile uint8_t tbuf[TBUFSIZE];
static volatile uint8_t rbuf[RBUFSIZE];
static volatile uint8_t rmsg =0;
static volatile uint8_t t_in =0;
static volatile uint8_t t_out=0;
static volatile uint8_t r_in =0;
static volatile uint8_t r_out=0;
static volatile uint8_t txien=0;

static uint8_t prompt  = 0;
static uint8_t echo    = 1;
static uint8_t newline = 0;

static void UART_PushChar(char ch);

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


/*******************************************************************************
* Function Name  : USART_Init
* Description    : Configure Open_USART 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void UART_Init(void)
{ 												
  GPIO_InitTypeDef       GPIO_InitStructure;
  USART_InitTypeDef      USART_InitStructure;
  USART_ClockInitTypeDef USART_ClockStructure;
  NVIC_InitTypeDef       NVIC_InitStructure;

  //=========================================================================================
  // Clock des USART einschalten:
  //------------------------------------------------------------------------------------------
  RCC_AHB1PeriphClockCmd(Open_USART_TX_GPIO_CLK,ENABLE);
  RCC_AHB1PeriphClockCmd(Open_USART_RX_GPIO_CLK,ENABLE);
  if ((Open_USART_CLK==RCC_APB2Periph_USART1)||(Open_USART_CLK==RCC_APB2Periph_USART6))
  {
    RCC_APB2PeriphClockCmd(Open_USART_CLK,ENABLE);
    RCC_APB2PeriphClockLPModeCmd(Open_USART_CLK,ENABLE);
  }
  else
  {
    RCC_APB1PeriphClockCmd(Open_USART_CLK,ENABLE);
    RCC_APB1PeriphClockLPModeCmd(Open_USART_CLK,ENABLE);
  }

  //=========================================================================================
  //GPIO Alternate functions configuration
  //------------------------------------------------------------------------------------------
  GPIO_PinAFConfig(Open_USART_TX_GPIO_PORT, Open_USART_TX_SOURCE, Open_USART_TX_AF);
  GPIO_PinAFConfig(Open_USART_RX_GPIO_PORT, Open_USART_RX_SOURCE, Open_USART_RX_AF);

  //=========================================================================================
  // USART Pins einschalten
  //------------------------------------------------------------------------------------------
  GPIO_InitStructure.GPIO_Pin = Open_USART_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  //------------------------------------------------------------------------------------------
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(Open_USART_TX_GPIO_PORT, &GPIO_InitStructure);
  //------------------------------------------------------------------------------------------
  GPIO_InitStructure.GPIO_Pin = Open_USART_RX_PIN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(Open_USART_RX_GPIO_PORT, &GPIO_InitStructure);

  //=========================================================================================
  // USART konfigurieren & einschalten
  //------------------------------------------------------------------------------------------
  //  - BaudRate = 115200 baud
  //  - Word Length = 8 Bits - One Stop Bit - No parity
  //  - Hardware flow control disabled (RTS and CTS signals)
  //  - Receive and transmit
  //------------------------------------------------------------------------------------------
  USART_InitStructure.USART_BaudRate = USART_BAUD;
  USART_InitStructure.USART_WordLength = USART_DATA;
  USART_InitStructure.USART_StopBits = USART_STOP;
  USART_InitStructure.USART_Parity = USART_PARITY;
  USART_InitStructure.USART_HardwareFlowControl = USART_FLOWCTRL;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  //------------------------------------------------------------------------------------------
  USART_Init(Open_USART, &USART_InitStructure);
  USART_ClockStructInit (&USART_ClockStructure);
  USART_ClockInit(Open_USART, &USART_ClockStructure);

  //=========================================================================================
  // Interrupt channel einschalten
  //------------------------------------------------------------------------------------------
  NVIC_InitStructure.NVIC_IRQChannel = Open_USART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //oder 0?
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         //oder 1?
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  //=========================================================================================
  // Interrupts einschalten
  //------------------------------------------------------------------------------------------
  //USART_ITConfig(Open_USART, USART_IT_TXE, ENABLE);  // TX register Empty - wir können was senden
  USART_ITConfig(Open_USART, USART_IT_RXNE, ENABLE); // RX register Not Empty - es wurde was empfangen

  //=========================================================================================
  // Uart einschalten
  //------------------------------------------------------------------------------------------
  USART_Cmd(Open_USART, ENABLE);
}


int UART_GetLine(char *str)
//-------------------------------------------------------
// Check if a CR-terminated line is received on UART
// if 'YES' then copy line to *str and return(StringLength)
// else return(StringLength=0)
//-------------------------------------------------------
{
  int i=0;
  if(rmsg)                               // if message in buffer available
  {
    rmsg--;                              // decrease message buffer
    str[i]=rbuf [r_out++ & RMASK];       // get first char
    while(str[i])                        // while not stringend
    {
      str[++i]=rbuf [r_out++ & RMASK];   // get next char
    }
    if (i==0)
    {
      str[0]='\r';                        // insert [CR]
      str[1]='\0';                        // insert NULL
      i=1;                                // adjust string length
    }
  }
  return(i);                              // return(StringLength)
}

//===============================================================================
// Check if cursor is on a new and empty line, else insert CR+LF
//-------------------------------------------------------------------------------
void UART_GoToNewLine(void)
{
  int timeout = UART_TIMEOUT;
  if (newline == 0)
  {
    while ((TBUFLEN > (TBUFSIZE-8)) && timeout--);  // wait a moment, if buffer full
    UART_PushChar('\n');
  }
}

//===============================================================================
// used for printf()
// - Put a char to the transmit buffer. Wait some time if
//   buffer is full, else push the char to the transmit buffer.
// - Keeping some free bytes on the buffer, needed by the RX-ISR for its echo.
//-------------------------------------------------------------------------------
int UART_PutChar(char ch)
{
  int timeout = UART_TIMEOUT;
  while ((TBUFLEN > (TBUFSIZE-8)) && timeout--);  // wait a moment, if buffer full
  UART_PushChar(ch);
  return 0;
}

//===============================================================================
// Called by UART_PutChar() or by RX-ISR echo
// - Push a char to the transmit buffer, no-wait and no check if buffer full
// - Inserts automatically a '\r' prior to any '\n'
// - Enable the UART TX Interrupt if necessary
//-------------------------------------------------------------------------------
static void UART_PushChar(char ch)
{
  if (ch == '\n')
  {
    tbuf[t_in++ & TMASK] = '\r';    // add [CR]
    tbuf[t_in++ & TMASK] = '\n';    // add [NL]
    newline = 1;
    if (prompt)
    {
      tbuf[t_in++ & TMASK] = '>';   // add [>]
    }
  }
  else
  {
    tbuf[t_in++ & TMASK] = ch;   // next char
    newline = 0;
  }
  if (txien == DISABLE)
  {
    txien  = ENABLE;
    USART_ITConfig(Open_USART, USART_IT_TXE, ENABLE);  // enable TX interrupt
  }
}



//=================================================================================
void USARTx_IRQHANDLER(void)
{
  char ch;
  //===============================================================================
  // USART RX Interrupt Service Routine
  //-------------------------------------------------------------------------------
  if (USART_GetFlagStatus(Open_USART, USART_FLAG_RXNE) == SET)
  {
    ch = USART_ReceiveData(Open_USART); // get received character
    if (echo)
    {
      UART_PushChar(ch);                // echo
    }
    if (ch=='\r')                       // if char = [CR]
    {
      rbuf[r_in++ & RMASK] = '\0';      // add stringend to buffer
      if (echo)
      {
        UART_PushChar('\n');            // append [LF] to echo
      }
      rmsg++;                           // increment message counter
    }
    if (ch=='\b')                       // if backspace
    {
      if (rbuf[(r_in-1) & RMASK])       // if there is a char, left from cursor
      {
        rbuf[(--r_in) & RMASK]=0;       // then delete it  and go one char back.
        if (echo)
        {
          UART_PushChar(' ');           // echo SPACE to delete char on Terminal
          UART_PushChar('\b');          // Back-SPACE again
        }
      }
    }
    if (ch > 0x19)                      // if printable char
    {
      rbuf[r_in++ & RMASK] = ch;        // add char to buffer
    }
  }
  //===============================================================================
  // USART TX Interrupt Service Routine
  //-------------------------------------------------------------------------------
  if (USART_GetFlagStatus(Open_USART, USART_FLAG_TXE) == SET)
  {
    // If TX-Buffer contains Data
    if (t_in != t_out)
    {
      // then send next char
      USART_SendData(Open_USART, tbuf[t_out & TMASK]);
      t_out++;   // pointer to next char
    }
    else
    {
      // else disable the DataRegisterEmpty Interrupt
      USART_ITConfig(Open_USART, USART_IT_TXE, DISABLE);
      txien  = DISABLE;
    }
  }
}
//=================================================================================


/* Use no semihosting */
#if 0
#pragma import(__use_no_semihosting)
struct __FILE
{  
	int handle;
};
FILE __stdout;

int _sys_exit(int x)
{
	x = x;
	return(0);
}
#endif

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  UART_PutChar((char)ch);
  return ch;
}
