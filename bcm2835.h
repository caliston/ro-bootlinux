#define PHYS_IOBASE	0x20000000

#define DMA0_BASE	PHYS_IOBASE + 0x7000
#define DMA15_BASE	PHYS_IOBASE + 0xE05000
#define DMA_BASE_OFFSET	0x100
#define DMA_BASE(x)	((x==15) ? DMA15_BASE : DMA0_BASE+(DMA_BASE_OFFSET*x))


#define DMA_CHANNELS	16

#define DMA_REG_CS	0x0
#define DMA_REG_CONBLK_AD	0x4
#define DMA_REG_TI		0x8
#define DMA_REG_SOURCE_AD	0xC
#define DMA_REG_DEST_AD		0x10
#define DMA_REG_TXFR_LEN	0x14
#define DMA_REG_STRIDE		0x18
#define DMA_REG_NExTCONBLK	0x1C
#define DMA_REG_DEBUG		0x20

#define DMA_CS_RESET	(1<<31)

/* reset all the DMA channels */
extern void dma_reset(void);
