#define LinuxBoot_SWIBase	0x12300
#define LinuxBoot_Go		0
#define LinuxBoot_Messaging	1



#define OSERROR _kernel_oserror
#define UNUSED(x)

#define OSMEMORY_SUPPLY_PHYSICAL_PAGENO        (1<<8)
#define OSMEMORY_SUPPLY_LOGICAL_ADDR   (1<<9)
#define OSMEMORY_SUPPLY_PHYSICAL_ADDR  (1<<10)
#define OSMEMORY_WANT_PHYSICAL_PAGENO  (1<<11)
#define OSMEMORY_WANT_LOGICAL_ADDR     (1<<12)
#define OSMEMORY_WANT_PHYSICAL_ADDR    (1<<13)
#define OSMEMORY_DISABLE_CACHING       (2<<14)
#define OSMEMORY_ENABLE_CACHING                (3<<14)

