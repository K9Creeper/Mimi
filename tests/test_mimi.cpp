#include <iostream>

#include <mimi.h>

#include "../profiles/6502/export.h"

int main()
{
	mimi_cpu_t cpu = { 0 };
	mimi_bus_t bus = { 0 };

	std::cout << "Hello, I am a test program for mimi!" << std::endl;

	int ret = mimi_cpu_init(&cpu, &impl_6502);
	std::cout << "Returned with " << ret << std::endl;
	
	ret = mimi_bus_init(&bus, &impl_6502_bus);
	std::cout << "Returned with " << ret << std::endl;

	std::cin.get();

	return 0;
}