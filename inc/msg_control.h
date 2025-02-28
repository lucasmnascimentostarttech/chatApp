#include "includes.h"


int getMessage(Clients *client, ClientsArray *clients);
int sendMessage(Clients *client, ClientsArray *clients);
int sendWelcomeMessage(Clients *client);
int recvMsg(Connection *conct);
int sendMsg(Connection *conct);
