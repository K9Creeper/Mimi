#ifndef EXPORT_6502_H
#define EXPORT_6502_H

#include <cpu.h>
#include <bus.h>

#ifdef __cplusplus
extern "C" {
#endif

	extern const struct mimi_cpu_impl_s impl_6502;

	#define MIMI_6502_BUS_ADDRESS 0


#ifdef __cplusplus
}
#endif

#endif