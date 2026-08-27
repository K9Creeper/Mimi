#include <iostream>

#include <mimi.h>

#include <bus-dev/mem.h>
#include <bus-dev/rom.h>

#include "../profiles/6502/export.h"

static const uint8_t test_memory[] = {
	0x1, 0x2, 0x3, 0x1, 0x2, 0x3,
	0xD, 0xE, 0xA, 0xD, 0xB, 0xE,
	0xE, 0xF
};

int main()
{
	mimi_cpu_t cpu = {};
	mimi_bus_t addr_bus = {};

	mimi_bus_device_t RAM = {};
	mimi_bus_device_t ROM = {};

	std::cout << "Hello, I am a test program for mimi!" << std::endl;

	int ret = mimi_cpu_init(&cpu, &impl_6502);
	std::cout << "CPU init returned " << ret << std::endl;

	ret = mimi_bus_init(&addr_bus);
	std::cout << "Bus init returned " << ret << std::endl;

	RAM.impl = &memory_bus_device_impl;
	RAM.size = 0x8000;

	ret = mimi_bus_map(
		&addr_bus,
		&RAM,
		0x0000
	);

	std::cout << "RAM map returned " << ret << std::endl;

	ROM.impl = &rom_bus_device_impl;
	ROM.size = 0x8000;

	ret = mimi_bus_map(
		&addr_bus,
		&ROM,
		0x8000
	);

	ret = mimi_rom_bus_device_special_write(ROM.private_data, 0x0, test_memory, sizeof(test_memory));

	ret = mimi_cpu_attach_bus(&cpu, &addr_bus, MIMI_6502_BUS_ADDRESS);
	std::cout << "CPU attach bus returned " << ret << std::endl;
	
	ret = mimi_cpu_tick(&cpu);
	std::cout << "CPU tick returned " << ret << std::endl;

	mimi_cpu_destroy(&cpu);

	mimi_bus_destroy(&addr_bus);

	std::cin.get();

	return 0;
}