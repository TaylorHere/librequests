#ifndef LIBREQUESTS_LIBREQUESTS_H
#define LIBREQUESTS_LIBREQUESTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tcp.h"
#include "http.h"

void fatal (char *mess);

void notimplemented ();

/**
 * get a a copy of slice from start_ptr to end_ptr
 * @param start_ptr
 * @param end_ptr
 * @return new char area
 */
char *strslice (char *start_ptr, char *end_ptr);

#endif //LIBREQUESTS_LIBREQUESTS_H
