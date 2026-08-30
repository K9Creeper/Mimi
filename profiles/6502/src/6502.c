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

static inline mimi_err_t cpu_read(cpu_t* cpu, address_t address, uint8_t* data);
static inline mimi_err_t cpu_write(cpu_t* cpu, address_t address, const uint8_t* data);

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
	if (!pcpu)
		return MIMI_6502_ERR_UNACC_DATA;

	if (*pcpu)
		return MIMI_6502_ERR_BAD_ARG;

	cpu_t* cpu = malloc(sizeof(cpu_t));
	if (!cpu)
		return MIMI_6502_ERR_UNACC_DATA;

	memset(cpu, 0, sizeof(cpu_t));

	*pcpu = (void*)cpu;
	return MIMI_6502_OK;
}

static int destroy(cpu_t** pcpu)
{
	if (!pcpu)
		return MIMI_6502_ERR_UNACC_DATA;

	cpu_t* cpu = *pcpu;
	if (!cpu)
		return MIMI_6502_ERR_UNACC_DATA;

	free(cpu);

	return MIMI_6502_OK;
}

static int attach_bus(cpu_t* cpu, mimi_bus_t* bus, mimi_bus_role_id_t role)
{
	if (!cpu)
		return MIMI_6502_ERR_UNACC_DATA;

	if (!bus)
		return MIMI_6502_ERR_BAD_ARG;

	switch (role) {
		case MIMI_6502_BUS:
		{
			cpu->bus = bus;
			break;
		}

		default: {
			return MIMI_6502_ERR_NOT_FOUND;
			break;
		}
	}

	return MIMI_6502_OK;
}

static int tick(cpu_t* cpu)
{
	if (!cpu)
		return MIMI_6502_ERR_UNACC_DATA;

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
			ret = MIMI_6502_ERR_GENERIC;
			break;
		}

		default:
		{
			ret = MIMI_6502_ERR_NOT_FOUND;
			break;
		}
	}

	cpu->cycles++;
	return ret;
}

static int tick_reset(cpu_t* cpu)
{
	if (!cpu)
		return MIMI_6502_ERR_UNACC_DATA;

	switch (cpu->cycle)
	{
		case 0:
		case 1:
		case 2:
		{
			address_t addr = 0x00FF;

			uint8_t dummy;
			mimi_err_t err = cpu_read(cpu, 0x00FF, &dummy);

			if (err == MIMI_OK) {
				cpu->cycle++;
				return MIMI_6502_OK;
			}
			
			return MIMI_ERR_BUS_DEV;
			break;
		}

		case 3:
		case 4:
		case 5:
		{
			address_t addr = get_stack_address(cpu);

			uint8_t dummy;
			mimi_err_t err = cpu_read(cpu, addr, &dummy);

			if (err == MIMI_OK) {
				cpu->SP--;
				cpu->cycle++;
				return MIMI_6502_OK;
			}

			return MIMI_ERR_BUS_DEV;
			break;
		}

		case 6:
		{
			address_t addr = 0xFFFC;

			uint8_t low_reset;
			mimi_err_t err = cpu_read(cpu, addr, &low_reset);

			if (err == MIMI_OK) {
				cpu->PC = ((cpu->PC & 0xFF00) | low_reset);
				cpu->flags.I = 1;
				cpu->cycle++;
				return MIMI_6502_OK;
			}

			return MIMI_ERR_BUS_DEV;
			break;
		}

		case 7:
		{
			address_t addr = 0xFFFD;

			uint8_t high_reset;
			mimi_err_t err = cpu_read(cpu, addr, &high_reset);

			if (err == MIMI_OK) {
				cpu->PC |= ((address_t)(high_reset << 8) & 0xFF00);
				// cpu->flags.B = 1;
				update_cpu_seq(cpu, CPU_SEQ_FETCH);
				return MIMI_6502_OK;
			}
			
			return MIMI_ERR_BUS_DEV;
			break;
		}

		default:
		{
			return MIMI_6502_ERR_TICK;
			break;
		}
	}

	return MIMI_6502_ERR_TICK;
}

static int tick_fetch(cpu_t* cpu)
{
	if (!cpu)
		return MIMI_6502_ERR_UNACC_DATA;

	switch (cpu->cycle)
	{
		case 0:
		{
			address_t addr = cpu->PC;

			uint8_t instruction;
			mimi_err_t err = cpu_read(cpu, addr, &instruction);

			if (err == MIMI_OK)
			{
				cpu->IR = instruction;
				cpu->PC++;

				update_cpu_seq(cpu, CPU_SEQ_EXECUTE);
				return MIMI_6502_OK;
			}

			return MIMI_ERR_BUS_DEV;
			break;
		}
		default:
		{
			return MIMI_6502_ERR_TICK;
			break;
		}
	}

	return MIMI_6502_ERR_TICK;
}

static int tick_execute(cpu_t* cpu)
{
	if (!cpu)
		return MIMI_6502_ERR_UNACC_DATA;

	const instruction_t* instr = lookup_opcode(cpu->IR);
	if (!instr || instr->mode != cpu->mode)
		return MIMI_6502_ERR_NOT_FOUND;

	if (!instr->handle)
		return MIMI_6502_ERR_GENERIC;

	int err = instr->handle(cpu);
	if (err != MIMI_6502_OK) return err;

	uint8_t int_pending = (cpu->nmi_pending || (cpu->irq_pending && cpu->flags.I));
	cpu_sequence_t next_seq = int_pending ? CPU_SEQ_INTERRUPT : CPU_SEQ_FETCH;

	update_cpu_seq(cpu, next_seq);
	return MIMI_6502_OK;
}

static inline mimi_err_t cpu_read(cpu_t* cpu, address_t address, uint8_t* data)
{
	mimi_bus_request_t req = {
		.access = MIMI_BUS_READ,
		.address = (mimi_address_t)address,
		.data.read = data,
		.size = sizeof(uint8_t)
	};

	return mimi_bus_access(cpu->bus, &req);
}

static inline mimi_err_t cpu_write(cpu_t* cpu, address_t address, const uint8_t* data)
{
	mimi_bus_request_t req = {
		.access = MIMI_BUS_WRITE,
		.address = (mimi_address_t)address,
		.data.write = data,
		.size = sizeof(uint8_t)
	};

	return mimi_bus_access(cpu->bus, &req);
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
