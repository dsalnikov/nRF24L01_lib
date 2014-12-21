#include <stm32f4xx.h>
#include <stm32f4_discovery.h>

#include "nRF24L01.h"

// счетчик 
static __IO uint32_t TimingDelay;
 
// прототипы
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);

void IRQInit();
void it_init();

nRF24L01_STATUS_REGISTER status_reg;
nRF24L01_CONFIG_REGISTER config_reg;

volatile u8 ptx = 0;

void main(void){
    SystemInit();
    IRQInit();
    
    // 0.001 с = 1/1000 с = 1мс
    if (SysTick_Config(SystemCoreClock / 1000)){ 
        /* если вернулся не ноль - ошибка */
        while (1);
    }
    
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED4);
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED6);
    
    Delay(100); 

    // test commit
    nRF24L01_init();
    u8 resp[5], r;
    u8 data[5] = {0x10,0x20,0x30,0x40,0x50};
    u32 i = 0;
    
    u8 str[] = "Hello blablacode.ru by nRF24";

    u8 addr[] = {0x7E,0x7E,0x7E,0x7E,0x7E};

    nRF24L01_read_regm(nRF24L01_RX_ADDR_P0_REG,resp,sizeof(resp));
    nRF24L01_write_regm(nRF24L01_RX_ADDR_P0_REG,addr,sizeof(addr));
    nRF24L01_read_regm(nRF24L01_RX_ADDR_P0_REG,resp,sizeof(resp));
    
    nRF24L01_ClearStatus();
    Delay(100);
    
    /*
    
    nRF24L01_write_regm(nRF24L01_RX_PW_P1_REG,addr,sizeof(addr));
    Delay(100);
    u8 addr2[] = {0xE7,0xE7,0xE7,0xE7,0xE7};
    
    nRF24L01_write_regm(nRF24L01_RX_PW_P0_REG,addr2,sizeof(addr2));
    Delay(100);
    nRF24L01_write_regm(nRF24L01_TX_ADDR_REG,addr2,sizeof(addr2));
    Delay(100);
    */
    nRF24L01_CS_SET;
    
    r = nRF24L01_spi_send(nRF24L01_W_REGISTER | nRF24L01_RX_PW_P1_REG);
    r = nRF24L01_spi_send(0x7E);
    r = nRF24L01_spi_send(0x7E);
    r = nRF24L01_spi_send(0x7E);
    r = nRF24L01_spi_send(0x7E);
    r = nRF24L01_spi_send(0x7E);
    
    nRF24L01_CS_RESET;
    
    Delay(100);
    
    nRF24L01_CS_SET;
    
    r = nRF24L01_spi_send(nRF24L01_W_REGISTER | nRF24L01_RX_PW_P0_REG);
    r = nRF24L01_spi_send(0xE7);
    r = nRF24L01_spi_send(0xE7);
    r = nRF24L01_spi_send(0xE7);
    r = nRF24L01_spi_send(0xE7);
    r = nRF24L01_spi_send(0xE7);
    
    nRF24L01_CS_RESET;
    
    Delay(100);
    
    nRF24L01_CS_SET;
    
    r = nRF24L01_spi_send(nRF24L01_W_REGISTER | nRF24L01_TX_ADDR_REG);
    r = nRF24L01_spi_send(0xE7);
    r = nRF24L01_spi_send(0xE7);
    r = nRF24L01_spi_send(0xE7);
    r = nRF24L01_spi_send(0xE7);
    r = nRF24L01_spi_send(0xE7);
    
    nRF24L01_CS_RESET;
 
    
    Delay(100);

    r = nRF24L01_write_reg(nRF24L01_RX_PW_P0_REG, 1);
    Delay(100);
    
    r = nRF24L01_write_reg(nRF24L01_RX_PW_P1_REG, 1);
    Delay(100);
    
    while (1)  {
        if ((GPIOB->IDR & GPIO_IDR_IDR_1) == 0) 
        {
            // irq
            STM_EVAL_LEDToggle(LED4);
            status_reg = nRF24L01_readStatus();
            
            nRF24L01_ClearStatus();
            
            if (status_reg.bit.TX_DS == 1)
            {
                //успешная передача
                STM_EVAL_LEDToggle(LED5);
            }
            
            if (status_reg.bit.TX_FULL == 1)
            {
                // чистим буфер
                nRF24L01_CS_SET;
                nRF24L01_spi_send(nRF24L01_FLUSH_TX);
                nRF24L01_CS_RESET;
            }
        }
        nRF24L01_writeTx(str);

        Delay(100); 
    }
}

void EXTI0_IRQHandler()
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        if (ptx == 0)
        {
            EXTI_ClearITPendingBit(EXTI_Line0);
            return;
        }
        
        status_reg = nRF24L01_readStatus();
                
        ptx = 0;
        STM_EVAL_LEDToggle(LED3);
        if (status_reg.bit.MAX_RT == 1)
        {
            nRF24L01_CS_SET;
            nRF24L01_spi_send(nRF24L01_FLUSH_TX);
            nRF24L01_CS_RESET;
        }
        nRF24L01_ClearStatus();
        EXTI_ClearITPendingBit(EXTI_Line0);  // сбрасываем флаг прерывания
    }
}

/* функция задержки */
void Delay(__IO uint32_t nTime){ 
  TimingDelay = nTime;
  while(TimingDelay != 0); // безконченый цикл пока значение счетчика не нулевое
}
 
/* декремент значения счетчика задержки, если он не равен нулю */
void TimingDelay_Decrement(void){
  if (TimingDelay != 0x00){ 
    TimingDelay--;
  }
}

void IRQInit() {
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
      GPIO_InitTypeDef gpio_b;   /// структура для инициализации кнопки
      GPIO_StructInit(&gpio_b);  // заполняем структуру стандартными параметрами
      gpio_b.GPIO_Mode = GPIO_Mode_IN;   // это кнопка - порт работает как вход
      gpio_b.GPIO_Pin = GPIO_Pin_1;
 
      GPIO_Init(GPIOB, &gpio_b);   // инициализируем порт B
      
      //it_init();
}

void it_init() {
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);
    EXTI_InitTypeDef exti;  // структура для настроки
    exti.EXTI_Line = EXTI_Line0;    /// кнопка на линии 0
    exti.EXTI_Mode = EXTI_Mode_Interrupt;    // прерывание (а не событие)
    exti.EXTI_Trigger = EXTI_Trigger_Falling;  // срабатываем по переднему фронту импульса
    exti.EXTI_LineCmd = ENABLE;    /// вкл
 
    EXTI_Init(&exti);
 
    NVIC_InitTypeDef nvic;
    nvic.NVIC_IRQChannel = EXTI0_IRQn;  // указываем канал IRQ
    nvic.NVIC_IRQChannelPreemptionPriority = 0;  // приоритет канала ( 0 (самый приоритетный) - 15)
    nvic.NVIC_IRQChannelSubPriority = 1;  // приоритет подгруппы
    nvic.NVIC_IRQChannelCmd = ENABLE;
 
    NVIC_Init(&nvic);
}
