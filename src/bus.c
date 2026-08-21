#include <bus.h>

#include <stdlib.h>
#include <string.h>

static inline struct mimi_bus_device_list_s* device_insert(struct mimi_bus_device_list_s* root, struct mimi_bus_device_list_s* device);
static inline int device_remove(mimi_bus_t* bus, mimi_bus_device_t* device);

int mimi_bus_init(mimi_bus_t* bus)
{
	if (!bus)
		return -1;

	memset(bus, 0, sizeof(mimi_bus_t));

	return 0;
}

int mimi_bus_map(mimi_bus_t* bus, mimi_bus_device_t* device, mimi_address_t address)
{
	if (!bus || !device || !device->impl)
		return -1;

	if (!device->size)
		return -1;

	if (address > UINTPTR_MAX - device->size)
		return -1;

	mimi_address_t end = address + device->size;

	for (struct mimi_bus_device_list_s* list = bus->root; list; list = list->next) {
		mimi_bus_device_t* mapped = list->device;
		mimi_address_t mapped_end = mapped->base + mapped->size;

		if (end <= mapped->base)
			break;

		if (address >= mapped_end)
			continue;

		return -2;
	}

	device->base = address;

	struct mimi_bus_device_list_s* node = malloc(sizeof(*node));

	if (!node)
		return -1;

	node->device = device;
	node->next = NULL;

	bus->root = device_insert(bus->root, node);

	return 0;
}

int mimi_bus_access(mimi_bus_t* bus, const mimi_bus_request_t* request)
{
	if (!bus || !request)
		return -1;

	for (struct mimi_bus_device_list_s* list = bus->root; list; list = list->next) {
		mimi_bus_device_t* device = list->device;

		if (request->address < device->base)
			break;

		mimi_address_t offset = request->address - device->base;

		if (offset > device->size)
			continue;

		if (request->size > device->size - offset)
			continue;

		switch (request->access) {
		case MIMI_BUS_READ:
			return device->impl->read(
				device->private_data,
				offset,
				request->data.read,
				request->size
			);

		case MIMI_BUS_WRITE:
			return device->impl->write(
				device->private_data,
				offset,
				request->data.write,
				request->size
			);

		default:
			return -1;
		}
	}

	return 1;
}

static inline struct mimi_bus_device_list_s* device_insert(struct mimi_bus_device_list_s* root, struct mimi_bus_device_list_s* device)
{
	if (!root || device->device->base < root->device->base) {
		device->next = root;
		return device;
	}

	root->next = device_insert(root->next, device);

	return root;
}

static inline int device_remove(mimi_bus_t* bus, mimi_bus_device_t* device)
{
	if (!bus || !device)
		return -1;

	struct mimi_bus_device_list_s** link = &bus->root;

	while (*link) {
		struct mimi_bus_device_list_s* current = *link;

		if (current->device == device) {
			*link = current->next;
			free(current);
			return 0;
		}

		link = &current->next;
	}

	return 1;
}
