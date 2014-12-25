#include <stm32f4xx.h>
#include "nRF24L01.h"

void nRF24L01_gpio_init()
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
    // CE & CSn
    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);

    gpio.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD,&gpio);
}

void nRF24L01_spi_init()
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);

    gpio.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC,&gpio);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

    SPI_InitTypeDef spi1;
    SPI_StructInit(&spi1);

    spi1.SPI_Mode = SPI_Mode_Master;
    spi1.SPI_DataSize = SPI_DataSize_8b;
    spi1.SPI_NSS = SPI_NSS_Soft;
    spi1.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    SPI_Init(SPI3,&spi1);
    
    SPI_Cmd(SPI3,ENABLE);
}

u8 nRF24L01_spi_send(u8 data)
{
	/* Fill output buffer with data */
    SPI3->DR = data;
    /* Wait for transmission to complete */
    while (!SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE));
    /* Wait for received data to complete */
    while (!SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE));
    /* Wait for SPI to be ready */
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY));
    /* Return data from buffer */
    return SPI3->DR;
}

void nRF24L01_init()
{  
    nRF24L01_gpio_init();
    
    nRF24L01_CS_HIGH;
    nRF24L01_CE_LOW;
    
    nRF24L01_spi_init();
    
    //select channel
    nRF24L01_write_reg(nRF24L01_RF_CH_REG, 15);
    
    //select data size
    nRF24L01_write_reg(nRF24L01_RX_PW_P0_REG, 32);
    nRF24L01_write_reg(nRF24L01_RX_PW_P1_REG, 32);
    nRF24L01_write_reg(nRF24L01_RX_PW_P2_REG, 32);
    nRF24L01_write_reg(nRF24L01_RX_PW_P3_REG, 32);
    nRF24L01_write_reg(nRF24L01_RX_PW_P4_REG, 32);
    
    nRF24L01_set_rf(nRF24L01_DataRate_2M, nRF24L01_OutputPower_0dBm);
    
    nRF24L01_write_reg(nRF24L01_CONFIG_REG, NRF24L01_CONFIG);
    
    //Enable auto-acknowledgment for all pipes
    nRF24L01_write_reg(nRF24L01_EN_AA_REG, 0x3f);
    
    //Enable RX addresses
    nRF24L01_write_reg(nRF24L01_EN_RXADDR_REG, 0x3f);
    
    //Auto retransmit delay: 1000 (4x250) us and Up to 15 retransmit trials
    nRF24L01_write_reg(nRF24L01_SETUP_RETR_REG, 0x4f);

    //Dynamic length configurations: No dynamic length
    nRF24L01_write_reg(nRF24L01_DYNPD_REG, 0);
    
    //clear fifo`s
    nRF24L01_flush_tx();
    nRF24L01_flush_rx();
    
    //clear interrupts
    nRF24L01_ClearStatus();

    nRF24L01_configure_rx();
}

u8 nRF24L01_read_reg(u8 reg)
{
    u8 res = 0;
    reg &= 0x1F; // 5bit reg num
    
    nRF24L01_CS_LOW;
    nRF24L01_spi_send(nRF24L01_R_REGISTER | reg);
    res = nRF24L01_spi_send(nRF24L01_NOP);
    nRF24L01_CS_HIGH;
    
    return res;
}

u8 nRF24L01_write_reg(u8 reg, u8 data)
{
    u8 resp;
    reg &= 0x1F; // 5bit reg num
    
    nRF24L01_CS_LOW;
    
    resp = nRF24L01_spi_send(nRF24L01_W_REGISTER | reg);
    resp = nRF24L01_spi_send(data);
    
    nRF24L01_CS_HIGH;
    
    return resp;
}

void nRF24L01_configure_rx()
{
    nRF24L01_flush_rx();
    nRF24L01_ClearStatus();
    nRF24L01_CE_LOW;
    nRF24L01_write_reg(nRF24L01_CONFIG_REG, NRF24L01_CONFIG | 1 << NRF24L01_PWR_UP | 1 << NRF24L01_PRIM_RX);
    nRF24L01_CE_HIGH;
}

u8 nRF24L01_writeTx(u8 *data)
{
    nRF24L01_CE_LOW;
    nRF24L01_ClearStatus();
    nRF24L01_write_reg(nRF24L01_CONFIG_REG, NRF24L01_CONFIG | (0 << NRF24L01_PRIM_RX) | (1 << NRF24L01_PWR_UP));
    nRF24L01_flush_tx();
    nRF24L01_write_payload(data,32);
    nRF24L01_CE_HIGH;
    return 0;
}

u8 nRF24L01_readRx(u8 *resp,u8 len)
{
    nRF24L01_CS_LOW;
    nRF24L01_spi_send(nRF24L01_R_RX_PAYLOAD);
    while(len--)
    {
        *resp++ = nRF24L01_spi_send(nRF24L01_NOP);
    }
    nRF24L01_CS_HIGH;

    return nRF24L01_read_reg(nRF24L01_STATUS_REG);
}

nRF24L01_STATUS_REGISTER nRF24L01_readStatus()
{
    nRF24L01_STATUS_REGISTER reg;
    reg.all = nRF24L01_read_reg(nRF24L01_STATUS_REG);
    
    return reg; 
}

void nRF24L01_ClearStatus()
{
    nRF24L01_WriteBit(nRF24L01_STATUS_REG,4,Bit_SET);
    nRF24L01_WriteBit(nRF24L01_STATUS_REG,5,Bit_SET);
    nRF24L01_WriteBit(nRF24L01_STATUS_REG,6,Bit_SET);
}

void nRF24L01_WriteBit(uint8_t reg, uint8_t bit, BitAction value) 
{
	u8 tmp;
	tmp = nRF24L01_read_reg(reg);
	if (value != Bit_RESET) {
		tmp |= 1 << bit;
	} else {
		tmp &= ~(1 << bit);
	}
	nRF24L01_write_reg(reg, tmp);
}


void nRF24L01_write_regm(u8 reg, u8 *data, u8 len)
{
    reg &= 0x1F; // 5bit reg num
    
    nRF24L01_CS_LOW;
    
    nRF24L01_spi_send(nRF24L01_W_REGISTER | reg);
    while(len--)
    {
        nRF24L01_spi_send(*data++);
    }
    nRF24L01_CS_HIGH; 
}

void nRF24L01_read_regm(u8 reg, u8 *data, u8 len)
{
    reg &= 0x1F; // 5bit reg num
    
    nRF24L01_CS_LOW;
    
    nRF24L01_spi_send(nRF24L01_R_REGISTER | reg);
    while(len--)
    {
        *data++ = nRF24L01_spi_send(nRF24L01_NOP);
    }
    nRF24L01_CS_HIGH; 
}

void nRF24L01_write_payload(u8 *data, u8 len)
{
    nRF24L01_CS_LOW;
    nRF24L01_spi_send(nRF24L01_W_TX_PAYLOAD);
    while(len--)
    {
        nRF24L01_spi_send(*data++);
    }
    nRF24L01_CS_HIGH;
}

void nRF24L01_flush_tx()
{
    nRF24L01_CS_LOW;
    nRF24L01_spi_send(nRF24L01_FLUSH_TX);
    nRF24L01_CS_HIGH;
}

void nRF24L01_flush_rx()
{
    nRF24L01_CS_LOW;
    nRF24L01_spi_send(nRF24L01_FLUSH_RX);
    nRF24L01_CS_HIGH;
}

void nRF24L01_set_rf(nRF24L01_DataRate_type dr, nRF24L01_OutputPower_type pow)
{
    u8 tmp = 0;
    if (dr == nRF24L01_DataRate_2M)
    {
        tmp |= 1 << NRF24L01_RF_DR_HIGH;
    } else if (dr == nRF24L01_DataRate_250k) {
		tmp |= 1 << NRF24L01_RF_DR_LOW;
	}
	//If 1Mbps, all bits set to 0
    
  	if (pow == nRF24L01_OutputPower_0dBm) {
		tmp |= 3 << NRF24L01_RF_PWR;
	} else if (pow == nRF24L01_OutputPower_M6dBm) {
		tmp |= 2 << NRF24L01_RF_PWR;
	} else if (pow == nRF24L01_OutputPower_M12dBm) {
		tmp |= 1 << NRF24L01_RF_PWR;
	}

    nRF24L01_write_reg(nRF24L01_RF_SETUP_REG, tmp);

}

void nRF24L01_set_my_addr(u8 *addr)
{
    nRF24L01_CE_LOW;
    nRF24L01_write_regm(nRF24L01_RX_ADDR_P1_REG,addr,5);
    nRF24L01_CE_HIGH;
}

void nRF24L01_set_tx_addr(u8 *addr)
{
    nRF24L01_write_regm(nRF24L01_RX_ADDR_P0_REG,addr,5);
    nRF24L01_write_regm(nRF24L01_TX_ADDR_REG,addr,5);
}

void nRF24L01_select_channel(u8 ch)
{
    nRF24L01_write_reg(nRF24L01_RF_CH_REG, ch);
}