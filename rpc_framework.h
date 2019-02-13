#ifndef _RPC_FRAMEWORK_H_ /* [ */
#define _RPC_FRAMEWORK_H_

#include "linked_list.h"

typedef int (rpc_func_t)(void *arg, void *ret);

typedef struct _rpc_request_header_t {
	int function_id;
	int data_size;
} rpc_request_header_t;

typedef struct _rpc_response_header_t {
	int result;
	int data_size;
} rpc_response_header_t;

typedef struct _func_desc_t {
	int id;
	rpc_func_t *ptr;
	int arg_size;
	int ret_size;
} func_desc_t;

typedef struct _rpc_server_t {
	int listen_fd;
	linkedList_t *function_list;
	linkedList_t *client_list;
} rpc_server_t;

typedef struct _client_t {
	int client_fd;
	int server_fd;
} client_t;

int rpc_function_register(rpc_server_t *rpc_server, int function_id, rpc_func_t *function_ptr, int arg_size, int ret_size);

#endif /* ] */
