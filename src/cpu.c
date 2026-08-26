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

int mimi_cpu_destroy(mimi_cpu_t* cpu)
{
	if (!cpu || !cpu->impl)
		return -1;

	if (!cpu->impl->destroy)
		return -1;

	return cpu->impl->destroy(&cpu->private_data);
}

int mimi_cpu_attach_bus(mimi_cpu_t* cpu, mimi_bus_t* bus, mimi_bus_role_id_t role)
{
	if (!cpu || !cpu->impl)
		return -1;

	if (!cpu->impl->attach_bus)
		return -1;

	return cpu->impl->attach_bus(cpu->private_data, bus, role);
}
