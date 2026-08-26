#include <bus-dev/rom.h>

#include <stdlib.h>
#include <string.h>

struct rom_device_s {
	uint8_t* raw_data;
};

static int init(void** pprivate_data, mimi_address_t size);
static int destroy(void** pprivate_data);
static int read(void* private_data, mimi_address_t address, void* data, size_t size);
static int write(void* private_data, mimi_address_t address, const void* data, size_t size);

const struct mimi_bus_device_impl_s rom_bus_device_impl = {
	.init = init,
	.destroy = destroy,
	.read = read,
	.write = write
};

int mimi_rom_bus_device_special_write(void* private_data, mimi_address_t address, const void* data, size_t size)
{
	if (!private_data || !data)
		return -1;

	struct rom_device_s* rom = private_data;

	if (!rom->raw_data)
		return -1;

	uint8_t* raw_dst = (uint8_t*)rom->raw_data + address;
	memcpy(raw_dst, data, size);

	return 0;
}

static int init(void** pprivate_data, mimi_address_t size)
{
	if (!pprivate_data || *pprivate_data)
		return -1;

	struct rom_device_s* mem = malloc(sizeof(struct rom_device_s));
	if (!mem)
		return -2;

	memset(mem, 0, sizeof(struct rom_device_s));

	uint8_t* raw_data = malloc(size);
	if (!raw_data) {
		free(mem);
		return -2;
	}

	memset(raw_data, 0, size);

	mem->raw_data = raw_data;
	*pprivate_data = mem;
	return 0;
}

static int destroy(void** pprivate_data)
{
	if (!pprivate_data)
		return -1;

	struct rom_device_s* rom = *pprivate_data;
	if (!rom)
		return 0;

	if (rom->raw_data) {
		free(rom->raw_data);
		rom->raw_data = NULL;
	}
	free(rom);

	*pprivate_data = NULL;
	return 0;
}

static int read(void* private_data, mimi_address_t address, void* data, size_t size)
{
	if (!private_data || !data)
		return -1;

	struct rom_device_s* rom = private_data;

	if (!rom->raw_data)
		return -1;

	uint8_t* raw_src = (uint8_t*)rom->raw_data + address;
	memcpy(data, raw_src, size);

	return 0;
}

static int write(void* private_data, mimi_address_t address, const void* data, size_t size)
{
	return 1;
}
