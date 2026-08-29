#include "6502.h"

#include <stddef.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <cpu.h>
#include <bus.h>

#include "opcodes.h"

static inline address_t get_stack_address(cpu_t* cpu);
static inline void update_cpu_seq(cpu_t* cpu, cpu_sequence_t seq);

static inline int cpu_read(cpu_t* cpu, address_t address, uint8_t* data);
static inline int cpu_write(cpu_t* cpu, address_t address, const uint8_t* data);

static int tick_reset(cpu_t* cpu);
static int tick_fetch(cpu_t* cpu);
static int tick_execute(cpu_t* cpu);

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
	switch (cpu->cur_seq) {
		case CPU_SEQ_RESET:
		{
			ret = tick_reset(cpu);
			break;
		}

		case CPU_SEQ_FETCH:
		{
			ret = tick_fetch(cpu);
			break;
		}

		case CPU_SEQ_EXECUTE:
		{
			ret = tick_execute(cpu);
			break;
		}

		case CPU_SEQ_INTERRUPT:
		{
			ret = 1;
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

static int tick_reset(cpu_t* cpu)
{
	if (!cpu)
		return -1;

	int ret;
	switch (cpu->cycle)
	{
		case 0:
		case 1:
		case 2:
		{
			address_t addr = 0x00FF;

			uint8_t dummy;
			ret = cpu_read(cpu, 0x00FF, &dummy);

			if (!ret) {
				cpu->cycle++;
			}
			break;
		}

		case 3:
		case 4:
		case 5:
		{
			address_t addr = get_stack_address(cpu);

			uint8_t dummy;
			ret = cpu_read(cpu, addr, &dummy);

			if (!ret) {
				cpu->SP--;
				cpu->cycle++;
			}
			break;
		}
		
		case 6:
		{
			address_t addr = 0xFFFC;

			uint8_t low_reset;
			ret = cpu_read(cpu, addr, &low_reset);

			if (!ret) {
				cpu->PC = ((cpu->PC & 0xFF00) | low_reset);
				cpu->flags.I = 1;
				cpu->cycle++;
			}
			break;
		}

		case 7:
		{
			address_t addr = 0xFFFD;

			uint8_t high_reset;
			ret = cpu_read(cpu, addr, &high_reset);
			
			if (!ret) {
				cpu->PC |= ((address_t)(high_reset << 8) & 0xFF00);
				// cpu->flags.B = 1;
				update_cpu_seq(cpu, CPU_SEQ_FETCH);
			}
			break;
		}

		default:
		{
			ret = -1;
			break;
		}
	}

	return ret;
}

static int tick_fetch(cpu_t* cpu)
{
	if (!cpu)
		return -1;
	
	int ret;
	switch (cpu->cycle)
	{
		case 0:
		{
			address_t addr = cpu->PC;

			uint8_t instruction;
			ret = cpu_read(cpu, addr, &instruction);
			if (!ret)
			{
				cpu->IR = instruction;
				cpu->PC++;

				update_cpu_seq(cpu, CPU_SEQ_EXECUTE);
			}
			break;
		}
		default:
		{
			ret = -1;
			break;
		}
	}

	return ret;
}

static int tick_execute(cpu_t* cpu)
{
	if (!cpu)
		return -1;

	opcode_handle_t handle = find_opcode_handle(cpu->IR);
	printf("Executing instruction 0x%x (func: %p).\n", cpu->IR, handle);
	if (!handle)
		return 1;

	return handle(cpu);
}

static inline int cpu_read(cpu_t* cpu, address_t address, uint8_t* data)
{
	mimi_bus_request_t req = {
		.access = MIMI_BUS_READ,
		.address = (mimi_address_t)address,
		.data.read = data,
		.size = sizeof(uint8_t)
	};

	return 	mimi_bus_access(cpu->addr_bus, &req);
}

static inline int cpu_write(cpu_t* cpu, address_t address, const uint8_t* data)
{
	mimi_bus_request_t req = {
		.access = MIMI_BUS_WRITE,
		.address = (mimi_address_t)address,
		.data.write = data,
		.size = sizeof(uint8_t)
	};

	return 	mimi_bus_access(cpu->addr_bus, &req);
}

static inline address_t get_stack_address(cpu_t* cpu)
{
	return 0x0100 - cpu->SP;
}

static inline void update_cpu_seq(cpu_t* cpu, cpu_sequence_t seq)
{
	cpu->cur_seq = seq;
	cpu->cycle = 0;
}
