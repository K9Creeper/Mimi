#ifndef _6502_H
#define _6502_H

#include <stdint.h>

typedef uint8_t register_t;
typedef uint16_t address_t;

typedef struct flags_s {
    uint8_t CF      : 1;
    uint8_t ZF      : 1;
    uint8_t IF      : 1;
    uint8_t DF      : 1;
    uint8_t BF      : 1;
    uint8_t ignored : 1;
    uint8_t OF      : 1;
    uint8_t NF      : 1;
} flags_t;
static_assert(sizeof(flags_t) == 1, "flags_t must be exactly 1 byte");

typedef struct cpu_s {
    address_t   PC;
    register_t  SP;

	register_t  A;
	register_t  X;
	register_t  Y;

	flags_t     flags;
} cpu_t;

#endif