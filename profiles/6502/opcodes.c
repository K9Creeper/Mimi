#include "opcodes.h"

#define MAX_COL 16
#define MAX_ROW 16

static int fail_handle(cpu_t* cpu)
{
	return -1;
}

static const opcode_handle_t opcode_table[MAX_ROW][MAX_COL];

opcode_handle_t find_opcode_handle(register_t opcode)
{
	uint8_t hi = (opcode >> 4) & 0x0F;
	uint8_t lo = opcode & 0x0F;
	return opcode_table[hi][lo];
}

static const opcode_handle_t opcode_table[MAX_ROW][MAX_COL] = {
	{ fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle },
	{ fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle },
	{ fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle },
	{ fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle },
	{ fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle },
	{ fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle },
	{ fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle },
	{ fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle },
	{ fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle },
	{ fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle },
	{ fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, NULL, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle },
	{ fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle },
	{ fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle },
	{ fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle },
	{ fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle },
	{ fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle, fail_handle }
};
