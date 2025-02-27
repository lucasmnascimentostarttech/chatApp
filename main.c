#include "./inc/includes.h"
#include "./inc/client.h"
#include "./inc/server.h"

int main(int argc, char *argv[]){
    setvbuf(stdout, NULL, _IONBF, 0);
    bool serverMode = false;
    char *IP = malloc(30*sizeof(char)), *Port = malloc(30*sizeof(char));
    int opt;

    while((opt = getopt(argc, argv, "si:p:")) != -1){
        switch(opt){
            case 's':
                return(runServer());
                break;
            case 'i':
                printf("%s\n", optarg);
                IP = optarg;
                break;
            case 'p':
                printf("%s\n", optarg);
                Port = optarg;
                break;
        }
    }

    if(strlen(IP) == 0 || strlen(Port) == 0){
        printf("Invalid arguments\n");
        printf("Running as a server [-s]\n");
        printf("Running as a client [-i <IP> -p <PORT>]\n");
        return 1;
    }

    return runClient(IP, Port);
}

