#ifndef CPU_H
#define CPU_H

#include <bus.h>

struct mimi_cpu_impl_s {
	int (*init)(void** pprivate_data);

	int (*destroy)(void** pprivate_data);

	int (*attach_bus)(void* private_data, mimi_bus_t* bus, mimi_bus_role_id_t role);
	
	int (*tick)(void* private_data);

	void (*reset)(void* private_data);
};

typedef struct mimi_cpu_s {
	const struct mimi_cpu_impl_s* impl;
	void* private_data;
} mimi_cpu_t;

#ifdef __cplusplus
extern "C" {
#endif

	extern int mimi_cpu_init(mimi_cpu_t* cpu, const struct mimi_cpu_impl_s* impl);

	extern int mimi_cpu_destroy(mimi_cpu_t* cpu);
	
	extern int mimi_cpu_attach_bus(mimi_cpu_t* cpu, mimi_bus_t* bus, mimi_bus_role_id_t role);

	extern int mimi_cpu_tick(mimi_cpu_t* cpu);

#ifdef __cplusplus
}
#endif

#endif