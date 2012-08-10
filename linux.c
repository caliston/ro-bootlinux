#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <kernel.h>
#include <swis.h>
#include "atags.h"

#define tag_next(t)     ((struct atag *)((u32 *)(t) + (t)->hdr.size))
#define tag_size(type)  ((sizeof(struct atag_header) + sizeof(struct type)) >> 2)
static struct atag *params; /* used to point at the current tag */

static void
setup_core_tag(void * address,long pagesize)
{
    params = (struct atag *)address;         /* Initialise parameters to start at given address */

    params->hdr.tag = ATAG_CORE;            /* start with the core tag */
    params->hdr.size = tag_size(atag_core); /* size the tag */

    params->u.core.flags = 1;               /* ensure read-only */
    params->u.core.pagesize = pagesize;     /* systems pagesize (4k) */
    params->u.core.rootdev = 0;             /* zero root device (typicaly overidden from commandline )*/

    params = tag_next(params);              /* move pointer to next tag */
}

static void
setup_ramdisk_tag(u32 size)
{
    params->hdr.tag = ATAG_RAMDISK;         /* Ramdisk tag */
    params->hdr.size = tag_size(atag_ramdisk);  /* size tag */

    params->u.ramdisk.flags = 0;            /* Load the ramdisk */
    params->u.ramdisk.size = size;          /* Decompressed ramdisk size */
    params->u.ramdisk.start = 0;            /* Unused */

    params = tag_next(params);              /* move pointer to next tag */
}

static void
setup_initrd2_tag(u32 start, u32 size)
{
    params->hdr.tag = ATAG_INITRD2;         /* Initrd2 tag */
    params->hdr.size = tag_size(atag_initrd2);  /* size tag */

    params->u.initrd2.start = start;        /* physical start */
    params->u.initrd2.size = size;          /* compressed ramdisk size */

    params = tag_next(params);              /* move pointer to next tag */
}

static void
setup_mem_tag(u32 start, u32 len)
{
    params->hdr.tag = ATAG_MEM;             /* Memory tag */
    params->hdr.size = tag_size(atag_mem);  /* size tag */

    params->u.mem.start = start;            /* Start of memory area (physical address) */
    params->u.mem.size = len;               /* Length of area */

    params = tag_next(params);              /* move pointer to next tag */
}

static void
setup_cmdline_tag(const char * line)
{
    int linelen = strlen(line);

    if(!linelen)
        return;                             /* do not insert a tag for an empty commandline */

    params->hdr.tag = ATAG_CMDLINE;         /* Commandline tag */
    params->hdr.size = (sizeof(struct atag_header) + linelen + 1 + 4) >> 2;

    strcpy(params->u.cmdline.cmdline,line); /* place commandline into tag */

    params = tag_next(params);              /* move pointer to next tag */
}

static void
setup_end_tag(void)
{
    params->hdr.tag = ATAG_NONE;            /* Empty tag ends list */
    params->hdr.size = 0;                   /* zero length */
}


#define DRAM_BASE 0x0
#define DRAM_SIZE 128*1024*1024
#define KERNEL_START DRAM_BASE + 0x8000
//#define INITRD_LOAD_ADDRESS DRAM_BASE + 0x800000
#define ATAGS_OFFSET	0x100

#define MACHTYPE_BCM2708 3072

#define CMDLINE "dma.dmachans=0x3c bcm2708_fb.fbwidth=656 bcm2708_fb.fbheight=416 bcm2708.boardrev=0x2 bcm2708.serial=0x3a8ad45d smsc95xx.macaddr=B8:27:EB:8A:D4:5D dwc_otg.lpm_enable=0 console=ttyAMA0,115200 kgdboc=ttyAMA0,115200 console=tty1 root=/dev/mmcblk0p1 rootfstype=ext4 rootwait"

#define OSMEMORY_SUPPLY_PHYSICAL_PAGENO        (1<<8)
#define OSMEMORY_SUPPLY_LOGICAL_ADDR   (1<<9)
#define OSMEMORY_SUPPLY_PHYSICAL_ADDR  (1<<10)
#define OSMEMORY_WANT_PHYSICAL_PAGENO  (1<<11)
#define OSMEMORY_WANT_LOGICAL_ADDR     (1<<12)
#define OSMEMORY_WANT_PHYSICAL_ADDR    (1<<13)
#define OSMEMORY_DISABLE_CACHING       (2<<14)
#define OSMEMORY_ENABLE_CACHING                (3<<14)

// load in on a page boundary
#define LOG2_KERNEL_ALIGNMENT	12

#define KERNEL_PAD	8*4096

PhysicalAddress jump_to_linux(int kernel_size, int machine_type, PhysicalAddress atags, PhysicalAddress kernel);



void
setup_tags(void *parameters)
{
    setup_core_tag(parameters, 4096);       /* standard core tag 4k pagesize */
    setup_mem_tag(DRAM_BASE, DRAM_SIZE);    /* 64Mb at 0x10000000 */
//    setup_ramdisk_tag(4096);                /* create 4Mb ramdisk */ 
//    setup_initrd2_tag(INITRD_LOAD_ADDRESS, 0x100000); /* 1Mb of compressed data placed 8Mb into memory */
    setup_cmdline_tag(CMDLINE);    /* commandline setting root device */
    setup_end_tag();                    /* end of tags */
}

/* allocate some contiguous physical RAM and load an image.
 * Note that currently this can't be called twice, because the RAM isn't reserved after being
 * requested, it's just trampled upon.  Therefore calling this twice will get the same memory
 * location returned
 */
 
static _kernel_oserror notfound = {0, "Kernel not found"};
 
_kernel_oserror *allocate_and_load_image(Image *i)
{
	_kernel_oserror *r;
	int type;
	
        if ((r=_swix(OS_File,_INR(0,1)|_OUT(0)|_OUT(4),5,i->name,&type,&i->length)))
		return r;
        printf("Found kernel %s, type %d, length %d\n",i->name,type,i->length);

        switch (type)
        {
            case 0:
                printf("Kernel %s not found\n",i->name);
                return &notfound;
            case 2:
                printf("Kernel %s is a directory\n",i->name);
                return &notfound;
        }
            

	if ((r=_swix(OS_Memory,_INR(0,2)|_OUT(3),12,i->length+KERNEL_PAD,LOG2_KERNEL_ALIGNMENT,&i->memory.number)))
		return r;
	printf("%d bytes contiguous starting at page %d\n",i->length,i->memory.number);

	if ((r=_swix(OS_Memory,_INR(0,2),0 | OSMEMORY_SUPPLY_PHYSICAL_PAGENO | OSMEMORY_WANT_LOGICAL_ADDR | OSMEMORY_WANT_PHYSICAL_ADDR,
		&i->memory,1)))
		return r;
	printf("Physical page %d at phys addr %x logical addr %p\n",i->memory.number,i->memory.physical,i->memory.logical);

        if ((r=_swix(OS_File,_INR(0,3),16,i->name,i->memory.logical,0)))
		return r;
        printf("Loaded kernel into logical addr %p, length %d\n",i->memory.logical,i->length);

	return NULL;
}



_kernel_oserror *start_linux(char *name, char *rdname, char *cmdline, int doBoot)
{
    Image kernel;
    _kernel_oserror *r;
    char buf[256];

    memset(&kernel,0,sizeof(Image));
    kernel.name = name;

	// free pool lock
	if ((r=_swix(OS_Memory,_INR(0,1),10,1)))
		return r;

    if ((r=allocate_and_load_image(&kernel)))
	return r;

	// can't load an initrd due to shortcomings in the above call

    setup_tags(kernel.memory.logical + ATAGS_OFFSET);	/* sets up parameters */
    sprintf(buf,"memory p %x",kernel.memory.physical + ATAGS_OFFSET);
    _swix(OS_CLI,_IN(0),buf);

    if (doBoot)
    {
    	int p=jump_to_linux(kernel.length, MACHTYPE_BCM2708, 
		ATAGS_OFFSET, kernel.memory.physical);
        printf("Returned %x",p);
    }

    _swix(OS_Memory,_INR(0,1),10,0); // free pool unlock

    return NULL;
}


//    machine_type = MACHTYPE_BCM2708;         /* get machine type */
//    irq_shutdown();                         /* stop irq */
//    cpu_op(CPUOP_MMUCHANGE, NULL);          /* turn MMU off */
//    theKernel = (void (*)(int, int, u32))exec_at; /* set the kernel address */
//    theKernel(0, machine_type, parm_at);    /* jump to kernel with register set */
//    return 0;
//}
