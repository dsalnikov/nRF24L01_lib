#include <stm32f4xx.h>
#include "nRF24L01.h"

void nRF24L01_init()
{
    nRF24L01_spi_init();
}

void nRF24L01_spi_init()
{
    // gpio init
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;    // разрешаем тактирование порта C
    
    // тактирование spi3
    RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
    
    //SPI3_MOSI
    GPIOC->MODER    |= GPIO_MODER_MODER12_1;        //Alternate function mode
    GPIOC->OTYPER   &= ~GPIO_OTYPER_OT_12;          //Output push-pull (reset state)
    GPIOC->OSPEEDR  |= GPIO_OSPEEDER_OSPEEDR12_0;   //Medium speed
    GPIOC->AFR[1]   |= (6)<<((12 - 8)*4);           //AF6

    //SPI3 MISO
    GPIOC->MODER    |= GPIO_MODER_MODER11_1;        //Alternate function mode
    GPIOC->OTYPER   &= ~GPIO_OTYPER_OT_11;          //Output push-pull (reset state)
    GPIOC->OSPEEDR  |= GPIO_OSPEEDER_OSPEEDR11_0;   //Medium speed
    GPIOC->AFR[1]   |= (6)<<((11 - 8)*4);           //AF6

    //SPI3 SCK
    GPIOC->MODER    |= GPIO_MODER_MODER10_1;        //Alternate function mode
    GPIOC->OTYPER   &= ~GPIO_OTYPER_OT_10;          //Output push-pull (reset state)
    GPIOC->OSPEEDR  |= GPIO_OSPEEDER_OSPEEDR10_0;   //Medium speed 
    GPIOC->AFR[1]   |= (6)<<((10 - 8)*4);           //AF6
    
    //SPI3 CS
    GPIOC->MODER |= GPIO_MODER_MODER9_0;        //выход
    GPIOC->OTYPER &= ~GPIO_OTYPER_OT_9;         
    GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9_0; //Medium speed 
            
    // spi 3
    // master 8bit
    // crc
    // делитель 256
    // программный CS
    SPI3->CR1 = SPI_CR1_CRCEN | SPI_CR1_SPE | SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0 | SPI_CR1_MSTR | SPI_CR1_SSM;
    
    // CRC: x^8 + x^2 + x + 1
    SPI3->CRCPR = 0x07; 
}

u8 spi_send(u8 data)
{
    nRF24L01_CS_SET;
    
    SPI2->DR = data;
    // ждем отправки данных
    while(SPI2->SR & SPI_SR_TXE);
    // жде пока данные придут
    while(SPI2->SR ^ SPI_SR_RXNE);
    
    nRF24L01_CS_RESET;
    
    return SPI2->DR;
}