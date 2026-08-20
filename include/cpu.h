#ifndef CPU_H
#define CPU_H

struct mimi_cpu_s;

struct mimi_cpu_impl_s {
	int (*init)(void** pprivate_data);
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

#ifdef __cplusplus
}
#endif

#endif