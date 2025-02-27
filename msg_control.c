#include "./inc/msg_control.h"

int getMessage(Clients *client){
    int recvBytes = 0, total_sent = 0;
    char msg[BUFFER_SIZE];
    memset(msg, 0, sizeof(msg));
    recvBytes = recv(client->client_fd, msg, BUFFER_SIZE, MSG_DONTWAIT);
    if(recvBytes < 0){
        if(errno == EAGAIN || errno == EWOULDBLOCK){
            return 0;
        }
        else{
            perror("recvError");
            return 1;
        }
    }
    else{
        int tries = 0;
        while(tries < 5){
            client->client_msg = strdup(msg);
            if(client->client_msg != NULL){
                return 0;
            }
            tries++;
        }
        printf("%s\n", client->client_msg);
        return 1;
    }
}
int sendMessage(Clients *client, ClientsArray *clients){
    Clients *next;
    char msg[BUFFER_SIZE];
    memset(msg, 0, strlen(msg));
    sprintf(msg, "%s", client->client_msg);
    size_t len = strlen(msg);
    for(next = clients->head; next != NULL; next = next->next){
        int sent = 0, total_sent = 0;
        if(next != client){
            while(total_sent < len){
                sent = send(next->client_fd, msg + total_sent, len - total_sent, 0);
                if(sent < 0){
                    if(errno == EAGAIN || errno == EWOULDBLOCK){
                        continue;
                    }
                    perror("sendError");
                    return 1;
                }
                else{
                    total_sent += sent;
                }
            }
            if(total_sent == len){
                continue;
            }
        }
    }
    memset(client->client_msg, 0, BUFFER_SIZE);
    return 0;
}

int sendWelcomeMessage(Clients *client){
    int sent = 0, total_sent = 0;
    char msg[100];
    sprintf(msg, "Welcome to the chat!\n");
    size_t len = strlen(msg);

    while(total_sent < len){
        sent = send(client->client_fd, msg + total_sent, len - total_sent, 0);
        if(sent < 0){
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                continue;
            }
            perror("sendError");
            return 1;
        }
        else{
            total_sent += sent;
        }
    }
    if(total_sent == len){
        return 0;
    }
    return 1;
}

int recvMsg(Connection *conct){
    int bytes, try = 0, fd = conct->fd;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    while(try < 10){
        bytes = recv(fd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
        if(bytes < 0){
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                try++;
                continue;
            }
            else{
                perror("recvError");
                return 1;
            }
        }
        else{
            printf("%s", buffer);
            memset(buffer, 0, strlen(buffer));
        }
    }
    return 0;

}

int sendMsg(Connection *conct){
    int sent = 0, total_sent = 0, bytes = 0;
    int fd = conct->fd;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    fgets(buffer, sizeof(buffer), stdin);
    bytes = strlen(buffer);
    //printf("helloo\n");
    if(bytes > 0){
        //printf("%s\n", buffer);
        fflush(stdout);
        while(total_sent < bytes){
            sent = send(fd, buffer + total_sent, bytes - total_sent, MSG_DONTWAIT);

            if(sent < 0){
                if(errno == EAGAIN || errno == EWOULDBLOCK){
                    continue;
                }
                else{
                    perror("Failed to send message.");
                    return 1;
                }
            }
            total_sent += sent;
        }
    }
    if(total_sent == bytes){
        return 0;
    }
    return 1;

}