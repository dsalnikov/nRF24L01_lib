#include <stm32f4xx.h>
#include "nRF24L01.h"

void nRF24L01_init()
{
    nRF24L01_spi_init();
    
    // CE и CSn
    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);

    gpio.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA,&gpio);
    
    nRF24L01_CS_RESET;
    nRF24L01_CE_RESET;

}

void nRF24L01_spi_init()
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);  // тактирование порта
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);  // тактирование SPI1 

    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);

    gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA,&gpio);

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);

    SPI_I2S_DeInit(SPI1);
    SPI_InitTypeDef spi1;
    SPI_StructInit(&spi1);

    spi1.SPI_Mode = SPI_Mode_Master;
    spi1.SPI_DataSize = SPI_DataSize_8b;
    spi1.SPI_NSS = SPI_NSS_Soft;
    spi1.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_Init(SPI1,&spi1);
    
    SPI_Cmd(SPI1,ENABLE);
}


u8 nRF24L01_spi_send(u8 data)
{
	/* Fill output buffer with data */
    SPI1->DR = data;
    /* Wait for transmission to complete */
    while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
    /* Wait for received data to complete */
    while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
    /* Wait for SPI to be ready */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
    /* Return data from buffer */
    return SPI1->DR;
}


u8 nRF24L01_read_reg(u8 reg, u8 *resp, u8 len)
{
    u8 i;
    u8 res = 0;
    
    reg &= 0x1F; // 5bit reg num
    
    nRF24L01_CS_SET;
    
    res = nRF24L01_spi_send(nRF24L01_R_REGISTER | reg);
    for(i=0; i < len; i++)
    {
        resp[i] = nRF24L01_spi_send(nRF24L01_NOP);
    }
    
    nRF24L01_CS_RESET;
    
    return res;
}

u8 nRF24L01_write_reg(u8 reg, u8 data)
{
    u8 resp;
    reg &= 0x1F; // 5bit reg num
    
    nRF24L01_CS_SET;
    
    resp = nRF24L01_spi_send(nRF24L01_W_REGISTER | reg);
    resp = nRF24L01_spi_send(data);
    
    nRF24L01_CS_RESET;
    
    return resp;
}

u8 nRF24L01_configure_tx()
{
    nRF24L01_CONFIG_REGISTER confReg;
    nRF24L01_SETUP_RETR_REGISTER setupRetr;
    u8 resp;
    
    // clear regs
    confReg.all = 0;
    setupRetr.all = 0;
    
    confReg.bit.PWR_UP = 1;
    confReg.bit.PRIM_RX = 0; // we are tranceiver
    confReg.bit.CRCO = 1; // 2B crc    
    
    //resp = nRF24L01_write_reg(nRF24L01_RF_CH_REG, 2);
    Delay(10);
    resp = nRF24L01_write_reg(nRF24L01_RX_PW_P0_REG, 1);
    Delay(10);
    resp = nRF24L01_write_reg(nRF24L01_CONFIG_REG, confReg.all);
    Delay(10);
    return resp;
}

u8 nRF24L01_configure_rx()
{
    nRF24L01_CONFIG_REGISTER confReg;
    nRF24L01_SETUP_RETR_REGISTER setupRetr;
    u8 resp;
    
    // clear regs
    confReg.all = 0;
    setupRetr.all = 0;
    
    confReg.bit.PWR_UP = 1;
    confReg.bit.PRIM_RX = 1; // we are receiver
    confReg.bit.CRCO = 1; // 2B crc
    
    //resp = nRF24L01_write_reg(nRF24L01_RF_CH_REG, 2);
    Delay(10);
    resp = nRF24L01_write_reg(nRF24L01_RX_PW_P0_REG, 1);
    Delay(10);
    resp = nRF24L01_write_reg(nRF24L01_CONFIG_REG, confReg.all);
    Delay(10);
    
    nRF24L01_CE_SET;
    Delay(135);
        
    //TODO:
    //config RF chenel
    //config data rate
    return resp;
}

u8 nRF24L01_readRx(u8 *resp)
{
    u8 res;
    nRF24L01_CS_SET;
    *resp++ = nRF24L01_spi_send(nRF24L01_R_RX_PAYLOAD);
    *resp++ = nRF24L01_spi_send(nRF24L01_NOP);
    //*resp++ = nRF24L01_spi_send(nRF24L01_NOP);
    //*resp++ = nRF24L01_spi_send(nRF24L01_NOP);
    //*resp++ = nRF24L01_spi_send(nRF24L01_NOP);
    //*resp++ = nRF24L01_spi_send(nRF24L01_NOP);
    
    nRF24L01_CS_RESET;
    
    return nRF24L01_read_reg(nRF24L01_STATUS_REG,resp,0);
}

u8 nRF24L01_writeTx(u8 *data)
{
    u8 res;
    u8 resp;
    u32 i = 0;
    nRF24L01_CONFIG_REGISTER confReg;
    
    
    
    /*

    resp = nRF24L01_write_reg(nRF24L01_EN_AA_REG,  0x3F);

    resp = nRF24L01_write_reg(nRF24L01_EN_RXADDR_REG,  0x3F);  
    
    resp = nRF24L01_write_reg(nRF24L01_SETUP_RETR_REG,  0x4F);
    
    resp = nRF24L01_write_reg(nRF24L01_RF_CH_REG,  2);
    
    
    // clear interrupts
    res = nRF24L01_read_reg(nRF24L01_STATUS_REG,&res,0);
    nRF24L01_write_reg(nRF24L01_STATUS_REG,res);
    
    nRF24L01_CS_SET;
    res = nRF24L01_spi_send(nRF24L01_FLUSH_TX);
    nRF24L01_CS_RESET;
    */
    
    ptx = 1;
    
    nRF24L01_CS_SET;
    res = nRF24L01_spi_send(nRF24L01_W_TX_PAYLOAD);
    res = nRF24L01_spi_send(0x55);
    nRF24L01_CS_RESET;
    
    
    //nRF24L01_write_payload(data,PAYLOAD_SIZE);
    
    for(;i<0xFFFFF;i++);
    
    nRF24L01_CE_RESET;
    
        // clear regs
    confReg.all = 0;
    
    confReg.bit.PWR_UP = 1;
    confReg.bit.PRIM_RX = 0; // we are tranceiver
    confReg.bit.CRCO = 1; // 2B crc
    confReg.bit.EN_CRC = 1;

    nRF24L01_write_reg(nRF24L01_CONFIG_REG, confReg.all);
    
    nRF24L01_CE_SET;
    for(;i<0xFFFFF;i++);
    //nRF24L01_CE_RESET;
    //for(;i<0xFFFFF;i++);
    
    return res;
}

nRF24L01_STATUS_REGISTER nRF24L01_readStatus()
{
    u8 resp;
    nRF24L01_STATUS_REGISTER reg;
    reg.all = nRF24L01_read_reg(nRF24L01_STATUS_REG,&resp,0);
    
    return reg; 
}

void nRF24L01_ClearStatus()
{
    u8 resp;
    u32 i = 0;
    u8 reg = nRF24L01_read_reg(nRF24L01_STATUS_REG,&resp,0);
    for(;i<0xFFFF;i++);
    nRF24L01_write_reg(nRF24L01_STATUS_REG, reg);
}

void nRF24L01_write_regm(u8 reg, u8 *data, u8 len)
{
    reg &= 0x1F; // 5bit reg num
    
    nRF24L01_CS_SET;
    
    nRF24L01_spi_send(nRF24L01_W_REGISTER | reg);
    while(len--)
    {
        nRF24L01_spi_send(*data++);
    }
    nRF24L01_CS_RESET; 
}

void nRF24L01_read_regm(u8 reg, u8 *data, u8 len)
{
    reg &= 0x1F; // 5bit reg num
    
    nRF24L01_CS_SET;
    
    nRF24L01_spi_send(nRF24L01_R_REGISTER | reg);
    while(len--)
    {
        *data++ = nRF24L01_spi_send(nRF24L01_NOP);
    }
    nRF24L01_CS_RESET; 
}

void nRF24L01_write_payload(u8 *data, u8 len)
{
    nRF24L01_CS_SET;
    nRF24L01_spi_send(nRF24L01_W_TX_PAYLOAD);
    while(len--)
    {
        nRF24L01_spi_send(*data++);
    }
    nRF24L01_CS_RESET;
}