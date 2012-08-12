#include "bcm2835.h"

void dma_reset(void)
{
  int channel=0;
  
  for (channel=0; channel<DMA_CHANNELS; channel++)
  {
    // point to the control register of this DMA block
    volatile unsigned int *control_reg = (unsigned int*) (DMA_BASE(channel) + DMA_REG_CS);
    // reset the DMA block (don't care about the other bits)
    *control_reg = DMA_CS_RESET;
  }
}
