#ifndef OPCODES_6502_H
#define OPCODES_6502_H

#include "6502.h"

typedef int (*opcode_handle_t)(cpu_t* cpu);

#ifdef __cplusplus
extern "C" {
#endif

	extern opcode_handle_t find_opcode_handle(register_t opcode);

#ifdef __cplusplus
}
#endif

#endif