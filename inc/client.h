#include "includes.h"
#include "msg_control.h"

#ifndef CLIENT_H
#define CLIENT_H

int runClient(char *IP, char *Port);
int connectToServer(int *fd, struct sockaddr *addr, socklen_t addrlen);
int setServerHints_client(struct addrinfo *hints, struct addrinfo **res);
int getfd_client(int *fd, int family, int socktype, int protocol, Connection *conct);
int setFlags_client(int *fd);

#endif
