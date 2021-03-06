#include "linuxboot.h"
#include "types.h"
#include "riscos.h"
#include "linux.h"
#include "vcmessaging.h"

static void *module_globalPrivateWord=NULL;

#define KERNEL_FILENAME	"SDFS:$.!Boot.Loader.kernel/img"
//#define KERNEL_FILENAME	"SDFS:$.blinker01/bin"
#define INITRD_FILENAME "none"
#define CMDLINE "not passed yet"
#define ZIMAGE_MODE	0	// doesn't work

OSERROR *module_swi(int swiNumber, _kernel_swi_regs *r, void *privateWord)
{
  OSERROR *result=NULL;
  UNUSED(privateWord);
  UNUSED(swiNumber);
  UNUSED(r);
  int output;
  

  switch(swiNumber)
  {
	case BootLinux_Boot:
		start_linux(r->r[1] /*KERNEL_FILENAME*/,INITRD_FILENAME,r->r[2] /*CMDLINE*/,r->r[0],ZIMAGE_MODE);
		printf("Phew!\n");
		break;
        case BootLinux_Messaging:
                //result=test_message(r);
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

