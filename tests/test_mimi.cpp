#include <iostream>
#include <cstdint>

#include <mimi.h>

#include "../profiles/6502/include/export.h"

// Program at $8000:
//	 NOP			; EA
//	 NOP			; EA
//   LDA #$42       ; A9 42
//   STA $0200      ; 8D 00 02
//   JMP $8000      ; 4C 00 80
//
static const uint8_t test_program[] = {
	0xEA, 
	0xEA,
	0xA9, 0x42,
	0x8D, 0x00, 0x02,
	0x4C, 0x00, 0x80
};

// Reset vector at $FFFC/$FFFD:
//
//   $FFFC = 00
//   $FFFD = 80
//
// Which means: reset -> $8000
//
static const uint8_t reset_vector[] = {
	0x00, 0x80
};

int main()
{
	mimi_cpu_t cpu = {};
	mimi_bus_t bus = {};

	mimi_bus_device_t RAM = {};
	mimi_bus_device_t ROM = {};

	int ret;

	std::cout << "Hello, I am a test program for mimi!" << std::endl;

	ret = mimi_cpu_init(&cpu, &impl_6502);
	std::cout << "CPU init returned " << ret << std::endl;

	ret = mimi_bus_init(&bus);
	std::cout << "Bus init returned " << ret << std::endl;

	// $0000-$7FFF
	RAM.impl = &memory_6502_bus_device_impl;
	RAM.size = 0x8000;

	ret = mimi_bus_map(&bus, &RAM, 0x0000);
	std::cout << "RAM map returned " << ret << std::endl;

	// $8000-$FFFF
	ROM.impl = &rom_6502_bus_device_impl;
	ROM.size = 0x8000;

	ret = mimi_bus_map(&bus, &ROM, 0x8000);
	std::cout << "ROM map returned " << ret << std::endl;

	// Load program at $8000.
	ret = mimi_rom_bus_device_special_write(
		ROM.private_data,
		0x0000,                 // $8000 - $8000
		test_program,
		sizeof(test_program)
	);

	std::cout << "Program load returned " << ret << std::endl;

	// Load reset vector at $FFFC.
	//
	// ROM offset = $FFFC - $8000 = $7FFC
	//
	ret = mimi_rom_bus_device_special_write(
		ROM.private_data,
		0x7FFC,
		reset_vector,
		sizeof(reset_vector)
	);

	std::cout << "Reset vector load returned " << ret << std::endl;

	ret = mimi_cpu_attach_bus(
		&cpu,
		&bus,
		MIMI_6502_BUS
	);

	std::cout << "CPU attach bus returned " << ret << std::endl;

	for (;;) {
		ret = mimi_cpu_tick(&cpu);

		if (ret) {
			std::cout << "Failed with " << ret << std::endl;
			break;
		}
	}

	mimi_cpu_destroy(&cpu);
	mimi_bus_destroy(&bus);

	std::cin.get();

	return 0;
}