/* Linux ATAG structures
 * taken from
 * http://www.simtec.co.uk/products/SWLINUX/files/booting_article.html
 *
 * BSD licensed
 */

#ifndef ATAGS_H
#define ATAGS_H

#include "types.h"

/* list of possible tags */
#define ATAG_NONE       0x00000000
#define ATAG_CORE       0x54410001
#define ATAG_MEM        0x54410002
#define ATAG_VIDEOTEXT  0x54410003
#define ATAG_RAMDISK    0x54410004
#define ATAG_INITRD2    0x54420005
#define ATAG_SERIAL     0x54410006
#define ATAG_REVISION   0x54410007
#define ATAG_VIDEOLFB   0x54410008
#define ATAG_CMDLINE    0x54410009

/* structures for each atag */
struct atag_header {
        u32 size; /* length of tag in words including this header */
        u32 tag;  /* tag type */
};

struct atag_core {
        u32 flags;              /* bit 0 = read-only */
        u32 pagesize;           /* systems page size (usually 4k) */
        u32 rootdev;            /* root device number */
};

struct atag_mem {
        u32     size;   /* size of the area */
        u32     start;  /* physical start address */
};

struct atag_videotext {
        u8              x;           /* width of display */
        u8              y;           /* height of display */
        u16             video_page;
        u8              video_mode;
        u8              video_cols;
        u16             video_ega_bx;
        u8              video_lines;
        u8              video_isvga;
        u16             video_points;
};

struct atag_ramdisk {
        u32 flags;      /* bit 0 = load, bit 1 = prompt */
        u32 size;       /* decompressed ramdisk size in _kilo_ bytes */
        u32 start;      /* starting block of floppy-based RAM disk image */
};

struct atag_initrd2 {
        u32 start;      /* physical start address */
        u32 size;       /* size of compressed ramdisk image in bytes */
};

struct atag_serialnr {
        u32 low;
        u32 high;
};

struct atag_revision {
        u32 rev;
};

struct atag_videolfb {
        u16             lfb_width;
        u16             lfb_height;
        u16             lfb_depth;
        u16             lfb_linelength;
        u32             lfb_base;
        u32             lfb_size;
        u8              red_size;
        u8              red_pos;
        u8              green_size;
        u8              green_pos;
        u8              blue_size;
        u8              blue_pos;
        u8              rsvd_size;
        u8              rsvd_pos;
};

struct atag_cmdline {
        char    cmdline[1];      /* this is the minimum size */
};

struct atag {
        struct atag_header hdr;
        union {
                struct atag_core         core;
                struct atag_mem          mem;
                struct atag_videotext    videotext;
                struct atag_ramdisk      ramdisk;
                struct atag_initrd2      initrd2;
                struct atag_serialnr     serialnr;
                struct atag_revision     revision;
                struct atag_videolfb     videolfb;
                struct atag_cmdline      cmdline;
        } u;
};


#endif
