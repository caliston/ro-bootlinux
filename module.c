#include "kernel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "swis.h"

#include "linuxboot.h"
#include "types.h"
#include "linux.h"

static void *module_globalPrivateWord=NULL;

#define KERNEL_FILENAME	"SDFS:$.!Boot.Loader.kernel/img"
//#define KERNEL_FILENAME	"SDFS:$.blinker01/bin"
#define INITRD_FILENAME "none"
#define CMDLINE "not passed yet"
#define ZIMAGE_MODE	0	// doesn't work
#define IO_BASE 0x20000000
#define IO_SIZE 0x01000000

extern int HAL_SendHostMessage(int channel, void*tagBuffer, void *iobase);

OSERROR *test_message(_kernel_swi_regs *r)
{
  _kernel_oserror *err;
  int output;
  PageBlock io;
  void *ioLogical;
  
  io.physical=IO_BASE;
  if ((err=_swix(OS_Memory,_INR(0,2)|_OUT(3),13,IO_BASE,IO_SIZE,
        &ioLogical)))
      return err;
  printf("IO base logical address = %p\n",ioLogical);
   
  output=HAL_SendHostMessage(r->r[0],r->r[1],r->r[2]);
  printf("Responded %x\n");
  return NULL;
}

OSERROR *module_swi(int swiNumber, _kernel_swi_regs *r, void *privateWord)
{
  OSERROR *result=NULL;
  UNUSED(privateWord);
  UNUSED(swiNumber);
  UNUSED(r);
  int output;
  

  switch(swiNumber)
  {
	case LinuxBoot_Go:
		start_linux(KERNEL_FILENAME,INITRD_FILENAME,CMDLINE,r->r[0],ZIMAGE_MODE);
		printf("Phew!\n");
		break;
        case LinuxBoot_Messaging:
                result=test_message(r);
                break;
	default:
		break;
  }

  return result;
}

OSERROR *module_cmd(char *argString, int argc, int commandNumber, void *privateWord)
{
  UNUSED(privateWord);
  UNUSED(argString);
  UNUSED(argc);
  UNUSED(commandNumber);

  start_linux(argString,"",CMDLINE,commandNumber,ZIMAGE_MODE);
  return 0;
}

#if 0
void module_service(int serviceNumber, _kernel_swi_regs *r, void *privateWord)
{
  UNUSED(serviceNumber);
  UNUSED(r);
  UNUSED(privateWord);

  /* Only interested in Service_ShutDown */

  xsyslog_irq_mode(TRUE);
  noise_save_seed();

  xsyslog_irq_mode(FALSE);
}
#endif

void module_finalise(void)
{
}

OSERROR *module_initialise(char *commandTail, int poduleBase, void *privateWord)
{
  OSERROR *result;

  UNUSED(commandTail);
  UNUSED(poduleBase);
  UNUSED(privateWord);

  module_globalPrivateWord=privateWord;

  /* if everything here went OK, we can attach the normal finalisation
   * code - don't do this before here, as we might try to finalise
   * something that hasn't been initialised
   */
  atexit(module_finalise);

  return 0;
}

