//
// Created by taylor on 22-11-17.
//
#include <assert.h>
#include "librequests.h"

int test_tcp() {
    char* data = "echo";
    TCPConnection conn = TCPConnection_new("127.0.0.1", 80);
    TCPConnection_send(&conn, data);
    char* receive = calloc(BUFFSIZE, sizeof(char*));
    char* buffer = calloc(BUFFSIZE, sizeof(char*));

    TCPConnection_receive(&conn, buffer);
    receive = (char*) realloc(receive, strlen(receive) + BUFFSIZE + 1);
    strcat(receive, buffer);
    printf("receive:\n%s\n", buffer);
    printf("data len: %lu\n", strlen(data));
    printf("receive len:%lu\n", strlen(receive));
    free(buffer);
    free(receive);
    TCPConnection_drop(&conn);
    return 0;
}

int test_http_request_message() {
    HTTPRequest request = HTTPRequest_new();
    HTTPRequest_set_request_line(&request, "GET", "/", "HTTP/1.0");
    hashmap_put(&request.headers, "Accept", "text/html");
    hashmap_put(&request.headers, "Connection", "keep-alive");
    hashmap_put(&request.headers, "Accept-Charset", "utf-8");
    hashmap_put(&request.headers, "Cache-Control", "no-cache");
    hashmap_put(&request.headers, "User-Agent", "librequests");

    HTTPRequest_set_body(&request, "hi");
    char* message = HTTPRequest_as_message(&request);
    printf("%s\n", message);
    HTTPRequest_drop(&request);
    free(message);
}

int test_get() {

    HTTPRequest request = HTTPRequest_new();
    HTTPRequest_set_request_line(&request, "GET", "/", "HTTP/1.0");
    hashmap_put(&request.headers, "Accept", "application/json");
    hashmap_put(&request.headers, "Connection", "keep-alive");
    hashmap_put(&request.headers, "Accept-Charset", "utf-8");
    hashmap_put(&request.headers, "Cache-Control", "no-cache");
    hashmap_put(&request.headers, "User-Agent", "librequests");
    HTTPRequest_set_body(&request, "  ");

    HTTPResponse response = HTTPResponse_new();
    TCPConnection conn = TCPConnection_new("127.0.0.1", 80);
    HTTP http = HTTP_new(&conn, &request, &response);
    HTTP_send(&http);
    HTTP_receive(&http);
    printf("version: %s\n", response.status_line.version);
    printf("status code: %lu\n", response.status_line.status_code);
    printf("reason: %s\n", response.status_line.reason_phrase);

    printf("Content-Type: %s\n", hashmap_get(&response.headers, "Content-Type"));
    printf("Vary: %s\n", hashmap_get(&response.headers, "Vary"));
    printf("Date: %s\n", hashmap_get(&response.headers, "Date"));
    printf("Content-Length: %s\n", hashmap_get(&response.headers, "Content-Length"));
    printf("body: %s", response.body);

    TCPConnection_drop(&conn);
    HTTPRequest_drop(&request);
    HTTPResponse_drop(&response);

}

int test_parse_header() {
    char* header = "HTTP/1.0 200 OK\r\nContent-Type: application/json; charset=utf-8\r\nVary: Origin\r\nDate: Mon, 21 Nov 2022 16:03:15 GMT\r\nContent-Length: 18\r\nConnection: keep-alive\r\n\r\n{\"hello\":\"clash\"}\n";
    ulong header_size = 0;
    HTTPResponse response = HTTPResponse_new();
    header_size = parse_header(header, &response);
    printf("version: %s\n", response.status_line.version);
    printf("status code: %lu\n", response.status_line.status_code);
    printf("reason: %s\n", response.status_line.reason_phrase);

    printf("Content-Type: %s\n", hashmap_get(&response.headers, "Content-Type"));
    printf("Vary: %s\n", hashmap_get(&response.headers, "Vary"));
    printf("Date: %s\n", hashmap_get(&response.headers, "Date"));
    printf("Content-Length: %s\n", hashmap_get(&response.headers, "Content-Length"));
    printf("header size: %zd\n", header_size);
    HTTPResponse_drop(&response);
}

int test_parse_url() {

    URL test1 = URL_new("http://www.test1.com");
    assert(test1.use_tls == -1);
    assert(strcmp(test1.host, "www.test1.com") == 0);
    assert(test1.port == 80);
    assert(strcmp(test1.path, "/") == 0);
    assert(test1.auth_user == NULL);
    assert(test1.auth_pwd == NULL);

    URL test2 = URL_new("www.test2.com");
    assert(test2.use_tls == -1);
    assert(strcmp(test2.host, "www.test2.com") == 0);
    assert(test2.port == 80);
    assert(strcmp(test2.path, "/") == 0);
    assert(test2.auth_user == NULL);
    assert(test2.auth_pwd == NULL);

    URL test3 = URL_new("https://test3.com");
    assert(test3.use_tls == 1);
    assert(strcmp(test3.host, "test3.com") == 0);
    assert(test3.port == 443);
    assert(strcmp(test3.path, "/") == 0);
    assert(test3.auth_user == NULL);
    assert(test3.auth_pwd == NULL);

    URL test4 = URL_new("http://user:pwd@www.test4.com");
    assert(test4.use_tls == -1);
    assert(strcmp(test4.host, "www.test4.com") == 0);
    assert(test4.port == 80);
    assert(strcmp(test4.path, "/") == 0);
    assert(strcmp(test4.auth_user, "user") == 0);
    assert(strcmp(test4.auth_pwd, "pwd") == 0);

    URL test5 = URL_new("http://www.test5.com/");
    assert(test5.use_tls == -1);
    assert(strcmp(test5.host, "www.test5.com") == 0);
    assert(test5.port == 80);
    assert(strcmp(test5.path, "/") == 0);
    assert(test5.auth_user == NULL);
    assert(test5.auth_pwd == NULL);

    URL test6 = URL_new("http://www.test6.com/hi");
    assert(test6.use_tls == -1);
    assert(strcmp(test6.host, "www.test6.com") == 0);
    assert(test6.port == 80);
    assert(strcmp(test6.path, "/hi") == 0);
    assert(test6.auth_user == NULL);
    assert(test6.auth_pwd == NULL);

    URL test7 = URL_new("www.test7.com:443/hi");
    assert(test7.use_tls == 1);
    assert(strcmp(test7.host, "www.test7.com") == 0);
    assert(test7.port == 443);
    assert(strcmp(test7.path, "/hi") == 0);
    assert(test7.auth_user == NULL);
    assert(test7.auth_pwd == NULL);

    URL test8 = URL_new("http://www.test8.com:443");
    assert(test8.use_tls == -1);
    assert(strcmp(test8.host, "www.test8.com") == 0);
    assert(test8.port == 443);
    assert(strcmp(test8.path, "/") == 0);
    assert(test8.auth_user == NULL);
    assert(test8.auth_pwd == NULL);

    URL test9 = URL_new("_:pwd@test9:443/");
    assert(test9.use_tls == 1);
    assert(strcmp(test9.host, "test9") == 0);
    assert(test9.port == 443);
    assert(strcmp(test9.path, "/") == 0);
    assert(strcmp(test9.auth_user, "_") == 0);
    assert(strcmp(test9.auth_pwd, "pwd") == 0);

}

int test_requests() {
    HTTPResponse response = requests("GET", "http://127.0.0.1/", "");

    if (response.status_line.status_code >= 200) {
        printf("%s", response.body);
    }

    HTTPResponse_drop(&response);
}

int main(void) {
    test_http_request_message();
    test_tcp();
    test_get();
    test_parse_header();
    test_parse_url();
    test_requests();
    return 0;
}


