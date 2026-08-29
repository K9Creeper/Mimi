#ifndef OPCODES_6502_H
#define OPCODES_6502_H

#include "6502.h"

typedef int (*opcode_handle_t)(cpu_t* cpu);

#ifdef __cplusplus
extern "C" {
#endif

	extern opcode_handle_t lookup_opcode(register_t opcode, mode_t addr_mode);

#ifdef __cplusplus
}
#endif

#define OPCODE_LIST(X)		\
	X(a9, IMPLICIT_MODE)	\
	X(da, IMPLICIT_MODE)

#endif