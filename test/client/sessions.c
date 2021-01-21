#include <stdlib.h>
#include <stdio.h>

#include <cerver/client.h>
#include <cerver/packets.h>
#include <cerver/sessions.h>

#include <cerver/threads/thread.h>

#include <app/app.h>
#include <app/auth.h>

#include "client.h"

#include "../test.h"

#define MAIN_REQUESTS			64
#define SECOND_REQUESTS			32

static const char *client_name = "test-client";
static const char *main_connection_name = "main-connection";
static const char *second_connection_name = "second-connection";

static Client *client = NULL;

static pthread_mutex_t *responses_lock = NULL; 
static unsigned int responses = 0;

static void app_handler (void *packet_ptr) {

	if (packet_ptr) {
		Packet *packet = (Packet *) packet_ptr;

		switch (packet->header.request_type) {
			case APP_REQUEST_NONE: break;

			case APP_REQUEST_TEST: {
				(void) pthread_mutex_lock (responses_lock);
				responses += 1;
				(void) pthread_mutex_unlock (responses_lock);
			} break;

			case APP_REQUEST_MESSAGE: break;

			case APP_REQUEST_MULTI: break;

			default: break;
		}
	}

}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// use the received session id to create a new connection
static void *connect_with_session (void *args) {

	Connection *connection = client_connection_create (
		client, "127.0.0.1", 7000, PROTOCOL_TCP, false
	);

	test_check_ptr (connection);

	connection_set_name (connection, second_connection_name);
	connection_set_max_sleep (connection, 30);

	// set the received session id as the connection's auth data
	test_check_ptr (client->session_id);

	SToken *s_token = (SToken *) malloc (sizeof (SToken));
	strncpy (s_token->token, client->session_id->str, TOKEN_SIZE);

	connection_set_auth_data (
		connection, 
		s_token, sizeof (SToken), 
		NULL,
		false
	);

	/*** start ***/
	test_check_int_eq (
		client_connect_and_start (client, connection), 0,
		"Failed to connect to cerver!"
	);

	// wait to be authenticated with cerver
	(void) sleep (3);

	test_check_bool_eq (
		connection->authenticated, true, "Failed to authenticate!"
	);

	/*** send ***/
	// send a bunch of requests to the cerver
	Packet *request = NULL;
	for (unsigned int i = 0; i < SECOND_REQUESTS; i++) {
		request = packet_create_request (
			PACKET_TYPE_APP, APP_REQUEST_TEST
		);

		if (request) {
			packet_set_network_values (
				request,
				NULL, client, connection, NULL
			);

			test_check_unsigned_eq (
				packet_send (request, 0, NULL, false),
				0, NULL
			);

			packet_delete (request);
		}
	}

	return NULL;

}

#pragma GCC diagnostic pop

int main (int argc, const char **argv) {

	(void) printf ("Testing CLIENT sessions...\n");

	responses_lock = pthread_mutex_new ();

	client = client_create ();

	test_check_ptr (client);

	client_set_name (client, client_name);
	test_check_str_eq (client->name, client_name, NULL);
	test_check_str_len (client->name, strlen (client_name), NULL);

	/*** handler ***/
	Handler *app_packet_handler = handler_create (app_handler);
	handler_set_direct_handle (app_packet_handler, true);
	client_set_app_handlers (client, app_packet_handler, NULL);

	/*** events ***/
	u8 event_result = 0;

	event_result = client_event_register (
		client,
		CLIENT_EVENT_CONNECTION_CLOSE,
		client_event_connection_close, NULL, NULL,
		false, false
	);

	test_check_unsigned_eq (event_result, 0, NULL);

	event_result = client_event_register (
		client,
		CLIENT_EVENT_AUTH_SENT,
		client_event_auth_sent, NULL, NULL,
		true, false
	);

	test_check_unsigned_eq (event_result, 0, NULL);

	event_result = client_event_register (
		client,
		CLIENT_EVENT_SUCCESS_AUTH,
		client_event_success_auth, NULL, NULL,
		false, false
	);

	test_check_unsigned_eq (event_result, 0, NULL);

	/*** errors ***/
	u8 error_result = 0;

	error_result = client_error_register (
		client,
		CLIENT_ERROR_FAILED_AUTH,
		client_error_failed_auth, NULL, NULL,
		false, false
	);

	test_check_unsigned_eq (error_result, 0, NULL);

	/*** connection ***/
	Connection *connection = client_connection_create (
		client, "127.0.0.1", 7000, PROTOCOL_TCP, false
	);

	test_check_ptr (connection);

	connection_set_name (connection, main_connection_name);
	connection_set_max_sleep (connection, 30);

	Credentials *credentials = credentials_new (
		"ermiry", "049ec1af7c1332193d602986f2fdad5b4d1c2ff90e5cdc65388c794c1f10226b"
	);

	connection_set_auth_data (
		connection, 
		credentials, sizeof (Credentials), 
		credentials_delete,
		false
	);

	/*** start ***/
	test_check_int_eq (
		client_connect_and_start (client, connection), 0,
		"Failed to connect to cerver!"
	);

	// wait to be authenticated with cerver
	(void) sleep (3);

	test_check_bool_eq (
		connection->authenticated, true, "Failed to authenticate!"
	);

	/*** session ***/
	pthread_t thread_id = 0;
	test_check_int_eq (
		pthread_create (&thread_id, NULL, connect_with_session, NULL),
		0, "Failed to create sessions thread!"
	);

	/*** send ***/
	// send a bunch of requests to the cerver
	Packet *request = NULL;
	for (unsigned int i = 0; i < MAIN_REQUESTS; i++) {
		request = packet_create_request (
			PACKET_TYPE_APP, APP_REQUEST_TEST
		);

		if (request) {
			packet_set_network_values (
				request,
				NULL, client, connection, NULL
			);

			test_check_unsigned_eq (
				packet_send (request, 0, NULL, false),
				0, NULL
			);

			packet_delete (request);
		}
	}

	// wait for sessions thread to finish
	(void) pthread_join (thread_id, NULL);

	// wait for any missing response
	(void) sleep (5);

	/*** check ***/
	// check that we have received all the responses
	test_check_unsigned_eq (
		responses, MAIN_REQUESTS + SECOND_REQUESTS, NULL
	);

	/*** end ***/
	client_connection_end (client, connection);
	client_teardown (client);

	pthread_mutex_delete (responses_lock);

	(void) printf ("Done!\n\n");

	return 0;

}