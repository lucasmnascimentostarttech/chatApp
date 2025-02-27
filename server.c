#include "./inc/server.h"

#define welcomeMsg "Welcome to the chat!"
int totalClients = 0;

int runServer(){
    setvbuf(stdout, NULL, _IONBF, 0);
    int fd;

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    ClientsArray *clients = malloc(sizeof(clients));
    if(clients == NULL){
        perror("mallocError");
        return 1;
    }


    /* Initialize Server */
    if(setServerHints(&hints, &res) != 0){return 1;}
    if(getfd(&fd, res->ai_family, res->ai_socktype, res->ai_protocol) != 0){return 1;}
    if(bindSocket(&fd, res->ai_addr, res->ai_addrlen) != 0){return 1;}
    if(setListen(&fd, BACKLOG) != 0){return 1;}
    if(setFlags(&fd) != 0){return 1;}
    freeaddrinfo(res);

    /* Run */
    while(true){
        if(incomingClients(&fd, clients) != 0){return 1;}
        if(ongoingClients(clients) != 0){return 1;}
    }
}

int setServerHints(struct addrinfo *hints, struct addrinfo **res){
    hints->ai_flags = AI_PASSIVE;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_family = AF_INET;

    int status = getaddrinfo(NULL, PORT, hints, res);
    if(status != 0){
        fprintf(stderr, "getaddrinfoError: %s\n", gai_strerror(status));
        return 1;
    }
    return 0;
}
int getfd(int *fd, int family, int socktype, int protocol){
    *fd = socket(family, socktype, protocol);
    if(fd < 0){
        perror("socketError");
        return 1;
    }
    return 0;
}

int bindSocket(int *fd, struct sockaddr *addr, socklen_t addrlen){
    if(bind(*fd, addr, addrlen) < 0){
        perror("bindError");
        return 1;
    }
    return 0;
}

int setListen(int *fd, int backlog){
    if(listen(*fd, backlog) != 0){
        perror("listenError");
        return 1;
    }

    struct sockaddr_in temp, *temp2;
    struct ifaddrs *ipAddress, *next;
    char netstring[INET_ADDRSTRLEN], ipstring[INET_ADDRSTRLEN], port[10];
    socklen_t temp_len = sizeof(temp);

    if(getifaddrs(&ipAddress) < 0){
        perror("getifaddrsError");
        return 1;
    }   
    for(next = ipAddress; next!=NULL; next = next->ifa_next){
        if(next->ifa_addr == NULL){
            continue;
        }
        if(next->ifa_addr->sa_family == AF_INET){
            temp2 = (struct sockaddr_in *)next->ifa_addr;
            inet_ntop(next->ifa_addr->sa_family, &(temp2->sin_addr), ipstring, temp_len);

            if(strcmp("127.0.0.1", ipstring) != 0){
                break;
            }
        }
    }

    if(getsockname(*fd, (struct sockaddr *)&temp, &temp_len) < 0){
        perror("getsocknameError");
        return 1;
    }
    inet_ntop(AF_INET, &temp.sin_addr, netstring, temp_len);
    int port_int = ntohs(temp.sin_port);
    snprintf(port, sizeof(port), "%d", port_int);
    printf("Listening -> NET: %s | PORT: %s | IP: %s\n", netstring, port, ipstring);
    return 0;
}

int setFlags(int *fd){
    int flags;
        if ((flags = fcntl(*fd, F_GETFL, 0)) == -1) {
            perror("fcntl F_GETFL");
            return -1;
        }
    
        flags |= O_NONBLOCK;
        if (fcntl(*fd, F_SETFL, flags) == -1) {
            perror("fcntl F_SETFL");
            return -1;
        }
    return 0;
}

int incomingClients(int *serverfd, ClientsArray *clients){
    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);
    int fd = accept(*serverfd, (struct sockaddr *)&addr, &len);
    if(fd < 0){
        if(errno == EAGAIN || errno == EWOULDBLOCK){
            return 0;
        }
        else{
            return 1;
        }
    }
    else{
        return addClient(fd, clients);
    }
}

int ongoingClients(ClientsArray *clients){
    Clients *next;
    for(next = clients->head; next != NULL; next = next->next){
        getMessage(next);
        if(strlen(next->client_msg) > 0){
            sendMessage(next, clients);
        }
    }
    return 0;
}

int addClient(int clientfd, ClientsArray *clients){
    /* Allocate memory for new Client in the server */
    fflush(stdout);
    Clients *newClient;
    int tries = 0;
    while(tries < 5){
        newClient = malloc(sizeof(Clients));
        if(newClient != NULL){
            newClient->next = NULL;
            newClient->client_msg = malloc(BUFFER_SIZE);
            if(newClient->client_msg != NULL){
                newClient->client_msg[0] = '\0';
            }
            else{perror("mallocError"); free(newClient); return 1;}
            newClient->client_fd = clientfd;
            break;
        }
        tries++;
    }
    if(newClient == NULL){
        perror("mallocError");
        return 1;
    }

    /* Add the new client in the linked list */
    if(totalClients == 0){
        clients->head = newClient;
        clients->tail = newClient;
        totalClients ++;
    }
    else{
        newClient->next = clients->head;
        clients->head = newClient;
        totalClients++;
    }
    printf("New Client connected.\n");
    sendWelcomeMessage(newClient);
    return 0;
}