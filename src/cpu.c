#include <cpu.h>

#include <string.h>

int mimi_cpu_init(mimi_cpu_t* cpu, const struct mimi_cpu_impl_s* impl)
{
	if (!cpu || !impl)
		return -1;

	memset(cpu, 0, sizeof(mimi_cpu_t));

	cpu->impl = impl;

	if (!cpu->impl->init)
		return -1;

	return cpu->impl->init(&cpu->private_data);
}