#include <stm32f4xx.h>

#include "nRF24L01.h"

void main(void){
  // test commit
  nRF24L01_init();
  u8 resp[5], r;
  
  r = nRF24L01_read_reg(nRF24L01_STATUS_REG,resp,0);
  r = nRF24L01_read_reg(nRF24L01_CONFIG_REG,resp,1);
  r = nRF24L01_read_reg(nRF24L01_TX_ADDR_REG,resp,5);

  nRF24L01_write_reg(nRF24L01_CONFIG_REG, 2);
  r = nRF24L01_read_reg(nRF24L01_CONFIG_REG,resp,1);
  
  u32 i;
    
  nRF24L01_configure_rx();
  
  while (1)  {
      for(i=0xffff;i>0;i--);
      for(i=0xffff;i>0;i--);
      r = nRF24L01_read_reg(nRF24L01_RX_ADDR_P2_REG,resp,1);
  }
}
