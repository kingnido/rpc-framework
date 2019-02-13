#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "utils.h"

int sendn(int fd, void *data, int length, int flags)
{
	int sent;

	while (length > 0) {
		sent = send(fd, data, length, flags);
		if (sent == 0) {
			printf("Connection lost.\n");
			return -1;
		}
		if (sent < 0) {
			printf("Some kind of error.\n");
			printf("errno = %d.\n", errno);
			return -1;
		}
		if (sent < length) {
			printf("Not completely sent.\n");
		}
		length -= sent;
		data += sent;
	} while (length > 0);

	return 0;
}

int recvn(int fd, void *data, int length, int flags)
{
	int rcvd;

	while (length > 0) {
		rcvd = recv(fd, data, length, flags);
		if (rcvd == 0) {
			printf("Connection lost.\n");
			return -1;
		}
		if (rcvd < 0) {
			printf("Some kind of error.\n");
			return -1;
		}
		if (rcvd < length) {
			printf("Not completely received.\n");
		}
		length -= rcvd;
		data += rcvd;
	}

	return 0;
}

void *memdup(void *src, int length)
{
	void *dst = NULL;

	if (!src || length <= 0) {
		return NULL;
	}

	dst = malloc(length);
	if (!dst) {
		return NULL;
	}

	memcpy(dst, src, length);

	return dst;
}
