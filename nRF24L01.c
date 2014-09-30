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
    nRF24L01_CE_SET;

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
    gpio.GPIO_PuPd = GPIO_PuPd_DOWN;
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
    SPI_I2S_SendData(SPI1,data);
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);  // ждём пока данные уйдут    // жде пока данные придут
    
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);  // ждём пока данные появтся
    return SPI1->DR;
}


u8 nRF24L01_cmd_send(u8 cmd)
{
    u8 resp; 
    
    resp = nRF24L01_spi_send(cmd);
    return resp;
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
    u8 data = 0; // some configurations  (1<<PWR_UP)|(1<<EN_CRC)|(0<<PRIM_RX)
    u8 resp;
    resp = nRF24L01_write_reg(nRF24L01_CONFIG_REG, data);
    return resp;
}

u8 nRF24L01_configure_rx()
{
    u8 data = 0; // some configurations (1<<PWR_UP)|(1<<EN_CRC)|(0<<PRIM_RX)
    u8 resp;
    resp = nRF24L01_write_reg(nRF24L01_CONFIG_REG, data);
    return resp;
}

/*u8 nRF24L01_send_byte(u8 data)
{
    u8 resp;
    nRF24L01_cmd_send(nRF24L01_W_TX_PAYLOAD);
    resp = nRF24L01_spi_send(data);
    
    return resp;
}*/