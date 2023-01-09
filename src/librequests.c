#include "librequests.h"

void fatal (char *mess)
{
  perror (mess);
  exit (1);
}

void notimplemented ()
{
  perror ("notimplemented");
  exit (1);
}

/**
 * get a a copy of slice from start_ptr to end_ptr
 * @param start_ptr
 * @param end_ptr
 * @return new char area
 */
char *strslice (char *start_ptr, char *end_ptr)
{
  ulong target_size = strlen (start_ptr) - strlen (end_ptr);
  char *target = calloc (1, target_size + 1);
  strncpy (target, start_ptr, target_size);
  return target;
}