#ifndef _UTILS_H_ /* [ */
#define _UTILS_H_

void *memdup(void *src, int length);

int sendn(int fd, void *data, int length, int flags);
int recvn(int fd, void *data, int length, int flags);

#endif /* ] */
