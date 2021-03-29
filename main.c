#include <stdio.h>
#include <stdlib.h>

#include "ccc.h"

Program *prog;

int main(int argc, char **argv) {
  if (argc != 2) {
    error("Invalid argument number");
    return 1;
  }

  prog = calloc(1, sizeof(Program));
  prog->input = argv[1];
  prog->tok = tokenize(prog->input);
  parse(prog);

  print_prologue(16);
  for (int i = 0; prog->code[i]; i++) {
    gen(prog->code[i]);
    printf("  pop rax\n");
  }

  print_epilogue();
  return 0;
}

