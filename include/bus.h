#ifndef BUS_H
#define BUS_H

#include <stdint.h>

struct mimi_bus_s;

typedef uintptr_t mimi_address_t;

typedef enum mimi_bus_access_e {
	MIMI_BUS_READ = 0,
	MIMI_BUS_WRITE,
} mimi_bus_access_t;

typedef struct mimi_bus_request_s {
	mimi_bus_access_t access;

	mimi_address_t address;

	union {
		void* read;
		const void* write;
	} data;

	size_t size;
} mimi_bus_request_t;

struct mimi_bus_impl_s {
	int (*init)(void** pprivate_data);
	int (*access)(void* private_data, mimi_bus_request_t* request);
};

typedef struct mimi_bus_s {
	const struct mimi_bus_impl_s* impl;
	void* private_data;
} mimi_bus_t;

#ifdef __cplusplus
extern "C" {
#endif

	extern int mimi_bus_init(mimi_bus_t* bus, const struct mimi_bus_impl_s* impl);

	extern int mimi_bus_access(mimi_bus_t* bus, mimi_bus_request_t* request);

#ifdef __cplusplus
}
#endif

#endif