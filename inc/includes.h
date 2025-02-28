#include <stdio.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include<stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <util.h>

#ifndef INCLUDES_H
#define INCLUDES_H

#define BACKLOG 5
#define PORT "8000"
#define BUFFER_SIZE 10000
extern int totalClients;

typedef struct Clients Clients;
struct Clients{
    bool free;
    int client_fd;
    char *client_msg;
    Clients *next;
    Clients *previous;
};

typedef struct{
    Clients *tail;
    Clients *head;
}ClientsArray;

typedef struct{
    int fd;
    char *port;
    char *ip;
    char *msg;
}Connection;
#endif
