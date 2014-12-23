#include <stm32f4xx.h>
#include <stm32f4_discovery.h>

#include "nRF24L01.h"

#include <stdio.h>

// счетчик 
static __IO uint32_t TimingDelay;
 
// прототипы
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);

void BInit();

nRF24L01_STATUS_REGISTER status_reg;
nRF24L01_CONFIG_REGISTER config_reg;

/* My address */
uint8_t MyAddress[] = {
	0xE7,
	0xE7,
	0xE7,
	0xE7,
	0xE7
};
/* Receiver address */
uint8_t TxAddress[] = {
	0x7E,
	0x7E,
	0x7E,
	0x7E,
	0x7E
};

void main(void){
    SystemInit();
    BInit();
    
    
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
    

    u8 str[] = "Hello blablacode.ru by nRF24\n";
    //u8 str[] = {0x55,0xAA};
    
    nRF24L01_init();

    nRF24L01_set_rf(nRF24L01_DataRate_2M, nRF24L01_OutputPower_M18dBm);
   
    
    nRF24L01_set_my_addr(MyAddress);
    nRF24L01_set_tx_addr(TxAddress);
    
    u8 dataOut[32];
    sprintf((char *)dataOut, "abcdefghijklmnoszxABCDEFCBDA");
     
    
    while (1)  {
        status_reg = nRF24L01_readStatus();
        STM_EVAL_LEDToggle(LED3);
        nRF24L01_writeTx(str);
        do {
            status_reg = nRF24L01_readStatus();
        } while (status_reg.bit.MAX_RT == 0 && status_reg.bit.TX_DS == 0);
        
        nRF24L01_ClearStatus();
        status_reg = nRF24L01_readStatus();
        Delay(100);
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

void BInit() {
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
      GPIO_InitTypeDef gpio_b;   /// структура для инициализации кнопки
      GPIO_StructInit(&gpio_b);  // заполняем структуру стандартными параметрами
      gpio_b.GPIO_Mode = GPIO_Mode_IN;   // это кнопка - порт работает как вход
      gpio_b.GPIO_Pin = GPIO_Pin_1;
 
      GPIO_Init(GPIOB, &gpio_b);   // инициализируем порт B
}
