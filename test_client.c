#include <stdio.h>
#include <stdlib.h>
#include "rpc_framework.h"

int main(int argc, char **argv)
{
	int arg[2] = { 5, 6 };
	int ret;
	int result;
	int cmd;
	int i;

	for (i = 0; i < argc; i++) {
		printf("argv[%d] = '%s'\n", i, argv[i]);
	}

	if (argc != 4) {
		printf("%s [+-] <num> <num>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	arg[0] = atoi(argv[2]);
	arg[1] = atoi(argv[3]);

	switch (*argv[1]) {
		case '-':
			cmd = 1;
			break;
		case '+':
			cmd = 0;
			break;
		default:
			printf("%s [+-] <num> <num>\n", argv[0]);
			exit(EXIT_FAILURE);
			break;
	}

	result = rpc_call("127.0.0.1", 6666, cmd, &arg, sizeof(arg), &ret, sizeof(ret));

	if (result < 0) {
		printf("error: %d\n", result);
		exit(EXIT_FAILURE);
	}

	printf("%d %c %d = %d.\n", arg[0], *argv[1], arg[1], ret);

	exit(EXIT_SUCCESS);
}
