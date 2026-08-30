#include <bus-dev/mem.h>

#include <stdlib.h>
#include <string.h>

struct mem_device_s {
	int err;
	uint8_t* raw_data;
};

static int init(struct mem_device_s** pmem, mimi_address_t size);
static int destroy(struct mem_device_s** pmem);
static int read(struct mem_device_s* mem, mimi_address_t address, void* data, size_t size);
static int write(struct mem_device_s* mem, mimi_address_t address, const void* data, size_t size);

const struct mimi_bus_device_impl_s memory_6502_bus_device_impl = {
	.init = init,
	.destroy = destroy,
	.read = read,
	.write = write
};

static int init(struct mem_device_s** pmem, mimi_address_t size)
{
	if (!pmem)
		return -1;

	if(*pmem)
		return 1;

	struct mem_device_s* mem = malloc(sizeof(struct mem_device_s));
	if (!mem)
		return 2;

	memset(mem, 0, sizeof(struct mem_device_s));

	uint8_t* raw_data = malloc(size);
	if (!raw_data) {
		free(mem);
		return 2;
	}
	
	memset(raw_data, 0, size);

	mem->raw_data = raw_data;
	*pmem = mem;
	return 0;
}

static int destroy(struct mem_device_s** pmem)
{
	if (!pmem)
		return -1;

	struct mem_device_s* mem = *pmem;
	if (!mem)
		return 0;

	if (mem->raw_data) {
		free(mem->raw_data);
		mem->raw_data = NULL;
	}
	free(mem);

	*pmem = NULL;
	return 0;
}

static int read(struct mem_device_s* mem, mimi_address_t address, void* data, size_t size)
{
	if (!mem)
		return -1;

	if (!data)
		return 1;

	if (!mem->raw_data)
		return 2;

	uint8_t* raw_src = (uint8_t*)mem->raw_data + address;
	memcpy(data, raw_src, size);

	return 0;
}

static int write(struct mem_device_s* mem, mimi_address_t address, const void* data, size_t size)
{
	if (!mem)
		return -1;

	if (!data)
		return 1;

	if (!mem->raw_data)
		return 2;

	uint8_t* raw_dst = (uint8_t*)mem->raw_data + address;
	memcpy(raw_dst, data, size);

	return 0;
}
