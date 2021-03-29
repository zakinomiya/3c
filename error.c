#include <stdio.h>

#include "ccc.h"

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  // int pos = loc - linestr;
  // fprintf(stderr, "error at LINE=%d, COL=%d\n", linenum, pos);
  // fprintf(stderr, "%s\n", linestr);
  // fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

