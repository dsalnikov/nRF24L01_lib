#include <stm32f4xx.h>

#include "nRF24L01.h"

#define RX 1

void main(void){
  // test commit
  nRF24L01_init();
  u8 resp[5], r;
  u8 data[5] = {0x10,0x20,0x30,0x40,0x50};
  u32 i = 0;
  nRF24L01_STATUS_REGISTER status_reg;

#ifdef RX
  nRF24L01_configure_rx();
#else
  nRF24L01_configure_tx();
#endif
  
  while (1)  {
      for(i=0xffff;i>0;i--);
      for(i=0xffff;i>0;i--);

      status_reg = nRF24L01_readStatus();
#ifdef RX
      if (status_reg.bit.RX_DR == 1)
      {
          r = nRF24L01_readRx(resp);
      }
#else
      r = nRF24L01_writeTx(data);
#endif
  
  }
}
