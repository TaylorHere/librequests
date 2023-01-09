//
// Created by taylor on 22-11-18.
//

#ifndef LIBREQUESTS_HTTP_H
#define LIBREQUESTS_HTTP_H

#include <ctype.h>
#include "tcp.h"
#include <stdbool.h>
#include "hashmap.h"

#define SP " "
#define CRLF "\r\n"

#define REQUEST_MESSAGE_SIZE(pRequest, headers)                          \
strlen(pRequest->line.method) + strlen(SP) + strlen(pRequest->line.uri) + \
strlen(SP) + strlen(pRequest->line.version) + strlen(CRLF) +               \
strlen(headers) + strlen(CRLF) +                                            \
strlen(pRequest->body)

#define REQUEST_FORMAT(pRequest, headers)                                   \
pRequest->line.method,SP,pRequest->line.uri,SP,pRequest->line.version,CRLF,  \
headers,                                                                      \
CRLF,                                                                          \
pRequest->body

typedef HASHMAP(char, char) Header;

typedef struct RequestLine {
  char *method;
  char *uri;
  char *version;
} RequestLine;

typedef struct HTTPRequest {
  RequestLine line;
  char *body;
  Header headers;
} HTTPRequest;

HTTPRequest HTTPRequest_new ();

void HTTPRequest_drop (HTTPRequest *self);

HTTPRequest *
HTTPRequest_set_request_line (HTTPRequest *self, char *method, char *uri, char *version);

HTTPRequest *HTTPRequest_set_body (HTTPRequest *self, char *body);

char *HTTPRequest_header_as_message (HTTPRequest *self);

char *HTTPRequest_as_message (HTTPRequest *self);

/**
 * StatusLine -> HTTP-Version Status-Code Reason-Phrase CRLF
 * Headers
 * CRLF
 * body
 */

typedef struct StatusLine {
  char *version;
  ulong status_code;
  char *reason_phrase;
} StatusLine;

typedef struct HTTPResponse {
  StatusLine status_line;
  char *body;
  Header headers;
} HTTPResponse;

HTTPResponse HTTPResponse_new ();
void
HTTPResponse_set_status_line (HTTPResponse *self, char *version, char *status_code, char *reason_phrase);

void HTTPResponse_set_body (HTTPResponse *self, char *body);

void HTTPResponse_drop (HTTPResponse *self);

typedef struct URL {
  int use_tls;
  char *host;
  long port;
  char *path;
  char *auth_user;
  char *auth_pwd;
} URL;

void URL_drop (URL *self);

URL URL_new (char *input_url);

ulong parse_header (char *receive, HTTPResponse *pResponse);

typedef struct HTTP {
  URL *URL;
  TCPConnection *pConn;
  HTTPRequest *pRequest;
  HTTPResponse *pResponse;
} HTTP;

HTTP
HTTP_new (TCPConnection *pConn, HTTPRequest *pRequest, HTTPResponse *pResponse);

void HTTP_send (HTTP *self);

void HTTP_receive (HTTP *self);

HTTPResponse requests (char *method, char *url_string, char *body);

#endif //LIBREQUESTS_HTTP_H
