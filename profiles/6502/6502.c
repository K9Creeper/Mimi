#include "6502.h"

#include <stddef.h>

#include <stdlib.h>
#include <string.h>

#include <cpu.h>
#include <bus.h>

static int cpu_init(void** pprivate_data)
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

const struct mimi_cpu_impl_s impl_6502 = {
	.init = cpu_init,
	.reset = NULL,
};
