#include "6502.h"

#include <stddef.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <cpu.h>
#include <bus.h>

#include "opcodes.h"

static uint8_t bus_used_read;
static inline void print_cpu_state(cpu_t* cpu);

static inline address_t get_stack_address(cpu_t* cpu);
static inline void update_cpu_seq(cpu_t* cpu, cpu_sequence_t seq);
static inline mimi_err_t cpu_read(cpu_t* cpu);
static inline mimi_err_t cpu_write(cpu_t* cpu);

static int tick_reset(cpu_t* cpu);
static int tick_fetch(cpu_t* cpu);
static int tick_execute(cpu_t* cpu);
static int tick_interrupt(cpu_t* cpu);

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

mimi_err_t mimi_6502_cpu_read(cpu_t* cpu)
{
	mimi_bus_request_t req = {
		.access = MIMI_BUS_READ,
		.address = (mimi_address_t)((((address_t)cpu->addr_hi << 8) & 0x00FF00) | cpu->addr_lo),
		.data.read = &cpu->data,
		.size = sizeof(uint8_t)
	};

	bus_used_read = 1;
	return mimi_bus_access(cpu->bus, &req);
}

mimi_err_t mimi_6502_cpu_write(cpu_t* cpu) {
	mimi_bus_request_t req = {
		.access = MIMI_BUS_WRITE,
		.address = (mimi_address_t)((((address_t)cpu->addr_hi << 8) & 0x00FF00) | cpu->addr_lo),
		.data.write = &cpu->data,
		.size = sizeof(uint8_t)
	};

	bus_used_read = 0;
	return mimi_bus_access(cpu->bus, &req);
}

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
		ret = tick_interrupt(cpu);
		break;
	}

	default:
	{
		ret = MIMI_6502_ERR_NOT_FOUND;
		break;
	}
	}
	
	print_cpu_state(cpu);

	cpu->cycles++;
	return ret;
}

static int tick_reset(cpu_t* cpu)
{
	if (!cpu)
		return MIMI_6502_ERR_UNACC_DATA;

	switch (cpu->seq_cycle)
	{
	case 0:
	case 1:
	case 2:
	{
		cpu->addr_lo = (register_t)((address_t)MIMI_6502_RESET_DUMMY_ADDRESS & 0x00FF);
		cpu->addr_hi = (register_t)(((address_t)MIMI_6502_RESET_DUMMY_ADDRESS >> 8) & 0x00FF);

		mimi_err_t err = cpu_read(cpu);

		if (err == MIMI_OK) {
			cpu->seq_cycle++;
			return MIMI_6502_OK;
		}

		return MIMI_ERR_BUS_DEV;
	}

	case 3:
	case 4:
	case 5:
	{
		address_t stack_addr = get_stack_address(cpu);

		cpu->addr_lo = (register_t)((address_t)stack_addr & 0x00FF);
		cpu->addr_hi = (register_t)(((address_t)stack_addr >> 8) & 0x00FF);

		mimi_err_t err = cpu_read(cpu);

		if (err == MIMI_OK) {
			cpu->SP--;
			cpu->seq_cycle++;
			return MIMI_6502_OK;
		}

		return MIMI_ERR_BUS_DEV;
	}

	case 6:
	{
		cpu->addr_lo = (register_t)((address_t)MIMI_6502_RESET_VECTOR_LO & 0x00FF);
		cpu->addr_hi = (register_t)(((address_t)MIMI_6502_RESET_VECTOR_LO >> 8) & 0x00FF);

		mimi_err_t err = cpu_read(cpu);

		if (err != MIMI_OK)
			return MIMI_ERR_BUS_DEV;

		cpu->PC = (cpu->PC & 0x00FF00) | cpu->data;
		cpu->flags.I = 1;
		cpu->seq_cycle++;

		return MIMI_6502_OK;
	}

	case 7:
	{
		cpu->addr_lo = (register_t)((address_t)MIMI_6502_RESET_VECTOR_HI & 0x00FF);
		cpu->addr_hi = (register_t)(((address_t)MIMI_6502_RESET_VECTOR_HI >> 8) & 0x00FF);

		mimi_err_t err = cpu_read(cpu);

		if (err != MIMI_OK)
			return MIMI_ERR_BUS_DEV;

		cpu->PC = (cpu->PC & 0x00FF) | ((address_t)cpu->data << 8);

		update_cpu_seq(cpu, CPU_SEQ_FETCH);

		return MIMI_6502_OK;
	}

	default:
		return MIMI_6502_ERR_TICK;
	}

	return MIMI_6502_ERR_TICK;
}

static int tick_fetch(cpu_t* cpu)
{
	if (!cpu)
		return MIMI_6502_ERR_UNACC_DATA;

	switch (cpu->seq_cycle)
	{
	case 0:
	{
		address_t addr = cpu->PC;

		cpu->addr_lo = (register_t)((address_t)addr & 0x00FF);
		cpu->addr_hi = (register_t)(((address_t)addr >> 8) & 0x00FF);

		mimi_err_t err = cpu_read(cpu);

		if (err == MIMI_OK)
		{
			cpu->IR = cpu->data;
			cpu->PC++;

			update_cpu_seq(cpu, CPU_SEQ_EXECUTE);

			return MIMI_6502_OK;
		}

		return MIMI_ERR_BUS_DEV;
	}

	default:
		return MIMI_6502_ERR_TICK;
	}

	return MIMI_6502_ERR_TICK;
}

static int tick_execute(cpu_t* cpu)
{
	if (!cpu)
		return MIMI_6502_ERR_UNACC_DATA;

	const instruction_t* instr = lookup_opcode(cpu->IR);

	if (!instr || !instr->handle)
		return MIMI_6502_ERR_NOT_FOUND;

	uint8_t instr_done = 1;
	int err = instr->handle(cpu, &instr_done);

	if (err != MIMI_6502_OK)
		return err;

	if (!instr_done)
		return MIMI_6502_OK;

	uint8_t int_pending = (cpu->nmi_pending || (cpu->irq_pending && cpu->flags.I));
	cpu_sequence_t next_seq = int_pending ? CPU_SEQ_INTERRUPT : CPU_SEQ_FETCH;

	update_cpu_seq(cpu, next_seq);

	return MIMI_6502_OK;
}

static int tick_interrupt(cpu_t* cpu)
{
	return MIMI_6502_ERR_GENERIC;
}

static inline mimi_err_t cpu_read(cpu_t* cpu)
{
	return mimi_6502_cpu_read(cpu);
}

static inline mimi_err_t cpu_write(cpu_t* cpu) {
	return mimi_6502_cpu_write(cpu);
}

static inline address_t get_stack_address(cpu_t* cpu)
{
	return MIMI_6502_STACK_PAGE | cpu->SP;
}

static inline void update_cpu_seq(cpu_t* cpu, cpu_sequence_t seq)
{
	static const char* seq_str[] = {
		[CPU_SEQ_RESET] = "RESET",
		[CPU_SEQ_FETCH] = "FETCH",
		[CPU_SEQ_EXECUTE] = "EXECUTE",
		[CPU_SEQ_INTERRUPT] = "INTERRUPT"
	};

	cpu->cur_seq = seq;
	cpu->seq_cycle = 0;
	printf("Switched to %s.\n", seq_str[seq]);
}

static inline void print_cpu_state(cpu_t* cpu)
{
	if (cpu)
	{
		printf("#%llu AB:%02X%02X D:%02X R/W:%u PC:%04X A:%02X X:%02X Y:%02X SP:%02X P:%02X IR:%02X ",
			(unsigned long long)cpu->cycles,
			(unsigned int)cpu->addr_hi,
			(unsigned int)cpu->addr_lo,
			(unsigned int)cpu->data,
			(unsigned int)0x01,
			(unsigned int)cpu->PC,
			(unsigned int)cpu->A,
			(unsigned int)cpu->X,
			(unsigned int)cpu->Y,
			(unsigned int)cpu->SP,
			(unsigned int)cpu->P,
			(unsigned int)cpu->IR);

		if (bus_used_read)
			printf("READ $%04X = $%02X",
				(unsigned int)(((address_t)cpu->addr_hi << 8) | cpu->addr_lo),
				(unsigned int)cpu->data);
		else
			printf("WRITE $%04X = $%02X",
				(unsigned int)(((address_t)cpu->addr_hi << 8) | cpu->addr_lo),
				(unsigned int)cpu->data);

		printf("\n");
	}
}
