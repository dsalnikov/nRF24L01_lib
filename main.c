#include <stm32f4xx.h>

#include "nRF24L01.h"

#define LED_PORT GPIOD

#define LED_GREEN (1 << 12) /* port D, pin 12 */
#define LED_ORANGE (1 << 13) /* port D, pin 13 */
#define LED_RED (1 << 14) /* port D, pin 14 */
#define LED_BLUE (1 << 15) /* port D, pin 15 */

static inline void setup_leds(void){
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;    // разрешаем тактирование порта D (диоды)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;   // и порта A (кнопка)
 
    GPIOA->MODER &= ~GPIO_MODER_MODER0;  // укажем что пин кнопки - вход
    LED_PORT->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 |
        GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0;    /// на диоды - выход
}

static inline void switch_leds_off(void){
    LED_PORT->ODR = 0;
}

void main(void){
  // test commit
  nRF24L01_init();
  u8 resp[5], r;
  
  r = nRF24L01_read_reg(nRF24L01_STATUS_REG,resp,0);
  r = nRF24L01_read_reg(nRF24L01_CONFIG_REG,resp,1);
  r = nRF24L01_read_reg(nRF24L01_TX_ADDR_REG,resp,5);

  nRF24L01_write_reg(nRF24L01_CONFIG_REG, 2);
  r = nRF24L01_read_reg(nRF24L01_CONFIG_REG,resp,1);
  
  setup_leds();   // инициализация
    u32 i;
  while (1)  {
      for(i=0xffff;i>0;i--);
      for(i=0xffff;i>0;i--);
      r = nRF24L01_read_reg(nRF24L01_RX_ADDR_P2_REG,resp,1);
  }
}
