#include "kernel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "swis.h"

#include "types.h"
#include "bcm2835.h"
#include "riscos.h"

OSERROR *riscos_log2phys(void *logical, PhysicalAddress *physical)
{
	OSERROR *err;
	PageBlock page;
  
	page.logical=logical;
	if ((err=_swix(OS_Memory,_INR(0,2),0 | OSMEMORY_SUPPLY_LOGICAL_ADDR | OSMEMORY_WANT_PHYSICAL_ADDR,
		&page,1)))
		return err;
	printf("Physical address = %x, logical address = %p\n",page.physical,page.logical);
	*physical = page.physical;
	return NULL;
}

OSERROR *riscos_readIObase(void **logical)
{
	OSERROR *err;
  
  if ((err=_swix(OS_Memory,_INR(0,2)|_OUT(3),13,PHYS_IOBASE,IO_SIZE,
        logical)))
      return err;
  printf("IO base logical address = %p\n",*logical);
  return NULL;
}