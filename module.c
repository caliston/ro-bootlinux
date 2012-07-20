#include "kernel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linuxboot.h"

static void *module_globalPrivateWord=NULL;


OSERROR *module_swi(int swiNumber, _kernel_swi_regs *r, void *privateWord)
{
  OSERROR *result=NULL;
  UNUSED(privateWord);
  UNUSED(swiNumber);
  UNUSED(r);

  switch(swiNumber)
  {
	case LinuxBoot_Go:
		printf("Go!\n");
		break;
//    case CryptRandom_AddNoise:
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

