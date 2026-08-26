#include "6502.h"

#include <stddef.h>

#include <stdlib.h>
#include <string.h>

#include <cpu.h>
#include <bus.h>

static int init(void** pprivate_data);
static int destroy(void** pprivate_data);
static int attach_bus(void* private_data, mimi_bus_t* bus, mimi_bus_role_id_t role);

const struct mimi_cpu_impl_s impl_6502 = {
	.init = init,
	.destroy = destroy,
	.attach_bus = attach_bus,
	.reset = NULL,
};

static int init(void** pprivate_data)
{
	if (!pprivate_data || *pprivate_data)
		return -1;

	cpu_t* cpu = malloc(sizeof(cpu_t));
	if (!cpu)
		return -2;

	memset(cpu, 0, sizeof(cpu_t));

	*pprivate_data = (void*)cpu;
	return 0;
}

static int destroy(void** pprivate_data)
{
	if (!pprivate_data)
		return -1;

	cpu_t* cpu = *pprivate_data;
	if (!cpu)
		return -1;

	free(cpu);

	return 0;
}

static int attach_bus(void* private_data, mimi_bus_t* bus, mimi_bus_role_id_t role)
{
	if (!private_data || !bus)
		return -1;

	cpu_t* cpu = private_data;

	switch (role) {
		case MIMI_6502_BUS_ADDRESS:
		{
			cpu->addr_bus = bus;
			break;
		}

		default: {
			return 1;
			break;
		}
	}

	return 0;
}
