#include "kernel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "swis.h"

#include "linuxboot.h"

static void *module_globalPrivateWord=NULL;

typedef unsigned int PhysicalAddr;
typedef unsigned int PageNumber;

#define KERNEL_SIZE	7*1024*1024
#define LOG2_KERNEL_ALIGNMENT	12
//#define KERNEL_FILENAME	"SDFS:$.!Boot.Loader.kernel/img"
#define KERNEL_FILENAME	"SDFS:$.blinker01/bin"

#define OSMEMORY_SUPPLY_PHYSICAL_PAGENO	(1<<8)
#define OSMEMORY_SUPPLY_LOGICAL_ADDR	(1<<9)
#define OSMEMORY_SUPPLY_PHYSICAL_ADDR	(1<<10)
#define OSMEMORY_WANT_PHYSICAL_PAGENO	(1<<11)
#define OSMEMORY_WANT_LOGICAL_ADDR	(1<<12)
#define OSMEMORY_WANT_PHYSICAL_ADDR	(1<<13)
#define OSMEMORY_DISABLE_CACHING	(2<<14)
#define OSMEMORY_ENABLE_CACHING		(3<<14)

#define MACHID_BCM2708	3072	// from ARM Linux registry

void jump_to_linux(int kernel_size, int machine_type, PhysicalAddr atags, PhysicalAddr kernel);

//#define PHYS_TO_UNCACHED_PHYS(x) ((x & 0x1FFFFFFF) | ( 0xC0000000))

typedef struct {
	PageNumber	number;
	void *		logical;
	PhysicalAddr	physical;
} PageBlock;

void unlock_free_pool(void)
{
	_swix(OS_Memory,_INR(0,1),10,0); // free pool unlock
}

void linuxboot(int doboot)
{
	PageNumber page_number=0;
	PageBlock pageInfo;
	int kernel_length=0;
	void *logical_addr=0;
	char buf[256];
	PhysicalAddr physical_addr=0; // no point dereferencing a pointer to physical address space
	
	_swi(OS_Memory,_INR(0,1),10,1); // free pool lock
	
	_swi(OS_Memory,_INR(0,2)|_OUT(3),12,KERNEL_SIZE,LOG2_KERNEL_ALIGNMENT,&page_number);
	printf("%d bytes contiguous starting at page %d\n",KERNEL_SIZE,page_number);
	pageInfo.number = page_number;
	_swi(OS_Memory,_INR(0,2),0 | OSMEMORY_SUPPLY_PHYSICAL_PAGENO | OSMEMORY_WANT_LOGICAL_ADDR | OSMEMORY_WANT_PHYSICAL_ADDR,
		&pageInfo,1);
	printf("Physical page %d at phys addr %x logical addr %p\n",pageInfo.number,pageInfo.physical,pageInfo.logical);
	logical_addr = pageInfo.logical;
	physical_addr = pageInfo.physical;
#if 0
	pageInfo.physical = 0;
	_swi(OS_Memory,_INR(0,2),0 | OSMEMORY_SUPPLY_PHYSICAL_ADDR | OSMEMORY_WANT_LOGICAL_ADDR | OSMEMORY_WANT_PHYSICAL_PAGENO,
		&pageInfo,1);
	printf("Physical page %d at phys addr %x logical addr %p\n",pageInfo.number,pageInfo.physical,pageInfo.logical);

	pageInfo.physical = 0x1000;
	_swi(OS_Memory,_INR(0,2),0 | OSMEMORY_SUPPLY_PHYSICAL_ADDR | OSMEMORY_WANT_LOGICAL_ADDR | OSMEMORY_WANT_PHYSICAL_PAGENO,
		&pageInfo,1);
	printf("Physical page %d at phys addr %x logical addr %p\n",pageInfo.number,pageInfo.physical,pageInfo.logical);
#endif

	_swi(OS_File,_INR(0,3)|_OUT(4),16,KERNEL_FILENAME,logical_addr,0,&kernel_length);
	printf("Loaded kernel into logical addr %p, length %d\n",logical_addr,kernel_length);
	//snprintf(buf,256,"*memoryi %x",logical_addr);
	//_swix(OS_CLI,buf);
	if (doboot)
		jump_to_linux(kernel_length, MACHID_BCM2708, 0x100, physical_addr);
	
	_swix(OS_Memory,_INR(0,1),10,0); // free pool unlock
}

OSERROR *module_swi(int swiNumber, _kernel_swi_regs *r, void *privateWord)
{
  OSERROR *result=NULL;
  UNUSED(privateWord);
  UNUSED(swiNumber);
  UNUSED(r);

  switch(swiNumber)
  {
	case LinuxBoot_Go:
		linuxboot(r->r[0]);
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

