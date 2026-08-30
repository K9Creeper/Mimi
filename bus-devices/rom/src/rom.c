#include <rom.h>

#include <stdlib.h>
#include <string.h>

struct rom_device_s {
	int err;
	uint8_t* raw_data;
};

static int init(struct rom_device_s** prom, mimi_address_t size);
static int destroy(struct rom_device_s** prom);
static int read(struct rom_device_s* rom, mimi_address_t address, void* data, size_t size);
static int write(struct rom_device_s* rom, mimi_address_t address, const void* data, size_t size);

const struct mimi_bus_device_impl_s rom_bus_device_impl = {
	.init = init,
	.destroy = destroy,
	.read = read,
	.write = write
};

mimi_err_t mimi_rom_bus_device_special_write(void* private_data, mimi_address_t address, const void* data, size_t size)
{
	if (!private_data || !data)
		return MIMI_ERR_BAD_ARG;

	struct rom_device_s* rom = private_data;

	if (!rom->raw_data)
		return MIMI_ERR_GENERIC;

	uint8_t* raw_dst = (uint8_t*)rom->raw_data + address;
	memcpy(raw_dst, data, size);

	return MIMI_OK;
}

static int init(struct rom_device_s** prom, mimi_address_t size)
{
	if (!prom)
		return -1;

	if (*prom)
		return 1;

	struct rom_device_s* rom = malloc(sizeof(struct rom_device_s));
	if (!rom)
		return 2;

	memset(rom, 0, sizeof(struct rom_device_s));

	uint8_t* raw_data = malloc(size);
	if (!raw_data) {
		free(rom);
		return 2;
	}

	memset(raw_data, 0, size);

	rom->raw_data = raw_data;
	*prom = rom;
	return 0;
}

static int destroy(struct rom_device_s** prom)
{
	if (!prom)
		return -1;

	struct rom_device_s* rom = *prom;
	if (!rom)
		return 0;

	if (rom->raw_data) {
		free(rom->raw_data);
		rom->raw_data = NULL;
	}
	free(rom);

	*prom = NULL;
	return 0;
}

static int read(struct rom_device_s* rom, mimi_address_t address, void* data, size_t size)
{
	if (!rom)
		return -1;

	if(!data)
		return 1;

	if (!rom->raw_data)
		return 2;

	uint8_t* raw_src = (uint8_t*)rom->raw_data + address;
	memcpy(data, raw_src, size);

	return 0;
}

static int write(struct rom_device_s* rom, mimi_address_t address, const void* data, size_t size)
{
	return -1;
}
