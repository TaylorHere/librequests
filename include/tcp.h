//
// Created by taylor on 22-11-18.
//

#ifndef LIBREQUESTS_TCP_H
#define LIBREQUESTS_TCP_H

#include <netinet/in.h>
#define BUFFSIZE 1024

typedef struct TCPConnection {
  char *hostname;
  long port;
  int fd;
  struct sockaddr_in sockaddr;
} TCPConnection;

TCPConnection TCPConnection_new (char *hostname, long port);

void TCPConnection_send (TCPConnection *self, char *message);

void TCPConnection_drop (TCPConnection *self);

void TCPConnection_receive (TCPConnection *self, char *buffer);

#endif //LIBREQUESTS_TCP_H
