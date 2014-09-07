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
  
  nRF24L01_init();
  
  setup_leds();   // инициализация

  while (1)  {
      if (GPIOA->IDR & GPIO_IDR_IDR_0) {   // если кнопка нажата (на нулевом пине есть потенциал)
 
          LED_PORT->ODR = LED_BLUE | LED_ORANGE | LED_RED | LED_GREEN;  // включаем диоды
 
      } else {
            switch_leds_off();   //гасим
 
      }
  }
}
