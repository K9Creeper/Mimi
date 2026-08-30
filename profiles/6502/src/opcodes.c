#include "opcodes.h"

static inline mimi_err_t cpu_read(cpu_t* cpu, address_t address, uint8_t* data);
static inline mimi_err_t cpu_write(cpu_t* cpu, address_t address, const uint8_t* data);

OPCODE_LIST(OPCODE_DECL)

static const instruction_t opcode_table[256] = {
#define OPCODE_ENTRY(op, addr_mode) \
    [0x##op] = {.mode = addr_mode, .handle = OPCODE_TO_FN(op)},

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
            uint8_t value;
            mimi_err_t err = cpu_read(cpu, cpu->PC, &value);

            if (err == MIMI_OK)
            {
                cpu->A = value;
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

DEFINE_OPCODE(da)
{
	if (!cpu || !done)
        return MIMI_6502_ERR_BAD_ARG;

    *done = 0;

    // NOP

    *done = 1;
    return MIMI_6502_OK;
}

static inline mimi_err_t cpu_read(cpu_t* cpu, address_t address, uint8_t* data)
{
	mimi_bus_request_t req = {
		.access = MIMI_BUS_READ,
		.address = (mimi_address_t)address,
		.data.read = data,
		.size = sizeof(uint8_t)
	};

	return mimi_bus_access(cpu->bus, &req);
}

static inline mimi_err_t cpu_write(cpu_t* cpu, address_t address, const uint8_t* data)
{
	mimi_bus_request_t req = {
		.access = MIMI_BUS_WRITE,
		.address = (mimi_address_t)address,
		.data.write = data,
		.size = sizeof(uint8_t)
	};

	return mimi_bus_access(cpu->bus, &req);
}
