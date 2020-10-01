#ifndef _CERVER_BALANCER_H_
#define _CERVER_BALANCER_H_

#include "cerver/types/types.h"
#include "cerver/types/string.h"

#include "cerver/cerver.h"
#include "cerver/client.h"
#include "cerver/connection.h"
#include "cerver/config.h"

#include "cerver/threads/thread.h"

struct _Service;

#pragma region type

#define BALANCER_TYPE_MAP(XX)									\
	XX(0, 	NONE, 				None)							\
	XX(1, 	ROUND_ROBIN, 		Round-Robin)					\

typedef enum BalancerType {

	#define XX(num, name, string) BALANCER_TYPE_##name = num,
	BALANCER_TYPE_MAP (XX)
	#undef XX

} BalancerType;

const char *balancer_type_to_string (BalancerType type);

#pragma endregion

#pragma region main

struct _Balancer {

	String *name;
	BalancerType type;

	Cerver *cerver;
	Client *client;

	int next_service;
	int n_services;					// how many services the load balancer is connected to
	struct _Service **services;		// references to the client's connections for direct access

	pthread_mutex_t *mutex;

};

typedef struct _Balancer Balancer;

CERVER_PRIVATE Balancer *balancer_new (void);

CERVER_PRIVATE void balancer_delete (void *balancer_ptr);

// create a new load balancer of the selected type
// set its network values & set the number of services it will handle
CERVER_EXPORT Balancer *balancer_create (
	const char *name, BalancerType type,
	u16 port, u16 connection_queue,
	unsigned int n_services
);

#pragma endregion

#pragma region services

#define DEFAULT_SERVICE_WAIT_TIME			20

#define SERVICE_STATUS_MAP(XX)																			\
	XX(0, 	NONE, 			None, 			None)														\
	XX(1, 	CONNECTING, 	Connecting, 	Creating connection with service)							\
	XX(2, 	READY, 			Ready, 			Ready to start accepting packets)							\
	XX(3, 	WORKING, 		Working, 		Handling packets)											\
	XX(4, 	DISCONNECTING, 	Disconnecting, 	In the process of closing the connection)					\
	XX(5, 	DISCONNECTED, 	Disconnected, 	The balancer has been disconnected from the service)		\
	XX(6, 	UNAVAILABLE, 	Unavailable, 	The service is down or is not handling packets)

typedef enum ServiceStatus {

	#define XX(num, name, string, description) SERVICE_STATUS_##name = num,
	SERVICE_STATUS_MAP (XX)
	#undef XX

} ServiceStatus;

CERVER_EXPORT const char *balancer_service_status_to_string (ServiceStatus status);

CERVER_EXPORT const char *balancer_service_status_description (ServiceStatus status);

struct _Service {

	ServiceStatus status;
	Connection *connection;

	unsigned int reconnect_wait_time;

};

typedef struct _Service Service;

// registers a new service to the load balancer
// a dedicated connection will be created when the balancer starts to handle traffic to & from the service
// returns 0 on success, 1 on error
CERVER_EXPORT u8 balancer_service_register (
	Balancer *balancer,
	const char *ip_address, u16 port
);

// sets the time (in secs) to wait to attempt a reconnection whenever the service disconnects
// the default value is 20 secs
CERVER_EXPORT void balancer_service_set_reconnect_wait_time (Service *service, unsigned int wait_time);

#pragma endregion

#pragma region start

// starts the load balancer by first connecting to the registered services
// and checking their ability to handle requests
// then the cerver gets started to enable client connections
// returns 0 on success, 1 on error
CERVER_EXPORT u8 balancer_start (Balancer *balancer);

#pragma endregion

#pragma region route

// routes the received packet to a service to be handled
// first sends the packet header with the correct sock fd
// if any data, it is forwarded from one sock fd to another using splice ()
CERVER_PRIVATE void balancer_route_to_service (
	Balancer *balancer, Connection *connection,
	PacketHeader *header
);

#pragma endregion

#pragma region end

// first ends and destroys the balancer's internal cerver
// then disconnects from each of the registered services
// last frees any balancer memory left
// returns 0 on success, 1 on error
CERVER_EXPORT u8 balancer_teardown (Balancer *balancer);

#pragma endregion

#endif