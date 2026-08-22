#include <iostream>

#include <mimi.h>

#include <bus-dev/mem.h>

#include "../profiles/6502/export.h"

static void test_bus_access(mimi_bus_t* bus)
{
	std::cout << "\nTesting bus access...\n";

	uint8_t write_value = 0x42;
	uint8_t read_value = 0x00;

	/*
	 * Write 0x42 to address 0x1234.
	 */
	mimi_bus_request_t request = {};

	request.access = MIMI_BUS_WRITE;
	request.address = 0x1234;
	request.data.write = &write_value;
	request.size = sizeof(write_value);

	int ret = mimi_bus_access(bus, &request);

	std::cout << "Write 0x42 @ 0x1234: "
		<< "ret=" << ret << '\n';

	/*
	 * Read it back.
	 */
	request.access = MIMI_BUS_READ;
	request.address = 0x1234;
	request.data.read = &read_value;

	ret = mimi_bus_access(bus, &request);

	std::cout << "Read @ 0x1234: "
		<< "ret=" << ret
		<< ", value=0x"
		<< std::hex
		<< static_cast<int>(read_value)
		<< std::dec
		<< '\n';

	/*
	 * Verify the value.
	 */
	if (ret == 0 && read_value == write_value)
		std::cout << "PASS: read/write test\n";
	else
		std::cout << "FAIL: read/write test\n";

	/*
	 * Test another address to make sure offsets work.
	 */
	write_value = 0xAB;

	request.access = MIMI_BUS_WRITE;
	request.address = 0x7FFF;
	request.data.write = &write_value;
	request.size = sizeof(write_value);

	ret = mimi_bus_access(bus, &request);

	std::cout << "Write 0xAB @ 0x7FFF: "
		<< "ret=" << ret << '\n';

	read_value = 0x00;

	request.access = MIMI_BUS_READ;
	request.data.read = &read_value;

	ret = mimi_bus_access(bus, &request);

	std::cout << "Read @ 0x7FFF: "
		<< "ret=" << ret
		<< ", value=0x"
		<< std::hex
		<< static_cast<int>(read_value)
		<< std::dec
		<< '\n';

	if (ret == 0 && read_value == write_value)
		std::cout << "PASS: boundary test\n";
	else
		std::cout << "FAIL: boundary test\n";

	/*
	 * Address 0x8000 should be ROM in the example mapping.
	 */
	write_value = 0x55;

	request.access = MIMI_BUS_WRITE;
	request.address = 0x8000;
	request.data.write = &write_value;
	request.size = sizeof(write_value);

	ret = mimi_bus_access(bus, &request);

	std::cout << "Write @ 0x8000: "
		<< "ret=" << ret << '\n';

	/*
	 * Address 0x10000 is outside our 64 KiB example.
	 */
	request.access = MIMI_BUS_READ;
	request.address = 0x10000;
	request.data.read = &read_value;
	request.size = sizeof(read_value);

	ret = mimi_bus_access(bus, &request);

	std::cout << "Read @ 0x10000: "
		<< "ret=" << ret << '\n';
}


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

	/*
	 * 6502 address space:
	 *
	 * 0x0000 - 0x7FFF : 32 KiB RAM
	 * 0x8000 - 0xFFFF : 32 KiB ROM
	 */

	RAM.impl = &memory_bus_device_impl;
	RAM.size = 0x8000;

	ret = mimi_bus_map(
		&addr_bus,
		&RAM,
		0x0000
	);

	std::cout << "RAM map returned " << ret << std::endl;

	ROM.impl = &memory_bus_device_impl;
	ROM.size = 0x8000;

	ret = mimi_bus_map(
		&addr_bus,
		&ROM,
		0x8000
	);

	std::cout << "ROM map returned " << ret << std::endl;

	test_bus_access(&addr_bus);

	std::cin.get();

	return 0;
}