#include "vcmessaging.h"

#define VCTAG_HEADER(x) ((struct vctag_header*) x)
#define VCTAG_TAG(x) ((struct vctag_tag*) x)
//#define VCTAG_NEXT(tag,length) tag=((struct vctag_tag) (((u32) tag)+length))
#define VCTAG_NEXT(tag,length) tag=((struct vctag_tag*) (((u32) tag)+length+12)) // including vctag_tag header

#define ALIGN(buf,bits) ((((unsigned int) buf+(1<<bits))>>bits)<<bits)

u32 *vc_maketag_set_emmc_clock(void *tags,unsigned int emmcClk)
{
	struct vctag_tag *current = tags;
	struct vctag_header *header = VCTAG_HEADER(current);
	u32 *result;
	
	current = (struct vctag_tag*) (((u32) header)+sizeof(struct vctag_header));
	current->id = ARM2VC_Tag_SetClockRate;
	current->size=8;
	current->lengthDirection=VC_DIR_REQUEST;
	current->buffer[0]=VC_CLK_EMMC;
	current->buffer[1]=emmcClk;
	result = current->buffer;
	VCTAG_NEXT(current,8);
	current->id = ARM2VC_Tag_End;
	current->size = 0;
	current->lengthDirection=VC_DIR_REQUEST;
	VCTAG_NEXT(current,0);
	header->size = ((u32) current) - ((u32) header);
	header->direction = VC_REQUEST;
	
	return result;
}

u32 *vc_maketag_get_ram(void *tags)
{
	struct vctag_tag *current = tags;
	struct vctag_header *header = VCTAG_HEADER(current);
	u32 *result;

	current = (struct vctag_tag*) (((u32) header)+sizeof(struct vctag_header));
	current->id = ARM2VC_Tag_GetARMMemory;
	current->size=8;
	current->lengthDirection=VC_DIR_REQUEST;
	result = current->buffer;
	current->buffer[0]=0;
	current->buffer[1]=0;
	VCTAG_NEXT(current,8);
	current->id = ARM2VC_Tag_End;
	current->size = 0;
	current->lengthDirection=VC_DIR_REQUEST;
	VCTAG_NEXT(current,0);
	header->size = ((u32) current) - ((u32) header);
	header->direction = VC_REQUEST;
	
	return result;
}


OSERROR *send_vc_request(struct vctag_header *bufAlign)
{
  _kernel_oserror *err;
  int output=0;
  int channel = MB_Chan_ARM2VC;
  PhysicalAddress bufferPhys=0;
  LogicalAddress iobaseLogical;
  
  // convert the buffer to a physical address
  ERROR_CHECK(err, riscos_log2phys(bufAlign,&bufferPhys));
  
  // work out where the IO space lives in logical address space
  ERROR_CHECK(err, riscos_readIObase(&iobaseLogical));
//	printf("channel = %d, IO base log = %x, parameters phys = %p\n",channel, iobaseLogical, bufferPhys);
   
  // send the message to the mailbox
  output=HAL_SendHostMessage(channel,bufferPhys,iobaseLogical);
/*  printf("Responded %x, tags=%x\n",output,bufAlign);
    sprintf(printbuf,"memory %x+100",bufAlign);
    _swix(OS_CLI,_IN(0),printbuf);*/
  
  return NULL;
}

OSERROR *set_emmc_clock(unsigned int emmcClock)
{
  char buf[1024];
  
  // align our buffer to 256 byte boundary
  struct vctag_header * bufAlign= (struct vctag_header *) ALIGN(buf,8);
  
  // create some mailbox tags to say to change the EMMC clock
  vc_maketag_set_emmc_clock(bufAlign, emmcClock);
  
  return send_vc_request(bufAlign);
}

OSERROR *get_ram(PhysicalAddress *ramBase,unsigned int *ramSize)
{
  char buf[1024];
  u32 *output;
  OSERROR *result;
  
  // align our buffer to 256 byte boundary
  struct vctag_header * bufAlign= (struct vctag_header *) ALIGN(buf,8);
  
  output=vc_maketag_get_ram(bufAlign);
  
  result=send_vc_request(bufAlign);
  
  *ramBase = output[0];
  *ramSize = output[1];
  
  return result;
}

