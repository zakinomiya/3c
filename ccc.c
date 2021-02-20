#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
};

Token *token;

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// if token is the same as op, return true; otherwise return false.
bool consume(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op) {
    return false;
  }

  token = token->next;
  return true;
}

// if token is the same as op, read the next token; otherwise return error
void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op) {
    error("'%c' not given", op);
  }
  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM) {
    error("Not a number");
  }
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() { return token->kind == TK_EOF; }

Token *new_token(TokenKind kind, Token *cur, char *str) {
  // allocate memory address to the new token
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  // new token as the next token of teh current one
  cur->next = tok;
  return tok;
}

// Split a string into tokens
Token *tokenize(char *p) {
  // init head token
  Token head;
  head.next = NULL;
  // set head as the current token
  Token *cur = &head;

  while (*p) {
    // skip space
    if (isspace(*p)) {
      p++;
      continue;
    }

    // if next letter is '+' or '-' create token and set the new one as current
    if (*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    // if next letter is numeric, create a new token and set the value
    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("Failed to tokenize");
  }

  // create EOF token
  new_token(TK_EOF, cur, p);
  return head.next;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("Invalid argument number");
    return 1;
  }

  token = tokenize(argv[1]);

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  printf("  mov rax, %d\n", expect_number());

  while (!at_eof()) {
    if (consume('+')) {
      printf("  add rax, %d\n", expect_number());
      continue;
    }

    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }

  printf("  ret\n");
  return 0;
}
