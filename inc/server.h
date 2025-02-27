#include "includes.h"
#include "msg_control.h"

#ifndef SERVER_H
#define SERVER_H

int runServer();
int setServerHints(struct addrinfo *hints, struct addrinfo **res);
int getfd(int *fd, int family, int socktype, int protocol);
int bindSocket(int *fd, struct sockaddr *addr, socklen_t addrlen);
int setListen(int *fd, int backlog);
int setFlags(int *fd);
int incomingClients(int *serverfd, ClientsArray *clients);
int ongoingClients(ClientsArray *clients);
int addClient(int clientfd, ClientsArray *clients);

#endif