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
  Token head;
  char *user_input = argv[1];
  tokenize(&head, user_input);
  prog->tok = head.next;

  parse(prog);
  print_main();
  print_prologue(16);
  for (int i = 0; prog->code[i]; i++) {
    gen(prog->code[i]);
    printf("  pop rax\n");
  }

  print_epilogue();
  return 0;
}

