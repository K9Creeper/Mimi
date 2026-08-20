#ifndef EXPORT_6502_H
#define EXPORT_6502_H

#include <cpu.h>
#include <bus.h>

#ifdef __cplusplus
extern "C" {
#endif

	extern const struct mimi_cpu_impl_s impl_6502;
	extern const struct mimi_bus_impl_s	impl_6502_bus;

#ifdef __cplusplus
}
#endif

#endif