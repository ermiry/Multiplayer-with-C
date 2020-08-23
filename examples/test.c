#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <time.h>
#include <signal.h>

#include <cerver/version.h>
#include <cerver/cerver.h>
#include <cerver/events.h>

#include <cerver/utils/log.h>
#include <cerver/utils/utils.h>

typedef enum AppRequest {

	TEST_MSG		= 0

} AppRequest;

static Cerver *my_cerver = NULL;

#pragma region end

// correctly closes any on-going server and process when quitting the appplication
static void end (int dummy) {
	
	if (my_cerver) {
		cerver_stats_print (my_cerver, true, true);
		cerver_teardown (my_cerver);
	} 

	exit (0);

}

#pragma endregion

#pragma region handler

static void handle_test_request (Packet *packet) {

	if (packet) {
		// cerver_log_debug ("Got a test message from client. Sending another one back...");
		cerver_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "Got a test message from client. Sending another one back...");
		
		Packet *test_packet = packet_generate_request (APP_PACKET, TEST_MSG, NULL, 0);
		if (test_packet) {
			packet_set_network_values (test_packet, NULL, NULL, packet->connection, NULL);
			size_t sent = 0;
			if (packet_send (test_packet, 0, &sent, false)) 
				cerver_log_error ("Failed to send test packet to client!");

			else {
				// printf ("Response packet sent: %ld\n", sent);
			}
			
			packet_delete (test_packet);
		}
	}

}

static void handler (void *data) {

	if (data) {
		Packet *packet = (Packet *) data;
		
		switch (packet->header->request_type) {
			case TEST_MSG: handle_test_request (packet); break;

			default: 
				cerver_log_msg (stderr, LOG_WARNING, LOG_PACKET, "Got an unknown app request.");
				break;
		}
	}

}

#pragma endregion

#pragma region events

static void on_cever_started (void *event_data_ptr) {

	if (event_data_ptr) {
		CerverEventData *event_data = (CerverEventData *) event_data_ptr;

		char *status = c_string_create (
			"Cerver %s has started!\n", 
			event_data->cerver->info->name->str
		);

		if (status) {
			printf ("\n");
			cerver_log_msg (stdout, LOG_EVENT, LOG_CERVER, status);
			free (status);
		}

		printf ("Test Message: %s\n\n", ((String *) event_data->action_args)->str);
	}

}

static void on_cever_teardown (void *event_data_ptr) {

	if (event_data_ptr) {
		CerverEventData *event_data = (CerverEventData *) event_data_ptr;

		char *status = c_string_create (
			"Cerver %s is going to be destroyed!\n", 
			event_data->cerver->info->name->str
		);

		if (status) {
			printf ("\n");
			cerver_log_msg (stdout, LOG_EVENT, LOG_CERVER, status);
			free (status);
		}
	}

}

static void on_client_connected (void *event_data_ptr) {

	if (event_data_ptr) {
		CerverEventData *event_data = (CerverEventData *) event_data_ptr;

		char *status = c_string_create (
			"Client %ld connected with sock fd %d to cerver %s!\n",
			event_data->client->id,
			event_data->connection->socket->sock_fd, 
			event_data->cerver->info->name->str
		);

		if (status) {
			printf ("\n");
			cerver_log_msg (stdout, LOG_EVENT, LOG_CLIENT, status);
			free (status);
		}
	}

}

static void on_client_close_connection (void *event_data_ptr) {

	if (event_data_ptr) {
		CerverEventData *event_data = (CerverEventData *) event_data_ptr;

		char *status = c_string_create (
			"A client closed a connection to cerver %s!\n",
			event_data->cerver->info->name->str
		);

		if (status) {
			printf ("\n");
			cerver_log_msg (stdout, LOG_EVENT, LOG_CLIENT, status);
			free (status);
		}
	}

}

#pragma endregion

#pragma region main

int main (void) {

	srand (time (NULL));

	// register to the quit signal
	signal (SIGINT, end);

	printf ("\n");
	cerver_version_print_full ();
	printf ("\n");

	cerver_log_debug ("Simple Test Message Example");
	printf ("\n");
	cerver_log_debug ("Single app handler with direct handle option enabled");
	printf ("\n");

	my_cerver = cerver_create (CERVER_TYPE_CUSTOM, "my-cerver", 7000, PROTOCOL_TCP, false, 2, 2000);
	if (my_cerver) {
		cerver_set_welcome_msg (my_cerver, "Welcome - Simple Test Message Example");

		/*** cerver configuration ***/
		cerver_set_receive_buffer_size (my_cerver, 4096);
		cerver_set_thpool_n_threads (my_cerver, 4);

		Handler *app_handler = handler_create (handler);
		// 27/05/2020 - needed for this example!
		handler_set_direct_handle (app_handler, true);
		cerver_set_app_handlers (my_cerver, app_handler, NULL);

		String *test = str_new ("This is a test!");
		cerver_event_register (
			my_cerver, 
			CERVER_EVENT_STARTED,
			on_cever_started, test, str_delete,
			false, false
		);

		cerver_event_register (
			my_cerver, 
			CERVER_EVENT_TEARDOWN,
			on_cever_teardown, NULL, NULL,
			false, false
		);

		cerver_event_register (
			my_cerver, 
			CERVER_EVENT_CLIENT_CONNECTED,
			on_client_connected, NULL, NULL,
			false, false
		);

		cerver_event_register (
			my_cerver, 
			CERVER_EVENT_CLIENT_CLOSE_CONNECTION,
			on_client_close_connection, NULL, NULL,
			false, false
		);

		if (cerver_start (my_cerver)) {
			char *s = c_string_create ("Failed to start %s!",
				my_cerver->info->name->str);
			if (s) {
				cerver_log_error (s);
				free (s);
			}

			cerver_delete (my_cerver);
		}
	}

	else {
        cerver_log_error ("Failed to create cerver!");

		cerver_delete (my_cerver);
	}

	return 0;

}

#pragma endregion