#include <stdlib.h>
#include <stdio.h>

#include <cerver/client.h>
#include <cerver/packets.h>

#include <app/app.h>

#include "../test.h"

#define MESSAGE			"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat."

#define MESSAGE_0		"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua."
#define MESSAGE_1		"Sagittis nisl rhoncus mattis rhoncus urna. Vitae congue eu consequat ac felis donec et odio. "
#define MESSAGE_2		"Commodo sed egestas egestas fringilla phasellus. Tellus id interdum velit laoreet id donec ultrices tincidunt. Porttitor massa id neque aliquam vestibulum morbi blandit cursus."
#define MESSAGE_3		"Malesuada pellentesque elit eget gravida cum. Pharetra vel turpis nunc eget lorem dolor sed viverra."
#define MESSAGE_4		"Justo donec enim diam vulputate. Dui nunc mattis enim ut. Quis vel eros donec ac odio tempor. Lorem ipsum dolor sit amet consectetur."

static Client *client = NULL;
static Connection *connection = NULL;

static void single_app_message (const char *msg) {

	Packet *message = packet_create (
		PACKET_TYPE_APP, APP_REQUEST_MESSAGE,
		msg, strlen (msg)
	);

	test_check_ptr (message);

	test_check_unsigned_eq (
		packet_generate (message), 0, NULL
	);

	// send the packet
	packet_set_network_values (
		message, NULL, client, connection, NULL
	);

	size_t sent = 0;
	test_check_unsigned_eq (
		packet_send (
			message, 0, &sent, false
		), 0, NULL
	);

	test_check_unsigned_ne (sent, 0);

	packet_delete (message);

}

static void single_app_message_generate_request (const char *msg) {

	Packet *message = packet_generate_request (
		PACKET_TYPE_APP, APP_REQUEST_MESSAGE,
		msg, strlen (msg)
	);

	test_check_ptr (message);

	// send the packet
	packet_set_network_values (
		message, NULL, client, connection, NULL
	);

	size_t sent = 0;
	test_check_unsigned_eq (
		packet_send (
			message, 0, &sent, false
		), 0, NULL
	);

	test_check_unsigned_ne (sent, 0);

	// done
	packet_delete (message);

}

static void single_app_message_manual (const char *msg) {

	Packet *message = packet_new ();
	
	test_check_ptr (message);

	// generate the packet
	size_t packet_len = sizeof (PacketHeader) + sizeof (AppMessage);
	message->packet = malloc (packet_len);
	message->packet_size = packet_len;

	char *end = (char *) message->packet;
	PacketHeader *header = (PacketHeader *) end;
	header->packet_type = PACKET_TYPE_APP;
	header->packet_size = packet_len;

	header->request_type = APP_REQUEST_MESSAGE;

	end += sizeof (PacketHeader);

	app_message_create_internal (
		(AppMessage *) end, msg
	);

	// send the packet
	packet_set_network_values (
		message, NULL, client, connection, NULL
	);

	size_t sent = 0;
	test_check_unsigned_eq (
		packet_send (
			message, 0, &sent, false
		), 0, NULL
	);

	test_check_unsigned_ne (sent, 0);

	// done
	packet_delete (message);

}

int main (int argc, const char **argv) {

	(void) printf ("Testing CLIENT packets...\n");

	client = client_create ();

	test_check_ptr (client);

	client_set_name (client, "test-client");
	test_check_ptr (client->name->str);
	test_check_str_eq (client->name->str, "test-client", NULL);
	test_check_str_len (client->name->str, strlen ("test-client"), NULL);

	// Handler *app_handler = handler_create (client_app_handler);
	// handler_set_direct_handle (app_handler, true);
	// client_set_app_handlers (client, app_handler, NULL);

	connection = client_connection_create (
		client, "127.0.0.1", 7000, PROTOCOL_TCP, false
	);

	test_check_ptr (connection);

	connection_set_max_sleep (connection, 30);

	test_check_int_eq (
		client_connect_and_start (client, connection), 0,
		"Failed to connect to cerver!"
	);

	/*** send ***/
	single_app_message (MESSAGE);
	single_app_message_generate_request (MESSAGE);
	single_app_message_manual (MESSAGE);

	// wait for any response to arrive
	(void) sleep (2);

	/*** end ***/
	client_connection_end (client, connection);
	client_teardown (client);

	(void) printf ("Done!\n\n");

	return 0;

}