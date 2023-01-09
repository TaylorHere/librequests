//
// Created by taylor on 23-1-10.
//


#ifndef LIBREQUESTS_TCP_C
#define LIBREQUESTS_TCP_C

#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include "librequests.h"
#include "tcp.h"

TCPConnection TCPConnection_new (char *hostname, long port)
{
  TCPConnection conn = {.hostname = hostname, .port = port, .fd = -1};
  struct hostent host = *gethostbyname (conn.hostname);

  memset (&conn.sockaddr, 0, sizeof (conn.sockaddr));
  conn.sockaddr.sin_family = host.h_addrtype;
  conn.sockaddr.sin_port = htons (conn.port);
  conn.sockaddr.sin_addr.s_addr = inet_addr (host.h_name);

  if ((conn.fd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
      fatal ("Failed to create TCP socket");
    }

  if (connect (conn.fd, (struct sockaddr *) &conn.sockaddr, sizeof (conn.sockaddr))
      != 0)
    {
      fatal ("Failed to connect with server");
    }
  return conn;
};

void TCPConnection_send (TCPConnection *self, char *message)
{
  size_t len_msg = strlen (message);
  if (send (self->fd, message, len_msg, 0) != len_msg)
    {
      fatal ("Failed to send");
    }
};

void TCPConnection_drop (TCPConnection *self)
{
  close (self->fd);
};

void TCPConnection_receive (TCPConnection *self, char *buffer)
{
  if (recv (self->fd, buffer, BUFFSIZE - 1, MSG_DONTWAIT) < 0
      && errno != EAGAIN)
    {
      TCPConnection_drop (self);
    }
}

#endif //LIBREQUESTS_TCP_C
