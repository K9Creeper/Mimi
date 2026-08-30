#include <bus.h>

#include <stdlib.h>
#include <string.h>

static int device_overlaps(struct mimi_bus_device_node_s* root, mimi_address_t base, mimi_address_t end);
static struct mimi_bus_device_node_s* device_remove_node(struct mimi_bus_device_node_s* root, mimi_bus_device_t* device, int* removed);
static struct mimi_bus_device_node_s* device_insert(struct mimi_bus_device_node_s* root, struct mimi_bus_device_node_s* node);
static int device_remove(mimi_bus_t* bus, mimi_bus_device_t* device);
static void device_destroy_tree(struct mimi_bus_device_node_s* root);

mimi_err_t mimi_bus_init(mimi_bus_t* bus)
{
	if (!bus)
		return MIMI_ERR_BAD_ARG;

	memset(bus, 0, sizeof(mimi_bus_t));

	return MIMI_OK;
}

mimi_err_t mimi_bus_destroy(mimi_bus_t* bus)
{
	if (!bus)
		return MIMI_ERR_BAD_ARG;

	device_destroy_tree(bus->root);

	memset(bus, 0, sizeof(mimi_bus_t));

	return MIMI_OK;
}

mimi_err_t mimi_bus_map(mimi_bus_t* bus, mimi_bus_device_t* device, mimi_address_t address)
{
	if (!bus || !device)
		return MIMI_ERR_BAD_ARG;

	if (!device->impl)
		return MIMI_ERR_UNSUPPORTED;

	if (!device->size)
		return MIMI_ERR_GENERIC;

	if (address > UINTPTR_MAX - device->size)
		return MIMI_ERR_BAD_ARG;

	if (device->is_mapped)
		return MIMI_ERR_GENERIC;

	mimi_address_t end = address + device->size;

	if (device_overlaps(bus->root, address, end))
		return MIMI_ERR_GENERIC;

	if (!device->impl->init)
		return MIMI_ERR_UNSUPPORTED;

	struct mimi_bus_device_node_s* node = malloc(sizeof(*node));

	if (!node)
		return MIMI_ERR_NO_MEMORY;

	int err = device->impl->init(&device->private_data, device->size);
	if (err) {
		free(node);
		return (err < 0) ? MIMI_ERR_BUS_DEV : err;
	}

	device->base = address;
	device->is_mapped = 1;

	node->device = device;
	node->left = NULL;
	node->right = NULL;

	bus->root = device_insert(bus->root, node);

	return MIMI_OK;
}

mimi_err_t mimi_bus_unmap(mimi_bus_t* bus, mimi_bus_device_t* device)
{
	if (!bus || !device)
		return MIMI_OK;

	if (!device->is_mapped)
		return MIMI_ERR_GENERIC;

	if (device_remove(bus, device))
		return MIMI_ERR_GENERIC;

	if (device->impl && device->impl->destroy) {
		device->impl->destroy(&device->private_data);
	}

	device->base = 0;
	device->is_mapped = 0;

	return MIMI_OK;
}

mimi_err_t mimi_bus_access(mimi_bus_t* bus, const mimi_bus_request_t* request)
{
	if (!bus || !request)
		return MIMI_OK;

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
		return MIMI_ERR_NOT_FOUND;

	mimi_address_t offset =	request->address - device->base;

	if (offset >= device->size)
		return MIMI_ERR_GENERIC;

	if (request->size > device->size - offset)
		return MIMI_ERR_GENERIC;

	switch (request->access) {
		case MIMI_BUS_READ:
		{
			if (!device->impl->read)
				return MIMI_ERR_UNSUPPORTED;

			int err = device->impl->read(device->private_data, offset, request->data.read, request->size);
			if (err) {
				return (err < 0) ? MIMI_ERR_BUS_DEV : err;
			}

			return MIMI_OK;
			break;
		}
		case MIMI_BUS_WRITE:
		{
			if (!device->impl->write)
				return MIMI_ERR_UNSUPPORTED;

			int err = device->impl->write(device->private_data, offset, request->data.write, request->size);
			if (err) {
				return (err < 0) ? MIMI_ERR_BUS_DEV : err;
			}

			return MIMI_OK;
			break;
		}
		default:
			return MIMI_ERR_BAD_ARG;
	}

	return MIMI_ERR_GENERIC;
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
