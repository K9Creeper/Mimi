#include <bus-dev/mem.h>

#include <stdlib.h>
#include <string.h>

struct mem_device_s {
	void* raw_data;
};

static int init(void** pprivate_data, mimi_address_t size);
static void destroy(void** pprivate_data);
static int read(void* private_data, mimi_address_t address, void* data, size_t size);
static int write(void* private_data, mimi_address_t address, const void* data, size_t size);

const struct mimi_bus_device_impl_s memory_bus_device_impl = {
	.init = init,
	.destroy = destroy,
	.read = read,
	.write = write
};

static int init(void** pprivate_data, mimi_address_t size)
{
	if (!pprivate_data || *pprivate_data)
		return -1;

	struct mem_device_s* mem = malloc(sizeof(struct mem_device_s));
	if (!mem)
		return -2;

	memset(mem, 0, sizeof(struct mem_device_s));

	void* raw_data = malloc(size);
	if (!raw_data) {
		free(mem);
		return -2;
	}
	
	memset(raw_data, 0, size);

	mem->raw_data = raw_data;
	*pprivate_data = mem;
	return 0;
}

static void destroy(void** pprivate_data)
{
	if (!pprivate_data)
		return -1;

	struct mem_device_s* mem = *pprivate_data;
	if (!mem)
		return 0;

	if (mem->raw_data) {
		free(mem->raw_data);
		mem->raw_data = NULL;
	}
	free(mem);

	*pprivate_data = NULL;
	return 0;
}

static int read(void* private_data, mimi_address_t address, void* data, size_t size)
{
	if (!private_data || !data)
		return -1;

	struct mem_device_s* mem = private_data;

	if (!mem->raw_data)
		return -1;

	uint8_t* raw_src = (uint8_t*)mem->raw_data + address;
	memcpy(data, raw_src, size);

	return 0;
}

static int write(void* private_data, mimi_address_t address, const void* data, size_t size)
{
	if (!private_data || !data)
		return -1;

	struct mem_device_s* mem = private_data;

	if (!mem->raw_data)
		return -1;

	uint8_t* raw_dst = (uint8_t*)mem->raw_data + address;
	memcpy(raw_dst, data, size);

	return 0;
}
