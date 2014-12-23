#ifndef __NRF24L01_H
#define __NRF24L01_H

extern volatile u8 ptx;
void Delay(__IO uint32_t nTime);

//RF setup register
#define NRF24L01_PLL_LOCK		4
#define NRF24L01_RF_DR_LOW		5
#define NRF24L01_RF_DR_HIGH		3
#define NRF24L01_RF_DR			3
#define NRF24L01_RF_PWR			1 //2 bits   

//Configuration register
#define NRF24L01_MASK_RX_DR		6
#define NRF24L01_MASK_TX_DS		5
#define NRF24L01_MASK_MAX_RT	4
#define NRF24L01_EN_CRC			3
#define NRF24L01_CRCO			2
#define NRF24L01_PWR_UP			1
#define NRF24L01_PRIM_RX		0

#define NRF24L01_CONFIG			((1 << NRF24L01_EN_CRC) | (0 << NRF24L01_CRCO))


typedef enum {
	nRF24L01_DataRate_2M,		// 2Mbps
	nRF24L01_DataRate_1M,		// 1Mbps
	nRF24L01_DataRate_250k		// 250kbps
} nRF24L01_DataRate_type;

typedef enum {
	nRF24L01_OutputPower_M18dBm,	// -18dBm
	nRF24L01_OutputPower_M12dBm,	// -12dBm
	nRF24L01_OutputPower_M6dBm,	// -6dBm
	nRF24L01_OutputPower_0dBm	// 0dBm
} nRF24L01_OutputPower_type;

// соединение с discovery
//
// CE --> pa3
// CSn --> pa4
// miso --> pa6
// mosi --> pa7
// sck --> pa5
// irq --> pa2

// макросы управления CS
// устанавливает ножку CS в 1 состояние
#define nRF24L01_CS_LOW     GPIOD->BSRRH = GPIO_Pin_7
// сбрасывает ножку CS в 0 состояние
#define nRF24L01_CS_HIGH    GPIOD->BSRRL = GPIO_Pin_7

// макросы управления сигналом ChipEnable
#define nRF24L01_CE_LOW     GPIOD->BSRRH = GPIO_Pin_8
#define nRF24L01_CE_HIGH    GPIOD->BSRRL = GPIO_Pin_8

// инициализация
void nRF24L01_init();
void nRF24L01_spi_init();

void nRF24L01_set_rf(nRF24L01_DataRate_type dr, nRF24L01_OutputPower_type pow);
void nRF24L01_set_my_addr(u8 *addr);
void nRF24L01_set_tx_addr(u8 *addr);
void nRF24L01_WriteBit(uint8_t reg, uint8_t bit, BitAction value);

void nRF24L01_write_payload(u8 *data, u8 len);

/**
* Отправка данных по spi
* @return полученные данные
**/
u8 nRF24L01_spi_send(u8 data);

/**
* Читаем регистр
**/
u8 nRF24L01_read_reg(u8 reg, u8 *resp, u8 len);

/**
* Пишем регистр
**/
u8 nRF24L01_write_reg(u8 reg, u8 data);

/**
* Настраиваем на прием
**/
void nRF24L01_configure_rx();

/**
* Read rx data
* @param resp - pointer to received data
* @param len - count of bytes
* @return status register
**/
u8 nRF24L01_readRx(u8 *resp,u8 len);

/**
* Write data to TX_PAYLOAD
* @param data - data to send
* @return error code
**/
u8 nRF24L01_writeTx(u8 *data);


void nRF24L01_ClearStatus();

// регистры nRF24L01
enum nRF24L01_REG {
    nRF24L01_CONFIG_REG = 0x00,
    nRF24L01_EN_AA_REG = 0x01,
    nRF24L01_EN_RXADDR_REG = 0x02,
    nRF24L01_SETUP_AW_REG = 0x03,
    nRF24L01_SETUP_RETR_REG = 0x04,
    nRF24L01_RF_CH_REG = 0x05,
    nRF24L01_RF_SETUP_REG = 0x06,
    nRF24L01_STATUS_REG = 0x07,
    nRF24L01_OBSERVE_TX_REG = 0x08,
    nRF24L01_RPD_REG = 0x09,
    nRF24L01_RX_ADDR_P0_REG = 0x0A,
    nRF24L01_RX_ADDR_P1_REG = 0x0B,
    nRF24L01_RX_ADDR_P2_REG = 0x0C,
    nRF24L01_RX_ADDR_P3_REG = 0x0D,
    nRF24L01_RX_ADDR_P4_REG = 0x0E,
    nRF24L01_RX_ADDR_P5_REG = 0x0F,
    nRF24L01_TX_ADDR_REG = 0x10,
    nRF24L01_RX_PW_P0_REG = 0x11,
    nRF24L01_RX_PW_P1_REG = 0x12,
    nRF24L01_RX_PW_P2_REG = 0x13,
    nRF24L01_RX_PW_P3_REG = 0x14,
    nRF24L01_RX_PW_P4_REG = 0x15,
    nRF24L01_RX_PW_P5_REG = 0x16,
    nRF24L01_FIFO_STATUS_REG = 0x17,
        
    nRF24L01_DYNPD_REG = 0x1c,
    nRF24L01_FEATURE_REG = 0x1d,
    
    nRF24L01_REG_MAX // максимальный номер регистра
};

// команды nRF24L01
enum nRF24L01_COMMAND {
    nRF24L01_R_REGISTER = 0,
    nRF24L01_W_REGISTER = 0x20,
    nRF24L01_R_RX_PAYLOAD = 0x61,
    nRF24L01_W_TX_PAYLOAD = 0xA0,
    nRF24L01_FLUSH_TX = 0xE1,
    nRF24L01_FLUSH_RX = 0xE2,
    nRF24L01_REUSE_TX_PL = 0xE3,
    nRF24L01_R_RX_PL_WID = 0x60,
    nRF24L01_nRF24L01_W_ACK_PAYLOAD = 0xA8,
    nRF24L01_W_TX_PAYLOAD_NOACK = 0xB0,
    nRF24L01_NOP = 0xFF
    
};

struct nRF24L01_CONFIG_REG_BITS {
    u8 PRIM_RX : 1;
    u8 PWR_UP : 1;
    u8 CRCO : 1;
    u8 EN_CRC : 1;
    u8 MASK_MAX_RT : 1;
    u8 MASK_TX_DS : 1;
    u8 MASK_RX_DR : 1;
    u8 res : 1;
};

typedef  union {
    struct nRF24L01_CONFIG_REG_BITS bit;
    u8 all;
} nRF24L01_CONFIG_REGISTER;


struct nRF24L01_SETUP_RETR_REG_BITS {
    u8 ARC : 4; //0..3
    u8 ARDa : 4; //4..7
};

typedef  union {
    struct nRF24L01_SETUP_RETR_REG_BITS bit;
    u8 all;
} nRF24L01_SETUP_RETR_REGISTER;

struct nRF24L01_STATUS_REG_BITS {
    u8 TX_FULL : 1;
    u8 RX_P_NO : 3;
    u8 MAX_RT : 1;
    u8 TX_DS : 1;
    u8 RX_DR : 1;
    u8 res : 1;
};

typedef  union {
    struct nRF24L01_STATUS_REG_BITS bit;
    u8 all;
} nRF24L01_STATUS_REGISTER;

/**
* Read status register
* @return value of status register
**/
nRF24L01_STATUS_REGISTER nRF24L01_readStatus();

/**
* Write multibyte register
* @param reg - register
* @param data - pointer to data
* @param len - count of bytes
**/
void nRF24L01_write_regm(u8 reg, u8 *data, u8 len);

/**
* Read multibyte register
* @param reg - register
* @param data - pointer to store data
* @param len - count of bytes
**/
void nRF24L01_read_regm(u8 reg, u8 *data, u8 len);

/**
* Clear tx fifo
**/
void nRF24L01_flush_tx();

/**
* Clear rx fifo
**/
void nRF24L01_flush_rx();

#endif // __NRF24L01_H