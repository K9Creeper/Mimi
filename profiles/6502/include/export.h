#ifndef EXPORT_6502_H
#define EXPORT_6502_H

#include <cpu.h>
#include <bus.h>

#define MIMI_6502_ERR_UNACC_DATA	-1
#define MIMI_6502_OK				0
#define MIMI_6502_ERR_GENERIC		1
#define MIMI_6502_ERR_BAD_ARG		2
#define MIMI_6502_ERR_NOT_FOUND		3
#define MIMI_6502_ERR_TICK			4

#ifdef __cplusplus
extern "C" {
#endif

	extern const struct mimi_cpu_impl_s impl_6502;

	#define MIMI_6502_BUS 0
	
#ifdef __cplusplus
}
#endif

#endif