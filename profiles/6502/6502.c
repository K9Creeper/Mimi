#include "6502.h"

#include <stddef.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <cpu.h>
#include <bus.h>

static inline int read_byte(cpu_t* cpu, address_t address, uint8_t* data);
static inline int write_byte(cpu_t* cpu, address_t address, const uint8_t* data);

static int init(cpu_t** pcpu);
static int destroy(cpu_t** pcpu);
static int attach_bus(cpu_t* cpu, mimi_bus_t* bus, mimi_bus_role_id_t role);
static int tick(cpu_t* cpu);

const struct mimi_cpu_impl_s impl_6502 = {
	.init = init,
	.destroy = destroy,
	.attach_bus = attach_bus,
	.tick = tick,
	.reset = NULL
};

static int init(cpu_t** pcpu)
{
	if (!pcpu || *pcpu)
		return -1;

	cpu_t* cpu = malloc(sizeof(cpu_t));
	if (!cpu)
		return -2;

	memset(cpu, 0, sizeof(cpu_t));

	*pcpu = (void*)cpu;
	return 0;
}

static int destroy(cpu_t** pcpu)
{
	if (!pcpu)
		return -1;

	cpu_t* cpu = *pcpu;
	if (!cpu)
		return -1;

	free(cpu);

	return 0;
}

static int attach_bus(cpu_t* cpu, mimi_bus_t* bus, mimi_bus_role_id_t role)
{
	if (!cpu || !bus)
		return -1;

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

static int tick(cpu_t* cpu)
{
	if (!cpu)
		return -1;
	
	int ret;
	switch (cpu->state) {
		case CPU_RESET:
		{
			ret = 0;
			break;
		}

		case CPU_FETCH:
		{
			ret = 0;
			break;
		}

		case CPU_EXECUTE:
		{
			ret = 0;
			break;
		}

		default:
		{
			ret = -1;
			break;
		}
	}

	cpu->cycles++;
	return ret;
}

static inline int read_byte(cpu_t* cpu, address_t address, uint8_t* data)
{
	if (!cpu || !data)
		return -1;

	mimi_bus_request_t req = {
		.access = MIMI_BUS_READ,
		.address = (mimi_address_t)address,
		.data.read = data,
		.size = sizeof(uint8_t)
	};

	return 	mimi_bus_access(cpu->addr_bus, &req);
}

static inline int write_byte(cpu_t* cpu, address_t address, const uint8_t* data)
{
	if (!cpu || !data)
		return -1;

	mimi_bus_request_t req = {
		.access = MIMI_BUS_WRITE,
		.address = (mimi_address_t)address,
		.data.write = data,
		.size = sizeof(uint8_t)
	};

	return 	mimi_bus_access(cpu->addr_bus, &req);
}
