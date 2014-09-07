#ifndef __NRF24L01_H
#define __NRF24L01_H

// макросы управления CS
// устанавливает ножку CS в 1 состояние
#define nRF24L01_CS_SET     GPIOC->BSRRL = GPIO_BSRR_BS_9
// сбрасывает ножку CS в 0 состояние
#define nRF24L01_CS_RESET   GPIOC->BSRRH = GPIO_BSRR_BS_9

// инициализация
void nRF24L01_init();
void nRF24L01_spi_init();

/**
* Посылка команды
* @return ответ на запрос
**/
u8 nRF24L01_cmd_send(u8 reg, u8 cmd);

/**
* Отправка данных по spi
* @return полученные данные
**/
u8 spi_send(u8 data);

// регистры nRF24L01
enum nRF24L01_REG {
    CONFIG_REG = 0,
    EN_AA_REG,
    EN_RXADDR_REG,
    SETUP_AW_REG,
    SETUP_RETR_REG,
    RF_CH_REG,
    RF_SETUP_REG,
    STATUS_REG,
    OBSERVE_TX_REG,
    RPD_REG,
    RX_ADDR_P0_REG,
    RX_ADDR_P1_REG,
    RX_ADDR_P2_REG,
    RX_ADDR_P3_REG,
    RX_ADDR_P4_REG,
    RX_ADDR_P5_REG,
    TX_ADDR_REG,
    RX_PW_P0_REG,
    RX_PW_P1_REG,
    RX_PW_P2_REG,
    RX_PW_P3_REG,
    RX_PW_P4_REG,
    RX_PW_P5_REG,
    FIFO_STATUS_REG,
    
    
    ACK_PLD_REG, // n/a addr
    TX_PLD_REG,  // n/a addr
    RX_PLD_REG,  // n/a addr
    
    DYNPD_REG = 0x1c, //1c
    FEATURE_REG = 0x1d
};

// команды nRF24L01
enum nRF24L01_COMMAND {
    R_REGISTER = 0,
    W_REGISTER = 0x20,
    R_RX_PAYLOAD = 0x61,
    W_TX_PAYLOAD = 0xA0,
    FLUSH_TX = 0xE1,
    FLUSH_RX = 0xE2,
    REUSE_TX_PL = 0xE3,
    R_RX_PL_WID = 0x60,
    W_ACK_PAYLOAD = 0xA8,
    W_TX_PAYLOAD_NOACK = 0xB0,
    NOP = 0xFF
    
};
#endif // __NRF24L01_H