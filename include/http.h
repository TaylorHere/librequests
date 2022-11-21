//
// Created by taylor on 22-11-18.
//

#ifndef LIBREQUESTS_HTTP_H
#define LIBREQUESTS_HTTP_H

#include <ctype.h>
#include "librequests.h"

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
    char* method;
    char* uri;
    char* version;
} RequestLine;

typedef struct HTTPRequest {
    RequestLine line;
    char* body;
    Header headers;
} HTTPRequest;

HTTPRequest HTTPRequest_new() {

    Header headers;
    hashmap_init(&headers, hashmap_hash_string, strcmp);
    hashmap_set_key_alloc_funcs(&headers, strdup, free);
    HTTPRequest request = {.body=calloc(0, sizeof(char*)), .headers=headers};

    return request;
}

void HTTPRequest_drop(HTTPRequest* self) {
    hashmap_cleanup(&self->headers);
    if (self->body) free(self->body);
    self = NULL;
}

HTTPRequest* HTTPRequest_set_request_line(HTTPRequest* self, char* method, char* uri, char* version) {
    self->line.method = method;
    self->line.uri = uri;
    self->line.version = version;
    return self;
}


HTTPRequest* HTTPRequest_set_body(HTTPRequest* self, char* body) {
    self->body = realloc(self->body, strlen(body) + 1);
    strcpy(self->body, body);
    return self;
}

char* HTTPRequest_header_as_message(HTTPRequest* self) {
    char* headers = calloc(1, sizeof(char*));
    const char* key;
    char* value;

    hashmap_foreach(key, value, &self->headers) {
        size_t size = strlen(key) + strlen(value) + 4;
        //4 is SP + CR + LF + ":"
        char* header_ch = calloc(size, sizeof(char*));
        sprintf(header_ch, "%s%s%s%s%s", key, ":", SP, value, CRLF);
        headers = realloc(headers, strlen(header_ch) + strlen(headers) + 1);
        strcat(headers, header_ch);
        free(header_ch);
    }

    return headers;
}

char* HTTPRequest_as_message(HTTPRequest* self) {
    char* headers = HTTPRequest_header_as_message(self);
    char* message = calloc(REQUEST_MESSAGE_SIZE(self, headers), sizeof(char*));
    sprintf(message, "%s%s%s%s%s%s%s%s%s", REQUEST_FORMAT(self, headers));
    free(headers);
    return message;
}


typedef struct StatusLine {
    char* version;
    ulong status_code;
    char* reason_phrase;
} StatusLine;

/**
 * StatusLine -> HTTP-Version Status-Code Reason-Phrase CRLF
 * Headers
 * CRLF
 * body
 */
typedef struct HTTPResponse {
    StatusLine status_line;
    char* body;
    Header headers;
} HTTPResponse;


HTTPResponse HTTPResponse_new() {
    Header headers;
    hashmap_init(&headers, hashmap_hash_string, strcmp);
    hashmap_set_key_alloc_funcs(&headers, strdup, free);
    StatusLine line = {.version=calloc(0, sizeof(char*)), .status_code=0, .reason_phrase=calloc(0, sizeof(char*))};
    HTTPResponse response = {.body=calloc(0, sizeof(char*)), .headers=headers, .status_line=line};
    return response;
}

void HTTPResponse_set_status_line(HTTPResponse* self, char* version, char* status_code, char* reason_phrase) {
    self->status_line.version = realloc(self->status_line.version, strlen(version) + 1);
    self->status_line.reason_phrase = realloc(self->status_line.reason_phrase, strlen(reason_phrase) + 1);

    strcpy(self->status_line.version, version);
    self->status_line.status_code = strtol(status_code, NULL, 10);
    strcpy(self->status_line.reason_phrase, reason_phrase);
}

void HTTPResponse_set_body(HTTPResponse* self, char* body) {
    self->body = realloc(self->body, strlen(body) + 1);
    strcpy(self->body, body);
}

void HTTPResponse_drop(HTTPResponse* self) {
    char* value;
    hashmap_foreach_data(value, &self->headers) {
        free(value);
    };
    hashmap_cleanup(&self->headers);
    free(self->status_line.version);
    free(self->status_line.reason_phrase);
    if (self->body) free(self->body);
}


typedef struct URL {
    int use_tls;
    char* host;
    long port;
    char* path;
    char* auth_user;
    char* auth_pwd;
} URL;


void URL_drop(URL* self) {
    if (self->path != NULL) free(self->path);
    if (self->host != NULL) free(self->host);
    if (self->auth_user != NULL) free(self->auth_user);
    if (self->auth_pwd != NULL) free(self->auth_pwd);
}

URL URL_new(char* input_url) {
    URL self = {.use_tls = 0};
    // URL-> http(s)://(user)(:)(pwd)(@)host(:port)(/)(path)
    //TODO: URL safe encoding;
    char* url = strdup(input_url);
    char* url_free_ptr = url;
    char* schema_hit;
    if ((schema_hit = strstr(url, "https://")) != NULL) {
        self.use_tls = 1;
        url = schema_hit + strlen("https://");
    } else if ((schema_hit = strstr(url, "http://")) != NULL) {
        self.use_tls = -1;
        url = schema_hit + strlen("http://");
    }

    char* auth_hit;
    if ((auth_hit = strstr(url, "@")) != NULL) {
        char* auth = strslice(url, auth_hit);
        char* auth_user_hit = strstr(auth, ":");
        self.auth_user = strslice(auth, auth_user_hit);
        auth = auth_user_hit + strlen(":");
        self.auth_pwd = strdup(auth);
        url = auth_hit + strlen("@");
    }


    char* hostports_hit;
    if ((hostports_hit = strstr(url, "/")) != NULL) {
        self.path = strdup(hostports_hit);
        url = strtok(url, self.path);
    } else {
        self.path = strdup("/");
    }

    char* host_hit;
    if ((host_hit = strstr(url, ":")) != NULL) {
        self.host = strslice(url, host_hit);
        url = host_hit + strlen(":");
        if (strlen(url) == 0) self.port = 80;
        else self.port = strtol(url, NULL, 10);
        if (self.port == 443 && self.use_tls == 0) self.use_tls = 1;
    } else {
        self.host = strdup(url);
        if (self.use_tls == 1) self.port = 443;
        else self.port = 80;
    }

    if (self.use_tls == 0) self.use_tls = -1;

    free(url_free_ptr);
    return self;
}


ulong parse_header(char* receive, HTTPResponse* pResponse) {
    ulong header_size = 0;
    char* target_hit;
    char* unseen = receive;
    if (receive == NULL || strlen(receive) == 0) {
        return header_size;
    }

    while ((target_hit = strstr(unseen, CRLF)) != NULL) {
        char* target_origin = NULL;
        target_origin = strslice(unseen, target_hit);
        char* target = target_origin;

        if (strlen(target) == 0) {
            free(target_origin);
            break; // 1.1.3 target is CRLF
        }
        header_size += strlen(target) + strlen(CRLF);

        if (strstr(target, ": ") == NULL) {
            // 1.1.1 target is status line;

            char* version_hit = strstr(target, SP);
            char* version = strslice(target, version_hit);

            target = version_hit + strlen(SP);
            char* status_code_hit = strstr(target, SP);

            char* status_code = strslice(target, status_code_hit);
            char* reason = strdup(status_code_hit + strlen(SP));

            HTTPResponse_set_status_line(pResponse, version, status_code, reason);
            free(version);
            free(status_code);
            free(reason);

        } else {
            // 1.1.2 target is headers;
            char* key_hit = strstr(target, ": ");
            char* key = strslice(target, key_hit);
            char* value = key_hit + strlen(": ");
            hashmap_put(&pResponse->headers, key, strdup(value));
            free(key);
        }
        unseen = target_hit + strlen(CRLF);
        free(target_origin);
    }

    return header_size;
}


typedef struct HTTP {
    URL* URL;
    TCPConnection* pConn;
    HTTPRequest* pRequest;
    HTTPResponse* pResponse;
} HTTP;

HTTP HTTP_new(TCPConnection* pConn, HTTPRequest* pRequest, HTTPResponse* pResponse) {
    HTTP http = {.pConn=pConn, .pRequest=pRequest, .pResponse=pResponse};
    return http;
}

HTTP HTTP_drop(HTTP* self) {

}

void HTTP_send(HTTP* self) {
    char* message = HTTPRequest_as_message(self->pRequest);
    TCPConnection_send(self->pConn, message);
    free(message);
}

void HTTP_receive(HTTP* self) {

    char* buffer = calloc(BUFFSIZE, sizeof(char*));
    char* receive = calloc(1, sizeof(char*));

    ulong header_size = 0;
    char* content_length;

    while (1) {
        if (recv(self->pConn->fd, buffer, BUFFSIZE - 1, MSG_DONTWAIT) < 0 && errno != EAGAIN) {
            free(receive);
            free(buffer);
            TCPConnection_drop(self->pConn);
            fatal("Failed to recv");
        }
        receive = (char*) realloc(receive, strlen(receive) + BUFFSIZE + 1);
        strcat(receive, buffer);
        memset(buffer, 0, BUFFSIZE);

//      1. parse the header, get the value of Content-Length;
        if ((header_size = parse_header(receive, self->pResponse)) > 0 &&
            (content_length = hashmap_get(&self->pResponse->headers, "Content-Length")) != NULL) {
            if ((strlen(receive) - header_size - strtol(content_length, NULL, 10)) >= 0)
                break; //FIXME: should not receive data more than content length says;
//               2. receive loop ends until Content-Length arrive;
        }
    }
//    3. copy body
    HTTPResponse_set_body(self->pResponse, &receive[header_size + strlen(CRLF)]);
    free(buffer);
    free(receive);
}


HTTPResponse requests(char* method, char* url_string, char* body) {

    HTTPResponse response = HTTPResponse_new();

    URL url = URL_new(url_string);
    TCPConnection conn = TCPConnection_new(url.host, url.port);

    HTTPRequest request = HTTPRequest_new();
    HTTPRequest_set_request_line(&request, method, url.path, "HTTP/1.0");
    HTTPRequest_set_body(&request, body);

    HTTP http = HTTP_new(&conn, &request, &response);

    HTTP_send(&http);
    HTTP_receive(&http);

    HTTP_drop(&http);
    HTTPRequest_drop(&request);
    TCPConnection_drop(&conn);
    URL_drop(&url);

    return response;

}

#endif //LIBREQUESTS_HTTP_H
