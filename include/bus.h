#ifndef BUS_H
#define BUS_H

#include <mimi.h>

typedef uint32_t mimi_bus_role_id_t;

typedef enum mimi_bus_access_e {
	MIMI_BUS_READ = 0,
	MIMI_BUS_WRITE,
} mimi_bus_access_t;

struct mimi_bus_device_impl_s {
	int (*init)(void** pprivate_data, mimi_address_t size);
	int (*destroy)(void** pprivate_data);

	int (*read)(void* private_data, mimi_address_t address, void* data, size_t size);
	int (*write)(void* private_data, mimi_address_t address, const void* data, size_t size);
};

typedef struct mimi_bus_device_s {
	const struct mimi_bus_device_impl_s* impl;
	void* private_data;

	mimi_address_t base;
	mimi_address_t size;

	uint8_t is_mapped;
} mimi_bus_device_t;

typedef struct mimi_bus_request_s {
	mimi_bus_access_t access;
	mimi_address_t address;

	union {
		void* read;
		const void* write;
	} data;

	mimi_address_t size;
} mimi_bus_request_t;

struct mimi_bus_device_node_s {
	mimi_bus_device_t* device;

	struct mimi_bus_device_node_s* left;
	struct mimi_bus_device_node_s* right;
};

typedef struct mimi_bus_s {
	struct mimi_bus_device_node_s* root;
} mimi_bus_t;

#ifdef __cplusplus
extern "C" {
#endif

	extern int mimi_bus_init(mimi_bus_t* bus);

	extern int mimi_bus_destroy(mimi_bus_t* bus);

	extern int mimi_bus_map(mimi_bus_t* bus,mimi_bus_device_t* device,mimi_address_t address);

	extern int mimi_bus_unmap(mimi_bus_t* bus,mimi_bus_device_t* device);
	
	extern int mimi_bus_access(mimi_bus_t* bus,const mimi_bus_request_t* request);
	
#ifdef __cplusplus
}
#endif

#endif
