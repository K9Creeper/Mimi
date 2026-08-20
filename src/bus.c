#include <bus.h>

#include <string.h>

int mimi_bus_init(mimi_bus_t* bus, const struct mimi_bus_impl_s* impl)
{
	if (!bus || !impl)
		return -1;

	memset(bus, 0, sizeof(mimi_bus_t));

	bus->impl = impl;

	if (!bus->impl->init)
		return -2;

	return bus->impl->init(&bus->private_data);
}

int mimi_bus_access(mimi_bus_t* bus, mimi_bus_request_t* request)
{
	if (!bus || !request)
		return -1;

	if (!bus->impl || !bus->impl->access)
		return -2;

	return bus->impl->access(bus->private_data, request);
}
