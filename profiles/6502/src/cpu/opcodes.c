#include "opcodes.h"

static inline mimi_err_t cpu_read(cpu_t* cpu);
static inline mimi_err_t cpu_write(cpu_t* cpu);

OPCODE_LIST(OPCODE_DECL)

static const instruction_t opcode_table[256] = {
#define OPCODE_ENTRY(op) \
    [0x##op] = {.handle = OPCODE_TO_FN(op)},

	OPCODE_LIST(OPCODE_ENTRY)
};

const instruction_t* lookup_opcode(register_t opcode)
{
	return &opcode_table[opcode];
}

#include <stdio.h>

DEFINE_OPCODE(ea)
{
	if (!cpu || !done)
		return MIMI_6502_ERR_BAD_ARG;

	*done = 1;

	return MIMI_6502_OK;
}

DEFINE_OPCODE(a9)
{
	if (!cpu || !done)
		return MIMI_6502_ERR_BAD_ARG;

	*done = 0;

	switch (cpu->seq_cycle)
	{
	case 0:
	{
		cpu->addr_lo = (register_t)(cpu->PC & 0x00FF);
		cpu->addr_hi = (register_t)(cpu->PC >> 8) & 0x00FF;

		mimi_err_t err = cpu_read(cpu);

		if (err != MIMI_OK)
			return MIMI_ERR_BUS_DEV;

		cpu->A = cpu->data;
		cpu->PC++;
		cpu->flags.Z = (cpu->A == 0);
		cpu->flags.N = (cpu->A & 0x80) != 0;

		*done = 1;

		return MIMI_6502_OK;
	}

	default:
		return MIMI_6502_ERR_TICK;
	}
}

DEFINE_OPCODE(8d)
{
	static register_t addr_lo;
	static register_t addr_hi;

	if (!cpu || !done)
		return MIMI_6502_ERR_BAD_ARG;

	*done = 0;

	switch (cpu->seq_cycle)
	{
	case 0:
	{
		cpu->addr_lo = (register_t)(cpu->PC & 0x00FF);
		cpu->addr_hi = (register_t)(cpu->PC >> 8) & 0x00FF;

		mimi_err_t err = cpu_read(cpu);

		if (err != MIMI_OK)
			return MIMI_ERR_BUS_DEV;
		
		addr_lo = cpu->data;
		cpu->PC++;
		cpu->seq_cycle++;

		return MIMI_6502_OK;
	}

	case 1:
	{
		cpu->addr_lo = (register_t)(cpu->PC & 0x00FF);
		cpu->addr_hi = (register_t)(cpu->PC >> 8) & 0x00FF;

		mimi_err_t err = cpu_read(cpu);

		if (err != MIMI_OK)
			return MIMI_ERR_BUS_DEV;

		addr_hi = cpu->data;
		cpu->PC++;
		cpu->seq_cycle++;

		return MIMI_6502_OK;
	}

	case 2:
	{
		cpu->addr_lo = addr_lo;
		cpu->addr_hi = addr_hi;

		cpu->data = cpu->A;

		mimi_err_t err = cpu_write(cpu);

		if (err != MIMI_OK)
			return MIMI_ERR_BUS_DEV;

		*done = 1;

		return MIMI_6502_OK;
	}

	default:
		return MIMI_6502_ERR_TICK;
	}
}

DEFINE_OPCODE(4c)
{
	static register_t addr_lo;
	static register_t addr_hi;

	if (!cpu || !done)
		return MIMI_6502_ERR_BAD_ARG;

	*done = 0;

	switch (cpu->seq_cycle)
	{
	case 0:
	{
		cpu->addr_lo = (register_t)(cpu->PC & 0x00FF);
		cpu->addr_hi = (register_t)(cpu->PC >> 8) & 0x00FF;

		mimi_err_t err = cpu_read(cpu);

		if (err != MIMI_OK)
			return MIMI_ERR_BUS_DEV;

		addr_lo = cpu->data;
		cpu->PC++;
		cpu->seq_cycle++;

		return MIMI_6502_OK;
	}

	case 1:
	{
		cpu->addr_lo = (register_t)(cpu->PC & 0x00FF);
		cpu->addr_hi = (register_t)(cpu->PC >> 8) & 0x00FF;

		mimi_err_t err = cpu_read(cpu);

		if (err != MIMI_OK)
			return MIMI_ERR_BUS_DEV;

		addr_hi = cpu->data;
		cpu->PC = (((address_t)addr_hi << 8) & 0xFF00) | ((address_t)addr_lo & 0x00FF);

		*done = 1;

		return MIMI_6502_OK;
	}

	default:
		return MIMI_6502_ERR_TICK;
	}
}

static inline mimi_err_t cpu_read(cpu_t* cpu)
{
	return mimi_6502_cpu_read(cpu);
}

static inline mimi_err_t cpu_write(cpu_t* cpu)
{
	return mimi_6502_cpu_write(cpu);
}
