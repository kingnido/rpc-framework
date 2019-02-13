#include <stdio.h>
#include <stdlib.h>

#include "rpc_framework.h"

int add(void *arg, void *ret)
{
	*((int*)ret) = ((int*)arg)[0] + ((int*)arg)[1];

	return 0;
}

int sub(void *arg, void *ret)
{
	*((int*)ret) = ((int*)arg)[0] - ((int*)arg)[1];

	return 0;
}

int main(int argc, char **argv)
{
	rpc_server_t rpc_server;

	rpc_server_init(&rpc_server);
	rpc_function_register(&rpc_server, 0, &add, sizeof(int[2]), sizeof(int));
	rpc_function_register(&rpc_server, 1, &sub, sizeof(int[2]), sizeof(int));

	rpc_server_thread(&rpc_server);

	printf("Wazaaaahh.\n");

	exit(EXIT_FAILURE);
}
