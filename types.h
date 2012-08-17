#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint32_t PhysicalAddress;
typedef uint32_t PageNumber;
typedef void *   LogicalAddress;

typedef struct {
        PageNumber      number;
        void *          logical;
        PhysicalAddress physical;
} PageBlock;


typedef struct
{
  char *name;
  uint32_t length;
  PageBlock memory;
} Image;

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

#endif
