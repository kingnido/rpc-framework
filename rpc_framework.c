#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/ip.h>

#include "rpc_framework.h"
#include "utils.h"

static int print_request(rpc_request_header_t *rpc_request_header)
{
	printf("rpc_request_header: function_id = %d, data_size = %d.\n", rpc_request_header->function_id, rpc_request_header->data_size);

	return 0;
}

static int print_response(rpc_response_header_t *rpc_response_header)
{
	printf("rpc_response_header: result = %d, data_size = %d.\n", rpc_response_header->result, rpc_response_header->data_size);

	return 0;
}

int rpc_function_register(rpc_server_t *rpc_server, int function_id, rpc_func_t *function_ptr, int arg_size, int ret_size)
{
	func_desc_t function;
	int result;

	if (!rpc_server || !function_ptr || arg_size < 0 || ret_size < 0) {
		return -1;
	}

	function.id = function_id;
	function.ptr = function_ptr;
	function.arg_size = arg_size;
	function.ret_size = ret_size;

	result = linkedList_append(rpc_server->function_list, memdup(&function, sizeof(function)));
	if (result < 0) {
		printf("failed.\n");
	}

	printf("Registered function:\n");
	printf("id       = %d\n", function.id);
	printf("ptr      = %p\n", function.ptr);
	printf("arg_size = %d\n", function.arg_size);
	printf("ret_size = %d\n", function.ret_size);

	return 0;
}

func_desc_t *rpc_function_find(rpc_server_t *rpc_server, int function_id)
{
	int i = 0;
	func_desc_t *function = NULL;
	linkedList_t *function_list = NULL;

	if (!rpc_server || !rpc_server->function_list) {
		return NULL;
	}

	function_list = rpc_server->function_list;

	for (i = 0; i < linkedList_getLength(function_list); i++) {
		function = linkedList_getAt(function_list, i);
		if (function->id == function_id) {
			return function;
		}
	}

	return NULL;
}

int rpc_server_init(rpc_server_t *rpc_server)
{
	if (!rpc_server) {
		return -1;
	}

	rpc_server->listen_fd = 0;

	rpc_server->function_list = linkedList_new();
	if (!rpc_server->function_list) {
		return -1;
	}

	rpc_server->client_list = NULL;

	return 0;
}

void rpc_server_thread(void *arg)
{
	rpc_server_t *rpc_server = (rpc_server_t *) arg;
	int result;
	int client_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	rpc_request_header_t rpc_request_header;
	rpc_response_header_t rpc_response_header;
	func_desc_t *function;
	char buff_in[1024];
	char buff_out[1024];

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(6666);

	memset(&client_addr, 0, sizeof(client_addr));

	signal(SIGPIPE, SIG_IGN);

	rpc_server->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (!rpc_server->listen_fd) {
		printf("Sem socket.\n");
		exit(EXIT_FAILURE);
	}

	// to reuse a address
	{ int on = 1; if (setsockopt(rpc_server->listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) { printf("setsockopt deu errado.\n"); exit(EXIT_FAILURE); } }

	result = bind(rpc_server->listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (result) {
		printf("bind deu errado.\n");
		exit(EXIT_FAILURE);
	}

	result = listen(rpc_server->listen_fd, 10);
	if (result) {
		printf("Listen deu errado.\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		/* accept new connection */
		client_fd = accept(rpc_server->listen_fd, NULL, 0);
		if (client_fd < 0) {
			printf("Failed to accept new connection.\n");
			exit(EXIT_FAILURE);
		}
		
		/* receive request */
		result = recvn(client_fd, &rpc_request_header, sizeof(rpc_request_header), 0);
		if (result < 0) {
			printf("Error on request receiving header.\n");
			exit(EXIT_FAILURE);
		}

		print_request(&rpc_request_header);

		if (rpc_request_header.data_size > 0) {
			result = recvn(client_fd, buff_in, rpc_request_header.data_size, 0);
			if (result < 0) {
				printf("Error on resquest receiving data.\n");
				exit(EXIT_FAILURE);
			}
		}

		// find function
		function = rpc_function_find(rpc_server, rpc_request_header.function_id);
		if (!function) {
			printf("Couldn't find function.\n");
			exit(EXIT_FAILURE);
		}

		// run function
		result = (*function->ptr)(buff_in, buff_out);
		if (result < 0) {
			printf("Whaaaat!\n");
			exit(EXIT_FAILURE);
		}

		rpc_response_header.data_size = function->ret_size;
		rpc_response_header.result = result;

		print_response(&rpc_response_header);

		/* send response */
		result = sendn(client_fd, &rpc_response_header, sizeof(rpc_response_header), 0);
		if (result < 0) {
			printf("Error on sending response header.\n");
			exit(EXIT_FAILURE);
		}

		if (rpc_response_header.data_size > 0) {
			result = sendn(client_fd, buff_out, rpc_response_header.data_size, 0);
			if (result < 0) {
				printf("Error on sending response data.\n");
				exit(EXIT_FAILURE);
			}
		}

		close(client_fd);
	}
}

int rpc_call(const char *ip, int port, int function_id, void *arg, int arg_size, void *ret, int ret_size)
{
	int server_fd;
	int client_fd;
	int result;
	rpc_request_header_t rpc_request_header;
	rpc_response_header_t rpc_response_header;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	inet_aton(ip, &server_addr.sin_addr);
	server_addr.sin_port = htons(port);

	signal(SIGPIPE, SIG_IGN);

	client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd < 0) {
		printf("Couldn't create socket.\n");
		return -1;
	}

	server_fd = connect(client_fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in));
	if (server_fd < 0) {
		printf("Couln't connect to server.\n");
		return -1;
	}

	rpc_request_header.function_id = function_id;
	rpc_request_header.data_size = arg_size;

	print_request(&rpc_request_header);

	result = sendn(client_fd, &rpc_request_header, sizeof(rpc_request_header), 0);
	if (result < 0) {
		printf("Error on sending request header.\n");
		exit(EXIT_FAILURE);
	}

	if (rpc_request_header.data_size > 0) {
		result = sendn(client_fd, arg, rpc_request_header.data_size, 0);
		if (result < 0) {
			printf("Error on sending request data.\n");
			exit(EXIT_FAILURE);
		}

	}

	result = recvn(client_fd, &rpc_response_header, sizeof(rpc_response_header), 0);
	if (result < 0) {
		printf("Error on receiving response header.\n");
		exit(EXIT_FAILURE);
	}

	print_response(&rpc_response_header);

	if (rpc_response_header.data_size > 0) {
		result = recvn(client_fd, ret, rpc_response_header.data_size, 0);
		if (result < 0) {
			printf("Error on receiving response data.\n");
			exit(EXIT_FAILURE);
		}
	}

	shutdown(server_fd, SHUT_RDWR);
	close(server_fd);
	close(client_fd);

	return rpc_response_header.result;
}
