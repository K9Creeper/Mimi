#include <cpu.h>

#include <string.h>

mimi_err_t mimi_cpu_init(mimi_cpu_t* cpu, const struct mimi_cpu_impl_s* impl)
{
	if (!cpu || !impl)
		return MIMI_ERR_BAD_ARG;

	memset(cpu, 0, sizeof(mimi_cpu_t));

	cpu->impl = impl;

	if (!cpu->impl->init)
		return MIMI_ERR_UNSUPPORTED;

	int err = cpu->impl->init(&cpu->private_data);

	if (err == 0)
		return MIMI_OK;

	return (err > 0) ? MIMI_ERR_CPU : err;
}

mimi_err_t mimi_cpu_destroy(mimi_cpu_t* cpu)
{
	if (!cpu)
		return MIMI_ERR_BAD_ARG;

	if (!cpu->impl || !cpu->impl->destroy)
		return MIMI_ERR_UNSUPPORTED;

	int err = cpu->impl->destroy(&cpu->private_data);

	if (err == 0)
		return MIMI_OK;

	return (err > 0) ? MIMI_ERR_CPU : err;
}

mimi_err_t mimi_cpu_attach_bus(mimi_cpu_t* cpu, mimi_bus_t* bus, mimi_bus_role_id_t role)
{
	if (!cpu)
		return MIMI_ERR_BAD_ARG;

	if (!cpu->impl || !cpu->impl->attach_bus)
		return MIMI_ERR_UNSUPPORTED;

	int err = cpu->impl->attach_bus(cpu->private_data, bus, role);

	if (err == 0)
		return MIMI_OK;

	return (err > 0) ? MIMI_ERR_CPU : err;
}

mimi_err_t mimi_cpu_tick(mimi_cpu_t* cpu)
{
	if (!cpu)
		return MIMI_ERR_BAD_ARG;

	if (!cpu->impl || !cpu->impl->tick)
		return MIMI_ERR_UNSUPPORTED;

	int err = cpu->impl->tick(cpu->private_data);

	if (err == 0)
		return MIMI_OK;

	return (err > 0) ? MIMI_ERR_CPU : err;
}
