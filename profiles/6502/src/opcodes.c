#include "opcodes.h"

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
    printf("Executing LDA #. ");
    printf("FAILING PURPOSEFULLY.\n");
    return MIMI_6502_ERR_GENERIC;
}

DEFINE_OPCODE(da)
{
    printf("Executing NOP.\n");
    return MIMI_6502_OK;
}
