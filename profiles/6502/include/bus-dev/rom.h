#ifndef _6502_ROM_H
#define _6502_ROM_H

#include <bus.h>

#ifdef __cplusplus
extern "C" {
#endif

	extern const struct mimi_bus_device_impl_s rom_6502_bus_device_impl;
	
	extern mimi_err_t mimi_rom_bus_device_special_write(void* private_data, mimi_address_t address, const void* data, size_t size);

#ifdef __cplusplus
}
#endif

#endif