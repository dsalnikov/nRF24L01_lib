#ifndef __NRF24L01_H
#define __NRF24L01_H

extern volatile u8 ptx;
void Delay(__IO uint32_t nTime);

#define PAYLOAD_SIZE (1)

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
#define nRF24L01_CS_SET     GPIO_ResetBits(GPIOA, GPIO_Pin_4)
// сбрасывает ножку CS в 0 состояние
#define nRF24L01_CS_RESET   GPIO_SetBits(GPIOA, GPIO_Pin_4)

// макросы управления сигналом ChipEnable
#define nRF24L01_CE_SET     GPIO_SetBits(GPIOA, GPIO_Pin_3)
#define nRF24L01_CE_RESET   GPIO_ResetBits(GPIOA, GPIO_Pin_3)

// инициализация
void nRF24L01_init();
void nRF24L01_spi_init();


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
* Настраиваем на передачу
**/
u8 nRF24L01_configure_tx();

/**
* Настраиваем на прием
**/
u8 nRF24L01_configure_rx();

/**
* Read rx data
* @param resp - pointer to received data
* @return status register
**/
u8 nRF24L01_readRx(u8 *resp);

/**
* Write data to TX_PAYLOAD
* @param data - data to send
* @return error code
**/
u8 nRF24L01_writeTx(u8 *data);


void nRF24L01_ClearStatus();

// регистры nRF24L01
enum nRF24L01_REG {
    nRF24L01_CONFIG_REG = 0,
    nRF24L01_EN_AA_REG,
    nRF24L01_EN_RXADDR_REG,
    nRF24L01_SETUP_AW_REG,
    nRF24L01_SETUP_RETR_REG,
    nRF24L01_RF_CH_REG,
    nRF24L01_RF_SETUP_REG,
    nRF24L01_STATUS_REG,
    nRF24L01_OBSERVE_TX_REG,
    nRF24L01_RPD_REG,
    nRF24L01_RX_ADDR_P0_REG,
    nRF24L01_RX_ADDR_P1_REG,
    nRF24L01_RX_ADDR_P2_REG,
    nRF24L01_RX_ADDR_P3_REG,
    nRF24L01_RX_ADDR_P4_REG,
    nRF24L01_RX_ADDR_P5_REG,
    nRF24L01_TX_ADDR_REG,
    nRF24L01_RX_PW_P0_REG,
    nRF24L01_RX_PW_P1_REG,
    nRF24L01_RX_PW_P2_REG,
    nRF24L01_RX_PW_P3_REG,
    nRF24L01_RX_PW_P4_REG,
    nRF24L01_RX_PW_P5_REG,
    nRF24L01_FIFO_STATUS_REG,
    
    
    nRF24L01_ACK_PLD_REG, // n/a addr
    nRF24L01_TX_PLD_REG,  // n/a addr
    nRF24L01_RX_PLD_REG,  // n/a addr
    
    nRF24L01_DYNPD_REG = 0x1c, //1c
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

void nRF24L01_write_regm(u8 reg, u8 *data, u8 len);

void nRF24L01_read_regm(u8 reg, u8 *data, u8 len);


#endif // __NRF24L01_H