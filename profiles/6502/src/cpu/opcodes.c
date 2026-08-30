#include "opcodes.h"

static inline mimi_err_t cpu_read(cpu_t* cpu, address_t address);
static inline mimi_err_t cpu_write(cpu_t* cpu, address_t address);

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

DEFINE_OPCODE(a9)
{
    if (!cpu || !done)
        return MIMI_6502_ERR_BAD_ARG;

    *done = 0;

    switch (cpu->cycle)
    {
		case 0:
		{
            mimi_err_t err = cpu_read(cpu, cpu->PC);

            if (err == MIMI_OK)
            {
                cpu->A = cpu->data;
				cpu->PC++;
                cpu->flags.Z = (cpu->A == 0);
                cpu->flags.N = (cpu->A & 0x80) != 0;
                cpu->cycle++;

				*done = 1;
                return MIMI_6502_OK;
            }
            return MIMI_ERR_BUS_DEV;
			break;
		}

		default:
		{
            return MIMI_6502_ERR_TICK;
			break;
		}
    }

    return MIMI_6502_ERR_TICK;
}

DEFINE_OPCODE(ea)
{
	if (!cpu || !done)
        return MIMI_6502_ERR_BAD_ARG;

    *done = 0;

    // NOP

    *done = 1;
    return MIMI_6502_OK;
}

DEFINE_OPCODE(8d)
{
	static uint8_t addr_lo;
	static uint8_t addr_hi;

	if (!cpu || !done)
		return MIMI_6502_ERR_BAD_ARG;

	*done = 0;
	
    switch (cpu->cycle)
    {
		case 0:
		{
			mimi_err_t err = cpu_read(cpu, cpu->PC);

			if (err == MIMI_OK)
			{
				addr_lo = cpu->data;
				cpu->PC++;
				cpu->cycle++;
				return MIMI_6502_OK;
			}
			return MIMI_ERR_BUS_DEV;
			break;
		}
		
		case 1:
		{
			mimi_err_t err = cpu_read(cpu, cpu->PC);

			if (err == MIMI_OK)
			{
				addr_hi = cpu->data;
				cpu->PC++;
				cpu->cycle++;
				return MIMI_6502_OK;
			}
			return MIMI_ERR_BUS_DEV;
			break;
		}

		case 2:
		{
			cpu->data = cpu->A;

			address_t address = (((address_t)addr_hi << 8) & 0xFF00) | addr_lo;
			mimi_err_t err = cpu_write(cpu, address);

			if (err == MIMI_OK)
			{
				cpu->cycle++;
				*done = 1;
				return MIMI_6502_OK;
			}
			return MIMI_ERR_BUS_DEV;
			break;
		}

		default:
		{
			return MIMI_6502_ERR_TICK;
			break;
		}
    }

    return MIMI_6502_ERR_TICK;
}

DEFINE_OPCODE(4c)
{
	static uint8_t addr_lo;
	static uint8_t addr_hi;

	if (!cpu || !done)
		return MIMI_6502_ERR_BAD_ARG;

	*done = 0;

	switch (cpu->cycle)
	{
		case 0:
		{
			mimi_err_t err = cpu_read(cpu, cpu->PC);

			if (err == MIMI_OK)
			{
				addr_lo = cpu->data;
				cpu->PC++;
				cpu->cycle++;
				return MIMI_6502_OK;
			}
			return MIMI_ERR_BUS_DEV;
			break;
		}

		case 1:
		{
			mimi_err_t err = cpu_read(cpu, cpu->PC);

			if (err == MIMI_OK)
			{
				addr_hi = cpu->data;

				address_t address = (((address_t)addr_hi << 8) & 0xFF00) | addr_lo;
				cpu->PC = address;
				cpu->cycle++;

				*done = 1;
				return MIMI_6502_OK;
			}
			return MIMI_ERR_BUS_DEV;
			break;
		}

		default:
		{
			return MIMI_6502_ERR_TICK;
			break;
		}
	}

	return MIMI_6502_ERR_TICK;
}

static inline mimi_err_t cpu_read(cpu_t* cpu, address_t address)
{
	mimi_bus_request_t req = {
		.access = MIMI_BUS_READ,
		.address = (mimi_address_t)address,
		.data.read = &cpu->data,
		.size = sizeof(uint8_t)
	};

	return mimi_bus_access(cpu->bus, &req);
}

static inline mimi_err_t cpu_write(cpu_t* cpu, address_t address)
{
	mimi_bus_request_t req = {
		.access = MIMI_BUS_WRITE,
		.address = (mimi_address_t)address,
		.data.write = &cpu->data,
		.size = sizeof(uint8_t)
	};

	return mimi_bus_access(cpu->bus, &req);
}
