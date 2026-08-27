#include <bus.h>

#include <stdlib.h>
#include <string.h>

static int device_overlaps(struct mimi_bus_device_node_s* root, mimi_address_t base, mimi_address_t end);
static struct mimi_bus_device_node_s* device_remove_node(struct mimi_bus_device_node_s* root, mimi_bus_device_t* device, int* removed);
static struct mimi_bus_device_node_s* device_insert(struct mimi_bus_device_node_s* root, struct mimi_bus_device_node_s* node);
static int device_remove(mimi_bus_t* bus, mimi_bus_device_t* device);
static void device_destroy_tree(struct mimi_bus_device_node_s* root);

int mimi_bus_init(mimi_bus_t* bus)
{
	if (!bus)
		return -1;

	memset(bus, 0, sizeof(*bus));

	return 0;
}

int mimi_bus_destroy(mimi_bus_t* bus)
{
	if (!bus)
		return -1;

	device_destroy_tree(bus->root);

	memset(bus, 0, sizeof(*bus));

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

	if (device->is_mapped)
		return -1;

	mimi_address_t end = address + device->size;

	if (device_overlaps(bus->root, address, end))
		return -3;

	if (!device->impl->init)
		return -1;

	struct mimi_bus_device_node_s* node = malloc(sizeof(*node));

	if (!node)
		return -2;

	if (device->impl->init(
		&device->private_data,
		device->size
	)) {
		free(node);
		return -4;
	}

	device->base = address;
	device->is_mapped = 1;

	node->device = device;
	node->left = NULL;
	node->right = NULL;

	bus->root = device_insert(bus->root, node);

	return 0;
}

int mimi_bus_unmap(mimi_bus_t* bus, mimi_bus_device_t* device)
{
	if (!bus || !device)
		return -1;

	if (!device->is_mapped)
		return -1;

	int ret = device_remove(bus, device);

	if (ret)
		return ret;

	if (device->impl && device->impl->destroy) {
		device->impl->destroy(&device->private_data);
	}

	device->base = 0;
	device->is_mapped = 0;

	return 0;
}

int mimi_bus_access(mimi_bus_t* bus, const mimi_bus_request_t* request)
{
	if (!bus || !request)
		return -1;

	struct mimi_bus_device_node_s* node = bus->root;
	mimi_bus_device_t* device = NULL;

	while (node) {
		if (request->address < node->device->base) {
			node = node->left;
		}
		else {
			device = node->device;
			node = node->right;
		}
	}

	if (!device)
		return 1;

	mimi_address_t offset =
		request->address - device->base;

	if (offset >= device->size)
		return 1;

	if (request->size > device->size - offset)
		return 1;

	switch (request->access) {
	case MIMI_BUS_READ:
		if (!device->impl->read)
			return -1;

		return device->impl->read(
			device->private_data,
			offset,
			request->data.read,
			request->size
		);

	case MIMI_BUS_WRITE:
		if (!device->impl->write)
			return -1;

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

static int device_overlaps(struct mimi_bus_device_node_s* root, mimi_address_t base, mimi_address_t end)
{
	struct mimi_bus_device_node_s* node = root;

	while (node) {
		mimi_bus_device_t* device = node->device;

		mimi_address_t device_end = device->base + device->size;

		if (end <= device->base) {
			node = node->left;
			continue;
		}

		if (base >= device_end) {
			node = node->right;
			continue;
		}

		return 1;
	}

	return 0;
}


static struct mimi_bus_device_node_s* device_remove_node(struct mimi_bus_device_node_s* root, mimi_bus_device_t* device, int* removed)
{
	if (!root)
		return NULL;

	if (device->base < root->device->base) {
		root->left = device_remove_node(
			root->left,
			device,
			removed
		);

		return root;
	}

	if (device->base > root->device->base) {
		root->right = device_remove_node(
			root->right,
			device,
			removed
		);

		return root;
	}

	if (root->device != device)
		return root;

	*removed = 1;

	if (!root->left) {
		struct mimi_bus_device_node_s* right = root->right;

		free(root);

		return right;
	}

	if (!root->right) {
		struct mimi_bus_device_node_s* left = root->left;

		free(root);

		return left;
	}

	struct mimi_bus_device_node_s* successor =
		root->right;

	while (successor->left)
		successor = successor->left;

	root->device = successor->device;

	root->right = device_remove_node(
		root->right,
		successor->device,
		removed
	);

	return root;
}

static struct mimi_bus_device_node_s* device_insert(struct mimi_bus_device_node_s* root, struct mimi_bus_device_node_s* node)
{
	if (!root)
		return node;

	if (node->device->base < root->device->base) {
		root->left = device_insert(root->left, node);
	}
	else {
		root->right = device_insert(root->right, node);
	}

	return root;
}

static int device_remove(mimi_bus_t* bus, mimi_bus_device_t* device)
{
	if (!bus || !device)
		return -1;

	int removed = 0;

	bus->root = device_remove_node(
		bus->root,
		device,
		&removed
	);

	return removed ? 0 : 1;
}

static void device_destroy_tree(struct mimi_bus_device_node_s* root)
{
	if (!root)
		return;

	device_destroy_tree(root->left);
	device_destroy_tree(root->right);

	mimi_bus_device_t* device = root->device;

	if (device && device->impl && device->impl->destroy) {
		device->impl->destroy(&device->private_data);
	}

	free(root);
}
