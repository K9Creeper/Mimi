#include "opcodes.h"

#define OPCODE_TO_FN(op) x##op

#define OPCODE_DECL(op, ...)    \
    static int OPCODE_TO_FN(op)(cpu_t *cpu);
#define DEFINE_OPCODE(op) \
    static int OPCODE_TO_FN(op)(cpu_t *cpu)

OPCODE_LIST(OPCODE_DECL)

struct opcode_s {
    mode_t mode;
    opcode_handle_t handle;
};

OPCODE_LIST(OPCODE_DECL)

#define OPCODE_ENTRY(op, addr_mode) \
    [0x##op] = { .handle = OPCODE_TO_FN(op), .mode = ##addr_mode },

static struct opcode_s opcode_table[256] = {
    OPCODE_LIST(OPCODE_ENTRY)
};

opcode_handle_t lookup_opcode(register_t opcode, mode_t addr_mode)
{
    struct opcode_s op = opcode_table[opcode];
    if (op.mode != addr_mode)
        return -1;
    return op.handle;
}

#include <stdio.h>

DEFINE_OPCODE(a9)
{
    printf("Executing LDA #. ");
    printf("FAILING PURPOSEFULLY.\n");
    return 1;
}

DEFINE_OPCODE(da)
{
    printf("Executing NOP.\n");
    return 0;
}
