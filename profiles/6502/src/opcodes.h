#ifndef OPCODES_6502_H
#define OPCODES_6502_H

#include "6502.h"

typedef struct instruction_s {
    mode_t mode;
    int (*handle)(cpu_t* cpu, uint8_t* done);
} instruction_t;

#ifdef __cplusplus
extern "C" {
#endif

	extern const instruction_t* lookup_opcode(register_t opcode);

#ifdef __cplusplus
}
#endif

#define OPCODE_LIST(X)      \
	X(a9, IMPLICIT_MODE)    \
	X(da, IMPLICIT_MODE)    \

#define OPCODE_TO_FN(op, ...) x##op

#define OPCODE_DECL(op, ...)    \
    static int OPCODE_TO_FN(op)(cpu_t *cpu, uint8_t* done);
#define DEFINE_OPCODE(op, ...) \
    static int OPCODE_TO_FN(op)(cpu_t *cpu, uint8_t* done)

#endif