#ifndef LIBREQUESTS_LIBREQUESTS_H
#define LIBREQUESTS_LIBREQUESTS_H


#include <stddef.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <asm-generic/errno.h>
#include <errno.h>
#include <stdbool.h>
#include "hashmap.h"

void fatal(char* mess) {
    perror(mess);
    exit(1);
}

void notimplemented() {
    perror("notimplemented");
    exit(1);
}

/**
 * get a a copy of slice from start_ptr to end_ptr
 * @param start_ptr
 * @param end_ptr
 * @return new char area
 */
char* strslice(char* start_ptr, char* end_ptr) {
    ulong target_size = strlen(start_ptr) - strlen(end_ptr);
    char* target = calloc(1, target_size + 1);
    strncpy(target, start_ptr, target_size);
    return target;
}

#include "tcp.h"
#include "http.h"

#endif //LIBREQUESTS_LIBREQUESTS_H
