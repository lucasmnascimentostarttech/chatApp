#include "./inc/client.h"
#include "./inc/msg_control.h"


int runClient(char *IP, char *Port){
    setvbuf(stdout, NULL, _IONBF, 0);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    int fd;

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    Connection *conct = malloc(sizeof(conct));
    conct->ip = IP;
    conct->port = Port;

    /* Initialize Server */
    if(setServerHints_client(&hints, &res) != 0){return 1;}
    if(getfd_client(&fd, res->ai_family, res->ai_socktype, res->ai_protocol, conct) != 0){return 1;}
    if(setFlags_client(&fd) != 0){return 1;}
    if(connectToServer(&fd, res->ai_addr, res->ai_addrlen) != 0){return 1;}
    freeaddrinfo(res);


    /* Run */
    while(true){
        if(sendMsg(conct) != 0){return 1;}
        if(recvMsg(conct) != 0){return 1;}
    }
}

int connectToServer(int *fd, struct sockaddr *addr, socklen_t addrlen){
    int status = 0;
    while(true){
        status = connect(*fd, addr, addrlen);
        if(status < 0){
            if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINPROGRESS || errno == EALREADY){
                continue;
            }
            else if(errno == EISCONN){
                printf("connected\n");
                return 0;
            }
            else{
                perror("connectError");
                return 1;
            }
        }
        return 0;
    }
}


int setServerHints_client(struct addrinfo *hints, struct addrinfo **res){
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
int getfd_client(int *fd, int family, int socktype, int protocol, Connection *conct){
    *fd = socket(family, socktype, protocol);
    if(fd < 0){
        perror("socketError");
        return 1;
    }
    conct->fd = *fd;
    return 0;
}

int setFlags_client(int *fd){
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
