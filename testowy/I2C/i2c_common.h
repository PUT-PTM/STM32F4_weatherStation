#define I2C_TIMEOUT_MAX 0xF000


#define MEM_DEVICE_WRITE_ADDR 0x90
#define MEM_DEVICE_READ_ADDR 0x90
#define MEM_RTC_ADDR 0xD0

struct RTC_Data_struct
{
  uint8_t rejestr;
  uint8_t numerdnia;
  uint8_t minuty;
  uint8_t sekundy;
  uint8_t dzien;
  uint8_t miesiac;
  uint8_t rok;
  uint8_t godziny;
 uint8_t temp;
};

struct RTC_Data_struct RTC_data;

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

#define RTC_HOUR_MASK 0x3f
#define RTC_MIN_MASK 0x7f
#define RTC_SEC_MASK 0x7F
#define RTC_DATE_MASK 0x3f
#define RTC_MONTH_MASK 0x1f
#define RTC_DAY_MASK 0x07

uint8_t buf[RTC_REGS_SIZE];

// KONFIGURACJA I2C - dzia³aj¹ca
void init_I2C1(void);
uint8_t Write(uint16_t Addr, uint16_t Data);
uint8_t Read(uint16_t Addr);

uint8_t Write_RTC(uint16_t Addr, uint16_t Data);
uint8_t Read_RTC(uint16_t Addr);


void date(const uint8_t *rtc);
void ReadRTCDate();
void WriteDate(uint8_t* buf);

