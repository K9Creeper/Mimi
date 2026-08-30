#ifndef _6502_H
#define _6502_H

#include <stdint.h>

#include <export.h>

typedef uint8_t register_t;
typedef uint16_t address_t;

typedef struct flags_s {
	uint8_t C : 1; // Carry
	uint8_t Z : 1; // Zero
	uint8_t I : 1; // Interrupt Disable
	uint8_t D : 1; // Decimal
	uint8_t B : 1; // Break
	uint8_t U : 1; // Unused
	uint8_t V : 1; // Overflow
	uint8_t N : 1; // Negative
} flags_t;
static_assert(sizeof(flags_t) == sizeof(register_t), "flags_t must be exactly 1 byte (the size of the architecture's register)");

typedef enum cpu_sequence_e {
	CPU_SEQ_RESET,
	CPU_SEQ_FETCH,
	CPU_SEQ_EXECUTE,
	CPU_SEQ_INTERRUPT
} cpu_sequence_t;

typedef enum mode_e {
	IMPLICIT_MODE = 0,
	ACCUMULATOR_MODE,
	IMMEDIATE_MODE,
	ZERO_PAGE_MODE,
	ZERO_PAGE_X_MODE,
	ZERO_PAGE_Y_MODE,
	RELATIVE_MODE,
	ABSOLUTE_MODE,
	ABSOLUTE_X_MODE,
	ABSOLUTE_Y_MODE,
	INDIRECT_MODE,
	INDIRECT_INDEXED_MODE
} mode_t;

typedef struct cpu_s {
	address_t   PC;
	register_t  IR;
	register_t  SP;

	register_t  A;
	register_t  X;
	register_t  Y;

	union {
		flags_t     flags;
		register_t	P;
	};

	mode_t mode;
	mimi_bus_t* bus;
	uint8_t bus_rw;

	register_t	addr_lo;
	register_t	addr_hi;
	register_t  data;

	cpu_sequence_t cur_seq;
	uint8_t seq_cycle;

	uint8_t irq_pending;
	uint8_t nmi_pending;

	uint64_t cycles;
} cpu_t;

#ifdef __cplusplus
extern "C" {
#endif

	extern mimi_err_t mimi_6502_cpu_read(cpu_t* cpu);
	extern mimi_err_t mimi_6502_cpu_write(cpu_t* cpu);

#ifdef __cplusplus
}
#endif

#define MIMI_6502_STACK_PAGE       ((address_t)0x0100)

#define MIMI_6502_RESET_DUMMY_ADDRESS ((address_t)0x00FF)

#define MIMI_6502_RESET_VECTOR_LO  ((address_t)0x00FFFC)
#define MIMI_6502_RESET_VECTOR_HI  ((address_t)0x00FFFD)

#define MIMI_6502_NMI_VECTOR       ((address_t)0x00FFFA)
#define MIMI_6502_NMI_VECTOR_HI    ((address_t)0x00FFFB)

#define MIMI_6502_IRQ_VECTOR       ((address_t)0x00FFFE)
#define MIMI_6502_IRQ_VECTOR_HI    ((address_t)0x00FFFF)

#endif