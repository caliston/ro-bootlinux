#include "types.h"
#include "riscos.h"

// mailbox channel numbers
#define MB_Chan_Pwr          0             // Power channel
#define MB_Chan_FB           1             // Frame Buffer channel
#define MB_Chan_VUart        2             // Virtual UART channel
#define MB_Chan_VCHIQ        3             // VCHIQ channel
#define MB_Chan_LEDS         4             // LEDS channel
#define MB_Chan_Btn          5             // Buttons channel
#define MB_Chan_TSc          6             // TouchScreen channel
#define MB_Chan_ARM2VC       8             // ARM -> VC property channel
#define MB_Chan_VC2ARM       9             // VC -> ARM property channel


#define ARM2VC_Tag_End                   0x00000000 // End of tag list
#define ARM2VC_Tag_GetFirmwareVersion    0x00000001 // return 4 byte firmware version
#define ARM2VC_Tag_GetBoardModel         0x00010001 // return 4 byte model
#define ARM2VC_Tag_GetBoardRevision      0x00010002 // return 4 byte revision
#define ARM2VC_Tag_GetBoardMAC           0x00010003 // return 6 byte MAC
#define ARM2VC_Tag_GetBoardSerial        0x00010004 // return 8 byte serial
#define ARM2VC_Tag_GetARMMemory          0x00010005 // return ARM mem base + size
#define ARM2VC_Tag_GetVCMemory           0x00010006 // return VC mem base + size
#define ARM2VC_Tag_GetClocks             0x00010007 // return clock tree
#define ARM2VC_Tag_GetConfig             0x00050001 // get kernel boot args string
#define ARM2VC_Tag_GetDMAChannels        0x00060001 // get mask of DMA channels usable by ARM
#define ARM2VC_Tag_GetPowerState         0x00020001 // get power state of specified MB_Pwr_ device
#define ARM2VC_Tag_GetPowerTiming        0x00020002 // get microsecond delay required after power on for indicated device
#define ARM2VC_Tag_SetPowerState         0x00028001 // set power state of device
#define ARM2VC_Tag_GetClockState         0x00030001 // get power state of clock
#define ARM2VC_Tag_SetClockState         0x00038001 // set power state of clock
#define ARM2VC_Tag_GetClockRate          0x00030002 // get rate of clock
#define ARM2VC_Tag_SetClockRate          0x00038002 // set rate of clock
#define ARM2VC_Tag_FBAlloc               0x00040001 // alloc framebuffer at given alignment
#define ARM2VC_Tag_FBRelease             0x00048001 // release framebuffer
#define ARM2VC_Tag_FBBlank               0x00040002 // blank screen
#define ARM2VC_Tag_FBGetPhysDimension    0x00040003 // get physical display width/height
#define ARM2VC_Tag_FBTestPhysDimension   0x00044003 // test physical display w/h
#define ARM2VC_Tag_FBSetPhysDimension    0x00048003 // set physical display w/h
#define ARM2VC_Tag_FBGetVirtDimension    0x00040004 // get virtual display w/h
#define ARM2VC_Tag_FBTestVirtDimension   0x00044004 // test virtual display w/h
#define ARM2VC_Tag_FBSetVirtDimension    0x00048004 // set virtual display w/h
#define ARM2VC_Tag_FBGetDepth            0x00040005 // get display BPP
#define ARM2VC_Tag_FBTestDepth           0x00044005 // test display BPP
#define ARM2VC_Tag_FBSetDepth            0x00048005 // set display BPP
#define ARM2VC_Tag_FBGetPixelOrder       0x00040006 // get RGB/BGR order
#define ARM2VC_Tag_FBTestPixelOrder      0x00044006 // test RGB/BGR order
#define ARM2VC_Tag_FBSetPixelOrder       0x00048006 // set RGB/BGR order
#define ARM2VC_Tag_FBGetAlphaMode        0x00040007 // get alpha mode
#define ARM2VC_Tag_FBTestAlphaMode       0x00044007 // test alpha mode
#define ARM2VC_Tag_FBSetAlphaMode        0x00048007 // set alpha mode
#define ARM2VC_Tag_FBGetPitch            0x00040008 // get row pitch
#define ARM2VC_Tag_FBGetVirtOffset       0x00040009 // get X/Y offset of virtual display
#define ARM2VC_Tag_FBTestVirtOffset      0x00044009 // test X/Y offset
#define ARM2VC_Tag_FBSetVirtOffset       0x00048009 // set X/Y offset
#define ARM2VC_Tag_FBGetOverscan         0x0004000a // get overscan values
#define ARM2VC_Tag_FBTestOverscan        0x0004400a // test overscan values
#define ARM2VC_Tag_FBSetOverscan         0x0004800a // set overscan values
#define ARM2VC_Tag_FBGetPalette          0x0004000b // get full palette table
#define ARM2VC_Tag_FBTestPalette         0x0004400b // test ranged update of table
#define ARM2VC_Tag_FBSetPalette          0x0004800b // do ranged update of table

#define VC_REQUEST		0x0
#define VC_RESPONSE_OK		0x80000000
#define VC_RESPONSE_ERROR	0x80000001

#define VC_DIR_REQUEST		(0<<31)
#define VC_DIR_RESPONSE		(1<<31)

#define VC_CLK_RESERVED		0x000000000
#define VC_CLK_EMMC		0x000000001
#define VC_CLK_UART		0x000000002
#define VC_CLK_ARM		0x000000003
#define	VC_CLK_CORE		0x000000004
#define VC_CLK_V3D		0x000000005
#define VC_CLK_H264		0x000000006
#define VC_CLK_ISP		0x000000007


struct vctag_header {
	u32 size;	// buffer size including header
	u32 direction;	// request or response
};

struct vctag_tag {
	u32 id;		// tag identifier
	u32 size;	// buffer size
	u32 lengthDirection;  // value length (LSBs 31), direction (MSB)
	u32 buffer[1];	// one or more bytes of buffer
};

#define vctag_next(t)     ((struct vctag_tag *)((u32 *)(t) + (t)->hdr.size))


extern int HAL_SendHostMessage(int channel, void*tagBuffer, void *iobase);
extern OSERROR *set_emmc_clock(unsigned int emmcClock);
