#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <signal.h>

#include <cerver/version.h>
#include <cerver/cerver.h>
#include <cerver/handler.h>

#include <cerver/http/http.h>
#include <cerver/http/route.h>
#include <cerver/http/request.h>
#include <cerver/http/response.h>

#include <cerver/utils/utils.h>
#include <cerver/utils/log.h>

Cerver *web_cerver = NULL;

#pragma region end

// correctly closes any on-going server and process when quitting the appplication
void end (int dummy) {

	if (web_cerver) {
		cerver_stats_print (web_cerver, false, false);
		printf ("\nHTTP Cerver stats:\n");
		http_cerver_all_stats_print ((HttpCerver *) web_cerver->cerver_data);
		printf ("\n");
		cerver_teardown (web_cerver);
	}

	cerver_end ();

	exit (0);

}

#pragma endregion

#pragma region routes

// GET /
void main_handler (CerverReceive *cr, HttpRequest *request) {

	if (http_response_render_file (cr, "./examples/web/public/echo.html")) {
		cerver_log_error ("Failed to send ./examples/web/public/echo.html");
	}

}

void test_handler (CerverReceive *cr, HttpRequest *request) {

	HttpResponse *res = http_response_json_msg ((http_status) 200, "Test route works!");
	if (res) {
		http_response_print (res);
		http_response_send (res, cr->cerver, cr->connection);
		http_respponse_delete (res);
	}

}

void echo_handler (CerverReceive *cr, HttpRequest *request) {

	HttpResponse *res = http_response_json_msg ((http_status) 200, "Echo route works!");
	if (res) {
		http_response_print (res);
		http_response_send (res, cr->cerver, cr->connection);
		http_respponse_delete (res);
	}

}

void echo_handler_on_open (Cerver *cerver, Connection *connection) {

	printf ("echo_handler_on_open ()\n");

}

void echo_handler_on_close (Cerver *cerver, const char *reason) {

	printf ("echo_handler_on_close ()\n");

}

void echo_handler_on_message (
	Cerver *cerver, Connection *connection,
	const char *msg, const size_t msg_len
) {

	printf ("echo_handler_on_message ()\n");

	printf ("message[%ld]: %.*s\n", msg_len, (int) msg_len, msg);

	http_web_sockets_send (
		cerver, connection,
		msg, msg_len
	);

}

void chat_handler (CerverReceive *cr, HttpRequest *request) {

	HttpResponse *res = http_response_json_msg ((http_status) 200, "Chat route works!");
	if (res) {
		http_response_print (res);
		http_response_send (res, cr->cerver, cr->connection);
		http_respponse_delete (res);
	}

}

#pragma endregion

#pragma region start

int main (int argc, char **argv) {

	srand (time (NULL));

	// register to the quit signal
	signal (SIGINT, end);

	cerver_init ();

	printf ("\n");
	cerver_version_print_full ();
	printf ("\n");

	cerver_log_debug ("Simple Web Cerver Example");
	printf ("\n");

	web_cerver = cerver_create (CERVER_TYPE_WEB, "web-cerver", 8080, PROTOCOL_TCP, false, 2, 1000);
	if (web_cerver) {
		/*** cerver configuration ***/
		cerver_set_receive_buffer_size (web_cerver, 4096);
		cerver_set_thpool_n_threads (web_cerver, 4);
		cerver_set_handler_type (web_cerver, CERVER_HANDLER_TYPE_THREADS);

		/*** web cerver configuration ***/
		HttpCerver *http_cerver = (HttpCerver *) web_cerver->cerver_data;

		http_cerver_static_path_add (http_cerver, "./examples/web/public");

		// GET /
		HttpRoute *main_route = http_route_create (REQUEST_METHOD_GET, "/", main_handler);
		http_cerver_route_register (http_cerver, main_route);

		// /test
		HttpRoute *test_route = http_route_create (REQUEST_METHOD_GET, "test", test_handler);
		http_cerver_route_register (http_cerver, test_route);

		// /echo
		HttpRoute *echo_route = http_route_create (REQUEST_METHOD_GET, "echo", echo_handler);
		http_route_set_modifier (echo_route, HTTP_ROUTE_MODIFIER_WEB_SOCKET);
		http_route_set_ws_on_open (echo_route, echo_handler_on_open);
		http_route_set_ws_on_message (echo_route, echo_handler_on_message);
		http_route_set_ws_on_close (echo_route, echo_handler_on_close);
		http_cerver_route_register (http_cerver, echo_route);

		// /chat
		HttpRoute *chat_route = http_route_create (REQUEST_METHOD_GET, "chat", chat_handler);
		http_route_set_modifier (chat_route, HTTP_ROUTE_MODIFIER_WEB_SOCKET);
		http_cerver_route_register (http_cerver, chat_route);

		if (cerver_start (web_cerver)) {
			cerver_log_error (
				"Failed to start %s!",
				web_cerver->info->name->str
			);

			cerver_delete (web_cerver);
		}
	}

	else {
		cerver_log_error ("Failed to create cerver!");

		cerver_delete (web_cerver);
	}

	cerver_end ();

	return 0;

}

#pragma endregion